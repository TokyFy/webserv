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
#include "HttpServer.hpp"
#include "utils.hpp"
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <fcntl.h>


HttpClient::HttpClient(int socket_fd , int server_id)
    : HttpAgent(socket_fd, CLIENT) , state(READ) , server_id(server_id) , file_fd(-1) , time(0) , full_header(false)
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


void    HttpClient::appendRawHeader(const char* buff, size_t len)
{
    if(rawHeaders.size() == 0)
        rawHeaders = buff;
    else
        rawHeaders.append(buff , len);

    if(rawHeaders.find("\r\n\r\n") != std::string::npos)
    {
        full_header = true;
    }
}


const std::string& HttpClient::getRawHeaders() const
{
    return rawHeaders;
}

void HttpClient::setTime(std::time_t t)
{
    time = t;
}

std::time_t HttpClient::getTimeOut() const 
{
    return std::time(NULL) - time;
}

bool HttpClient::isHeaderFull() const
{
    return full_header;
}
int HttpClient::getServerId() const
{
    return server_id;
}

void HttpClient::setServer(HttpServer *ptr) 
{
    server = ptr;
}

std::string replaceFirstOccurrence(const std::string &originalString, const std::string &target, const std::string &replacement)
{
    if (target.empty())
        return originalString;

    std::string resultString = originalString;
    std::string::size_type position = resultString.find(target);

    if (position != std::string::npos)
        resultString.replace(position, target.length(), replacement);

    return resultString;
}

int HttpClient::openFile(std::string path , int &code , FILE_TYPE& type) const
{ 
    // source -> /public
    // root   -> /www
    // path   -> /public/index.html -> /www/index.html
    // path   -> /public/folder 

    std::string npath = "." + server->getRoutedPath(path);
    Location location = server->getLocation(path);

    std::cerr << "OPENFILE : " << npath << std::endl;

    FILE_TYPE t = mime(npath);

    if(t == ERR_DENIED)
    {
        code = 403;
        type = HTML;
        return error_page_builder(code);
    }

    if(t == ERR_NOTFOUND)
    {
        type = HTML;
        code = 403;
        return error_page_builder(code);
    }

    if (t == FOLDER)
    {
        code = 404;
        type = HTML;

        if(!location.getAutoIndex())
        {
            if(location.getIndex().size() == 0)
                return -1;

            int index = this->openFile(path + location.getIndex() , code , type);
        
            if(index > 0 && code == 200)
                return index;
        }

        code = 200;
        type = HTML;
        return indexof(location , path);
    }
   
    code = 200;
    type = t;
    return open(npath.c_str() , O_RDONLY);
}
