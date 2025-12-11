/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hrasoamb < hrasoamb@student.42antananar    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/19 14:32:09 by hrasoamb          #+#    #+#             */
/*   Updated: 2025/12/06 09:18:58 by hrasoamb         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

# include <map>
# include <string>
# include "utils.hpp"

enum { GET, POST, DELETE };

class HttpResponse;

class HttpRequest
{
    public:
        std::string     methodName;
        std::string     path; 
        std::string     protocol;
        std::map<std::string, std::string> headers;
        std::string     body;
        bool            full;
        bool            fullHeader;

        HttpRequest(void);
        HttpRequest &operator=(HttpRequest const &to_what);
        ~HttpRequest(void);
        int         parse(std::string request);
        void        parseHeaders(std::string headers);
        int         parseHead(std::string head, HttpResponse &response);
        void        fillPair(std::string pair);
        std::string getPair(std::string entry);
};

#endif
