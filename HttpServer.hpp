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

#include <string>
#include <sys/types.h>
#include "HttpAgent.hpp"

enum HttpMethod {
    METHOD_GET      = 1 << 0,
    METHOD_POST     = 1 << 1,
    METHOD_DELETE   = 1 << 2,
};

class Location
{
    private:
        std::string index;
        bool        autoindex;
        std::string root;

        // Bit masking
        int         allow_methods;
        std::string cgi_path;
        std::string cgi_ext;
        std::string error_pages[0x255];
};

class HttpServer : public HttpAgent
{
    private :
        HttpServer();
        ssize_t     client_max_body_size;
        int         port;
        std::string interface;
        Location*   locations[0x0F];
        

    public :
        ~HttpServer();
        HttpServer(int socket_fd);
};

