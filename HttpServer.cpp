#include "HttpServer.hpp"

HttpAgent::HttpAgent(int socket_fd)
    : socket_fd(socket_fd)
{
    return;
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
    : fd(fd) , type(type) , owner(nullptr)
{
    return;
}

void Fd::setOwner(HttpAgent* agent)
{
    if(owner)
        return;

    owner = agent;
}

FD_TYPE Fd::getType() const 
{
    return type;
}

int Fd::getFd() const 
{
    return fd;
}
