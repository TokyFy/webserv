/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 22:16:05 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/09 00:42:03 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpAgent.hpp"
#include "HttpServer.hpp"
#include "HttpClient.hpp"
#include "Pool.hpp"
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <cstdio>
#include <iostream>
#include "utils.hpp"

#define CHUNK_SIZE 8 * 1024


int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	if (bind(server_fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) == -1)
    {
		return 1;
    }

	listen(server_fd, 5);

	int epoll = epoll_create(1);
	struct epoll_event server_event;
	server_event.events = EPOLLIN;
	server_event.data.fd = server_fd;

	epoll_ctl(epoll, EPOLL_CTL_ADD, server_fd , &server_event);

	struct epoll_event all_events[0xFF];


    Pool            httpAgentPool;
    httpAgentPool.add(server_fd, new HttpServer(server_fd));


	while (true) {
		int queue = epoll_wait(epoll, all_events, 0xFF, 0);
        
        for(int i = 0 ; i < queue ; i++)
        {
            struct epoll_event event = all_events[i];

            HttpAgent* agent = httpAgentPool.pull(event.data.fd);

            if(agent->getType() == SERVER)
            {
                int client_fd = accept( event.data.fd, NULL , NULL);
                struct epoll_event client_event;
                client_event.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP;
                client_event.data.fd = client_fd;
                epoll_ctl(epoll , EPOLL_CTL_ADD , client_fd , &client_event);

                httpAgentPool.add(client_fd, new HttpClient(client_fd , event.data.fd));

                std::cerr << "ID " << client_fd << " connected" << std::endl;
                continue;
            }
            else
            {
                HttpClient* client = dynamic_cast<HttpClient*>(httpAgentPool.pull(event.data.fd));

                int client_fd = client->getSockeFd();
                
                STATE t = client->getState();
                
                if(t == READ)
                {
                    char buffer[CHUNK_SIZE];
                    std::memset(buffer , 0 , CHUNK_SIZE);

                    ssize_t readed = recv(client_fd, buffer, sizeof(buffer), MSG_NOSIGNAL | MSG_DONTWAIT); 
                    
                    if(readed > 0)
                    {
                        client->appendRawHeader(buffer, readed);
                        client->setState(WRITE);
                        continue;
                    }
                }
                else if (t == WRITE)
                {
                    FILE_TYPE t;
                    int code;
                    int file_fd;

                    if(client->getFileFd() == -1)
                    {
                        std::string path = "./" + getRequestPath(client->getRawHeaders());

                        std::cerr << path << std::endl;

                        t = mime(path);

                        if(t == ERR_DENIED || t == ERR_NOTFOUND || t == FOLDER)
                        {
                            file_fd = open("./www/400.html" , O_RDONLY);
                            code = 400;
                        }
                        else {
                            file_fd = open(path.c_str() , O_RDONLY);
                            code = 200;
                        }

                        client->setFileFd(file_fd);
                    }

                    std::string header = header_builder(code , t); 
                    send(client_fd , header.c_str() , header.size() , MSG_NOSIGNAL);
                    client->setState(SEND_DATA);
                    continue;
                }
                else if (t == SEND_DATA)
                {
                    int fd = client->getFileFd();

                    char buffer[CHUNK_SIZE];
                    std::memset(buffer , 0 , sizeof(buffer));
                    
                    ssize_t readed = read(fd, buffer, sizeof(buffer));

                    if(readed > 0)
                    {
                        std::string len = hex(readed);
                        // REMOVE THIS FOR THE SHAKE OF 42
                        send(client_fd , len.c_str() , len.size() , MSG_DONTWAIT | MSG_NOSIGNAL);
                        send(client_fd , "\r\n" , 2 , MSG_DONTWAIT | MSG_NOSIGNAL);
                        send(client_fd , buffer , readed , MSG_DONTWAIT | MSG_NOSIGNAL);
                        send(client_fd , "\r\n" , 2 , MSG_DONTWAIT | MSG_NOSIGNAL);
                        
                        continue;
                    }

                    if(readed == 0)
                    {
                        client->setState(SEND_EOF);
                        continue;
                    }
                }
                else if(t == SEND_EOF)
                {
                    ssize_t sent = send(client_fd, "0\r\n\r\n", 5 , MSG_DONTWAIT | MSG_NOSIGNAL);
                
                    if(sent <= 0)
                    {
                        std::cout << "ID " << client_fd << " closed " << std::endl;
                        client->setState(CLOSED);
                    }

                    continue;
                }
                else if(t == CLOSED)
                {
                    httpAgentPool.erase(client_fd);
                }

            }
        }

	}

	close(server_fd);
	return 0;
}
