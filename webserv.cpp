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
#include <ctime>
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

                HttpClient *new_client = new HttpClient(client_fd , event.data.fd);
                new_client->setTime(std::time(NULL));

                httpAgentPool.add(client_fd, new_client);
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

                    if(client->getTimeOut() >= 10)
                        client->setState(SEND_EOF);
                }
                else if (t == WRITE)
                {
                    FILE_TYPE t = BINARY;
                    int code = 0;
                    int file_fd = -1;

                    if(client->getFileFd() == -1)
                    {
                        std::string path = "." + getRequestPath(client->getRawHeaders());
                        t = mime(path);

                        if(t == ERR_DENIED || t == ERR_NOTFOUND)
                        {
                            file_fd = open("./www/400.html" , O_RDONLY);
                            code = 400;
                        }
                        else if (t == FOLDER)
                        {
                            file_fd = indexof(path.c_str());
                            code = 200;
                        }
                        else {
                            file_fd = open(path.c_str() , O_RDONLY);
                            code = 200;
                        }
                        
                        client->setFileFd(file_fd);
                        
                        std::cerr << " " << code << " | " << "GET " << path << std::endl; 
                    }
                    std::string header = header_builder(code , t); 
                    ssize_t sended = send(client_fd , header.c_str() , header.size() , MSG_NOSIGNAL);
                    
                    if(sended <= 0)
                    {
                        client->setState(SEND_EOF);
                        continue;
                    }

                    client->setState(SEND_DATA);
                    continue;
                }
                else if (t == SEND_DATA)
                {
                    char buffer[CHUNK_SIZE];
                    int fd = client->getFileFd();

                    if(fd == -1)
                    {
                        client->setState(SEND_EOF);
                        continue;
                    }

                    std::memset(buffer , 0 , sizeof(buffer));
                    ssize_t readed = read(fd, buffer, sizeof(buffer));

                    if(readed > 0)
                    {
                        std::string chunk = to_chuncked(buffer, readed);                        
                        ssize_t sended = send(client_fd , chunk.c_str() , chunk.size() , MSG_NOSIGNAL | MSG_DONTWAIT);

                        if(sended < 0)
                            client->setState(SEND_EOF);

                        continue;
                    }

                    if(readed == 0)
                    {
                        client->setTime(std::time(NULL));
                        send(client_fd, "0\r\n\r\n", 5 , MSG_DONTWAIT | MSG_NOSIGNAL);
                        client->setState(SEND_EOF);
                        continue;
                    }
                }
                else if(t == SEND_EOF)
                {
                    if(client->getTimeOut() >= 5 )
                    {
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
