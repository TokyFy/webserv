/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasoamb < hrasoamb@student.42antananar    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/22 15:16:08 by hrasoamb          #+#    #+#             */
/*   Updated: 2025/12/04 14:21:52 by hrasoamb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

# include <iostream>
# include <cstdio>
# include <fstream>
# include <string>
# include <unistd.h>
# include <map>
# include <iostream>
# include "HttpRequest.hpp"

class HttpRequest;

class HttpResponse
{
    public:
        std::string method;
        std::string protocol;
        int         statusCode;
        std::string body;
        std::string responseHeader;
        std::string contentType;
        std::string connection;
        int         contentLength;
        std::string response;
        std::string mimeType;
        bool        full;
        bool        header;
        std::ifstream   file;
        ssize_t      lastRead;

        HttpResponse(void);
        ~HttpResponse(void);
        HttpResponse &operator=(HttpResponse const &to_what);
        std::string createHeader(void);
        std::string getStatusMessage(void);
        std::string getBody(void);
        std::string createResponse(HttpRequest request);
        std::string rightMethod(void);
        void        deleteMethod(std::string path);
        std::string getPairs(HttpRequest request);
        std::string getRightMimeType(std::string path);
        int         getContentLenght(std::string path);
        std::string getContentType(void);
        void        openFile(std::string path);
};

#endif