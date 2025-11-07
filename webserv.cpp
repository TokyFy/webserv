/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 22:16:05 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 10:24:07 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpAgent.hpp"
#include "HttpServer.hpp"
#include "HttpClient.hpp"
#include "Pool.hpp"
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

#define CHUNK_SIZE 32 * 1024

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
                client_event.events = EPOLLIN;
                client_event.data.fd = client_fd;
                epoll_ctl(epoll , EPOLL_CTL_ADD , client_fd , &client_event);

                httpAgentPool.add(client_fd, new HttpClient(client_fd , event.data.fd));
                continue;
            }
            else
            {
                HttpClient* client = dynamic_cast<HttpClient*>(httpAgentPool.pull(event.data.fd));

                int client_fd = client->getSockeFd();

                std::cout << client_fd << std::endl;

                std::string greeting =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 12\r\n"
                "\r\n"
                "Hello world !";


                std::cout << send( client_fd , greeting.c_str(), greeting.size(), MSG_DONTWAIT | MSG_NOSIGNAL) << std::endl;
                httpAgentPool.erase(client_fd);
            }
        }

	}

	close(server_fd);
	return 0;
}
