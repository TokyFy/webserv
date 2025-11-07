/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 08:44:08 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 09:00:44 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpClient.hpp"
#include "HttpAgent.hpp"
#include <stdexcept>
#include <unistd.h>

HttpClient::HttpClient(int socket_fd , int server_id)
    : HttpAgent(socket_fd, CLIENT) , state(READ) , server_id(server_id) , rawHeaders("") , file_fd(-1)
{
    return;
}

HttpClient::~HttpClient()
{
    if(file_fd != -1)
        close(file_fd);
    close(socket_fd);
}

STATE HttpClient::getState() const 
{
    return state;
}


void HttpClient::setState(STATE t)
{
    state = t;
}

int HttpClient::getFileFd() const
{
    return  file_fd;
}

void HttpClient::setFileFd(int fd)
{
    if(file_fd != -1)
        throw std::runtime_error("Attempting to overate fd");
    file_fd = fd;
}
