/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   serv.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 22:16:05 by franaivo          #+#    #+#             */
/*   Updated: 2025/09/15 22:53:51 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include "HttpServer.hpp"
#include <fcntl.h>
#include <cstdio>

int DEBUG = 0;

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
	server_event.data.ptr = new Fd(server_fd , SERVER_FD);

	epoll_ctl(epoll, EPOLL_CTL_ADD, server_fd , &server_event);

	struct epoll_event all_events[0xA];

	while (true) {
        // Clean all 
		int queue = epoll_wait(epoll, all_events, 0xA, 0);

		for(int i = 0 ; i < queue ; i++)
		{
            struct epoll_event event = all_events[i];
            Fd *fd_object =  (Fd*)event.data.ptr;

			if(fd_object->getType() == SERVER_FD)
			{
				int client_fd = accept(server_fd, NULL, NULL);
				struct epoll_event client_event;
				client_event.events = EPOLLIN;
				client_event.data.ptr = new Fd(client_fd , CLIENT_FD);
				epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &client_event);
                continue;
			}

            if (fd_object->getType() == CLIENT_FD)
			{
                int static_fd = open("./www/index.html" , O_RDONLY);

                std::string respone = "HTTP/1.1 200 OK\r\nTransfer-Encoding: chunked\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
                send( fd_object->getFd(), respone.c_str() , respone.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
                
                Fd* file_fd = new Fd(static_fd , OUT_FILE_FD);
                file_fd->setOwner(new HttpClient(fd_object->getFd()));

                event.events = EPOLLOUT;
                event.data.ptr = file_fd;
                epoll_ctl(epoll , EPOLL_CTL_MOD , fd_object->getFd() , &event);
                delete fd_object;
                continue;
			}

            if(fd_object->getType() == OUT_FILE_FD)
            {
                HttpClient* client = (HttpClient*)(fd_object->getOwner());

                char buffers[ 16 * 1024 ]; 
                std::memset(buffers , 0 , 16 * 1024);
                int readed = read(fd_object->getFd(), buffers, 16 * 1024);

                if(readed <= 0)
                {
                    std::cout << "end ! " << DEBUG++ << std::endl;
                    std::string response = "0\r\n\r\n";
                    send( client->getSockeFd() , response.c_str(), response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);

                    epoll_ctl( epoll, EPOLL_CTL_DEL, client->getSockeFd() , NULL);
                }
                else 
                {
                    std::string response;

                    char sizeBuf[32];
                    std::memset(sizeBuf , 0 , 32 );
                    int len = std::snprintf(sizeBuf, sizeof(sizeBuf), "%X\r\n", (uint)readed);

                    response.append(sizeBuf, len);
                    response.append(buffers, readed);
                    response.append("\r\n");
                    send( client->getSockeFd() , response.c_str(), response.size(), MSG_DONTWAIT | MSG_NOSIGNAL);
                }
            }
		}
	}

	close(server_fd);
	return 0;
}
