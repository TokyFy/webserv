#include "HttpServer.hpp"
#include <cstddef>

HttpAgent::HttpAgent(int socket_fd)
    : socket_fd(socket_fd)
{
    return;
}

int HttpAgent::getSockeFd() const
{
    return socket_fd;
}

HttpServer::HttpServer(int socket_fd)
    : HttpAgent(socket_fd)
{
    return;
}

HttpClient::HttpClient(int socket_fd)
    : HttpAgent(socket_fd)
{
    return;
}

Fd::Fd(int fd , FD_TYPE type)
    : fd(fd) , type(type) , owner(NULL)
{
    return;
}

void Fd::setOwner(HttpAgent* agent)
{
    if(owner)
        return;

    owner = agent;
}

HttpAgent* Fd::getOwner()
{
    return owner;
}

FD_TYPE Fd::getType() const 
{
    return type;
}

int Fd::getFd() const 
{
    return fd;
}
