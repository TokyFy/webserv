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
#include <strings.h>
#include <unistd.h>

HttpServer::HttpServer(int socket_fd)
    : HttpAgent(socket_fd, SERVER) , client_max_body_size(1024) , port(-1),
        interface("0.0.0.0") 
{
    int i = 0;

    while (i < 0x0F) {
        locations[i++] = NULL;
    }

    return;
}

HttpServer::~HttpServer()
{
    close(socket_fd);
    return;
}

ssize_t HttpServer::getClientMaxBodySize() const
{
    return client_max_body_size;
}
        
void    HttpServer::setClientMaxBodySize(ssize_t value)
{
    client_max_body_size = value;
}

int     HttpServer::getPort() const
{
    return port;
}

void    HttpServer::setPort(int value)
{
    port = value;
}

const std::string&  HttpServer::getInterface() const
{
    return interface;
}

void    HttpServer::setInterface(const std::string& value)
{
    interface = value;
}

// Location Object

Location::Location()
    : index("index.html") , autoindex(true) , root("/") , allow_methods(0b111)
{
    error_pages[400] = "/www/400.html";

    return;
}

Location::~Location()
{
    (void)(allow_methods);
    return;
}

const std::string& Location::getIndex() const {
    return index;
}

void Location::setIndex(const std::string& value) {
    index = value;
}

bool Location::getAutoIndex() const {
    return autoindex;
}

void Location::setAutoIndex(bool value) {
    autoindex = value;
}

const std::string& Location::getRoot() const {
    return root;
}

void Location::setRoot(const std::string& value) {
    root = value;
}
