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
#include "HttpAgent.hpp"

class HttpServer : public HttpAgent
{
    private :
        std::string root;

        HttpServer();

    public :
        ~HttpServer();
        HttpServer(int socket_fd);
};

