/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpServer.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/15 21:17:55 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 09:04:27 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <map>
#include <string>
#include <sys/types.h>
#include <vector>
#include "HttpAgent.hpp"

enum HttpMethod {
    METHOD_GET      = 1 << 0,
    METHOD_POST     = 1 << 1,
    METHOD_DELETE   = 1 << 2,
};

class Location
{
    private:
        std::string source;
        std::string index;
        bool        autoindex;
        std::string root;

        // Bit masking
        int         allow_methods;
        std::string cgi_path;
        std::string cgi_ext;

    public:
        Location();
        ~Location();

        const std::string&  getSource() const;
        void                setSource(const std::string& value);
        const std::string&  getIndex() const;
        void                setIndex(const std::string& value);
        bool                getAutoIndex() const;
        void                setAutoIndex(bool value);
        const std::string&  getRoot() const;
        void                setRoot(const std::string& value);
};

class HttpServer : public HttpAgent
{
    private :
        HttpServer();
        std::string name;
        ssize_t     client_max_body_size;
        int         port;
        std::string interface;
        std::vector<Location> location;
        std::map<int , std::string> error_pages;
        

    public :
        ~HttpServer();
        HttpServer(int socket_fd);
        HttpServer(int socket_fd , std::string name);

        ssize_t             getClientMaxBodySize() const;
        void                setClientMaxBodySize(ssize_t value);
        int                 getPort() const;
        void                setPort(int value);
        const std::string&  getInterface() const;
        void                setInterface(const std::string& value);
        void                setErrorPage(int code , std::string path);
        const std::string&  getErrorPage(int code);

        void addLocation(Location& location);
        void getLocation(std::string& path);
};

