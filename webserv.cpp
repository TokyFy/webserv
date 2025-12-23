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
#include <exception>
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
#include <vector>
#include "utils.hpp"
#include "config.hpp"

#define CHUNK_SIZE 8 * 1024

int main(int argc , char **argv) {

    if(argc != 2)
        return EXIT_FAILURE;
    
    std::vector<HttpServer> servers = parser(argv[1]);
	    
    std::vector<HttpServer>::iterator it = servers.begin();


    Pool            httpAgentPool;
    int epoll = epoll_create(1);
    
    try 
    {
        while (it != servers.end()) {
            it->setToEppoll(epoll);
            httpAgentPool.add(it->getSockeFd(), &(*it));
            it++;
        }
    } catch (std::exception &e)
    {
        std::cout << "Error :" << e.what() << std::endl;
        return 1;
    }

    struct epoll_event all_events[0xFF];

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
                new_client->setServer(dynamic_cast<HttpServer*>(agent));
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

                    if(!client->isHeaderFull())
                    {
                        client->appendRawHeader(buffer, readed);
                        continue;
                    }

                    if(client->getTimeOut() >= 10)
                        client->setState(SEND_EOF);

                    client->setState(WRITE);
                }
                else if (t == WRITE)
                {
                    FILE_TYPE t = BINARY;
                    int code = 0;
                    int file_fd = -1;

                    if(client->getFileFd() == -1)
                    {
                        std::string method = getRequestMethod(client->getRawHeaders());
                        std::string path   = getRequestPath(client->getRawHeaders());

                        // Only simple GET handling for now
                        if (method != "GET")
                        {
                            code = 405;
                            t = HTML;
                            file_fd = error_page_builder(code);
                        }
                        else
                        {
                            // Enforce location allowed methods
                            Location loc = client->getServer()->getLocation(path);
                            if (!loc.isAllowedMethod("GET"))
                            {
                                code = 405;
                                t = HTML;
                                file_fd = error_page_builder(code);
                            }
                            else
                            {
                                file_fd = client->openFile(path , code , t);
                                if(file_fd == -1)
                                    file_fd = error_page_builder(500);
                            }
                        }
                        client->setFileFd(file_fd);
                        
                        std::cerr << " " << code << " | " << method << " " << path << std::endl; 
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

	return 0;
}
