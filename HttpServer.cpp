/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 09:01:24 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 09:03:46 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpServer.hpp"
#include "HttpAgent.hpp"
#include <unistd.h>

HttpServer::HttpServer(int socket_fd)
    : HttpAgent(socket_fd, SERVER)
{
    return;
}

HttpServer::~HttpServer()
{
    close(socket_fd);
    return;
}
