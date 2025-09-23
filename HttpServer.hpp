/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 21:17:55 by franaivo          #+#    #+#             */
/*   Updated: 2025/09/15 23:37:37 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <map>
#include <string>

enum FD_TYPE { IN_FILE_FD , OUT_FILE_FD , SERVER_FD , CLIENT_FD };

class HttpAgent {
    protected :
        int socket_fd;

    public :
        HttpAgent( int socket_fd);
        int getSockeFd() const;
};

class HttpClient : public HttpAgent
{
    private :
        std::string rawHeaders;
        HttpAgent   *server;

    public :
        HttpClient(int socket_fd);
};

class HttpServer : public HttpAgent
{
    private :
        std::string root;
        std::map<int , HttpClient* > clients;

    public :
        HttpServer(int socket_fd);
};


class Fd 
{
    private :
        int fd;
        FD_TYPE type;
        HttpAgent *owner;

    public :
        Fd(int fd , FD_TYPE type);
        void        setOwner(HttpAgent* agent);
        HttpAgent*  getOwner();
        FD_TYPE     getType() const;
        int         getFd() const;
};
