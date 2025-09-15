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

#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <iostream>
#include "HttpServer.hpp"

int main() {
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);

	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	sockaddr_in addr{};
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
	server_event.data.ptr = new Fd();

	epoll_ctl(epoll, EPOLL_CTL_ADD, server_fd , &server_event);

	struct epoll_event all_events[0xA];

	while (true) {
		int queue = epoll_wait(epoll, all_events, 0xA, -1);

		for(int i = 0 ; i < queue ; i++)
		{
            struct epoll_event event = all_events[i];

			if(event.data.fd == server_fd)
			{
				int client_fd = accept(server_fd, nullptr, nullptr);
				struct epoll_event client_event;
				client_event.events = EPOLLIN;
				client_event.data.fd = client_fd;
				epoll_ctl(epoll, EPOLL_CTL_ADD, client_fd, &client_event);
			}
			else
			{
                if(event.events == EPOLLIN)
                {
                    char to_read[0x400];
                    memset(to_read, 0, 0x400);
                    ssize_t read_size = recv(event.data.fd , &to_read , 0x400 , MSG_DONTWAIT | MSG_NOSIGNAL);

                    std::cout << to_read;

                    event.events = EPOLLOUT;
                    epoll_ctl(epoll , EPOLL_CTL_MOD , event.data.fd , &event);
                }
                else
                {
                    std::string respone = "HTTP/1.0 200 OK\r\nContent-Length: 32\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n<html><body>Hello!</body></html>";
                    send(event.data.fd, respone.c_str() , respone.length() , MSG_DONTWAIT | MSG_NOSIGNAL);
                    event.events = EPOLLIN;
                    epoll_ctl(epoll , EPOLL_CTL_MOD , event.data.fd , &event);
                }
			}
		}
	}

	close(server_fd);
	return 0;
}
