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
#include <stdexcept>
#include <string>
#include <strings.h>
#include <unistd.h>

HttpServer::HttpServer(int socket_fd)
    : HttpAgent(socket_fd, SERVER) , name("~ SERVER ~") , client_max_body_size(1024) , port(-1) ,
        interface("0.0.0.0")
{
    return;
}

HttpServer::HttpServer(int socket_fd , std::string name)
    : HttpAgent(socket_fd, SERVER) , name(name) , client_max_body_size(1024) , port(-1) ,
        interface("0.0.0.0")
{
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

const std::string&  HttpServer::getName() const
{
    return name;
}

void    HttpServer::setName(const std::string& value)
{
    name = value;
}

void HttpServer::setErrorPage(int code , std::string path)
{
    if(code <= 0 || code >= 0x255)
        throw std::runtime_error("Invalid error code");
    
    error_pages[code] = path; 
}

const std::string& HttpServer::getErrorPage(int code) {

    (void)(code);

    std::map<int,std::string>::const_iterator it = error_pages.find(code);
    if (it == error_pages.end())
        throw std::runtime_error("Error page not found");

    return it->second;
}


void HttpServer::addLocation(Location& location)
{
    locations.push_back(location);
}


// Location Object

Location::Location()
    : source("/") , index("index.html") , autoindex(true) , root("/") , allow_methods(0b000)
{
    return;
}

Location::~Location()
{
    (void)(allow_methods);
    return;
}

const std::string& Location::getSource() const {
    return source;
}

void Location::setSource(const std::string& value) {
    source = value;
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

const std::string& Location::getCgiPath() const {
    return cgi_path;
}

void Location::setCgiPath(const std::string& value) {
    cgi_path = value;
}

const std::string& Location::getCgiExt() const {
    return cgi_ext;
}

void Location::setCgiExt(const std::string& value) {
    cgi_ext = value;
}

void Location::addAllowedMethod(const std::string & method)
{
    if(method == "GET")
        allow_methods |= (1 << 0);
    else if(method == "POST")
        allow_methods |= (1 << 1);
    else if(method == "DELETE")
        allow_methods |= (1 << 2);
    else 
        throw std::runtime_error("Unkown method " + method);
}

bool Location::isAllowedMethod(const std::string& method)
{
    if(method == "GET")
        return ( allow_methods >> 0 ) & 1;
    else if(method == "POST")
        return ( allow_methods >> 1 ) & 1;
    else if(method == "DELETE")
        return ( allow_methods >> 2 ) & 1;
    else 
        return false;
}
