/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasoamb < hrasoamb@student.42antananar    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:41:34 by hrasoamb          #+#    #+#             */
/*   Updated: 2025/12/06 09:22:45 by hrasoamb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpRequest.hpp"

HttpRequest::HttpRequest(void)
{
    full = false;
    fullHeader = false; 
}

HttpRequest::~HttpRequest(void)
{

}

HttpRequest &HttpRequest::operator=(HttpRequest const &to_what)
{
    if (this != &to_what)
    {
        methodName = to_what.methodName;
        path = to_what.path; 
        protocol = to_what.protocol;
        body = to_what.body;
        headers = to_what.headers;
    }
    return (*this);
}

int    HttpRequest::parseHead(std::string head, HttpResponse &response)
{
    std::stringstream ss(head);
    char del = ' ';
    std::string headers[4];
    int i = 0;
    
    for (; i < 3; i++)
        getline(ss, headers[i], del);
    methodName = headers[0];
    if (!isValidMethod(methodName))
    {
        std::cout << "invalid method" << std::endl;
        return (405);
    }
    path = headers[1];
    std::cout << "\e[1;36mREQUEST PATH : " << path << "\e[0m" << std::endl;
    protocol = headers[2];
    if (getline(ss, headers[i], del) || !isUpper(methodName)
        || !isValidProtocol(protocol) || path[0] != '/')
        return (400);
    response.method = methodName;
    return (0);
}

void    HttpRequest::fillPair(std::string pair)
{
    std::stringstream ss(pair);
    char del = ':';
    std::string p[2];
    std::string trimmed;
    int i = 0;

    if (pair.empty())
        return ;
    for (; i < 2; i++)
        getline(ss, p[i], del);
    trimmed = p[1].substr(p[1].find_first_not_of(' '), (p[1].find_last_not_of(' ') - p[1].find_first_not_of(' ') + 1));
    headers.insert(std::pair<std::string, std::string>(p[0], trimmed));
}

void   HttpRequest::parseHeaders(std::string rest)
{
    size_t pos = 0;
    std::string delimiter = "\r\n";
    while ((pos = rest.find(delimiter)) != std::string::npos)
    {
        fillPair(rest.substr(0, pos));
        rest.erase(0, pos + delimiter.length());
    }
    std::map<std::string, std::string>::iterator it;
    // for (it = headers.begin(); it != headers.end(); ++it)
    //     std::cout << it->first << " +:+ " << it->second << std::endl;
}

int    HttpRequest::parse(std::string request)
{
    // std::string head;

    // response.statusCode = 200;
    size_t pos = request.find("\r");
    if (pos == std::string::npos)
    {
        std::cout << "zavatra" << std::endl;
        return (0);
    }
    // head = request.substr(0, pos);
    // if (parseHead(head))
    //     return (1);
    parseHeaders(request.substr(pos + 2));
    // if (crochet != std::string::npos)
    //     body = request.substr(crochet);
    return (0);
}

std::string HttpRequest::getPair(std::string entry)
{
    std::map<std::string, std::string>::iterator it;

    it = headers.find(entry);
    if (it != headers.end())
        return (it->second);
    return ("");
}
