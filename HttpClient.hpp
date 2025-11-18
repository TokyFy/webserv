/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 08:25:15 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/09 00:20:39 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpAgent.hpp"
#include <ctime>
#include <string>

enum STATE  { READ , WRITE , SEND_HEADER , PARSING , SEND_DATA , SEND_EOF , ERROR , CLOSED};

class HttpClient : public HttpAgent
{
    private :
        STATE       state;
        int         server_id;
        std::string rawHeaders;
        int         file_fd;
        std::time_t time;

        HttpClient();

    public :
        ~HttpClient();
        HttpClient(int socket_fd , int server_id);
        STATE       getState() const;
        void        setState(STATE);
        int         getFileFd() const;
        void        setFileFd(int fd);
        void        appendRawHeader(const char* , size_t);
        void        setTime(std::time_t t);
        std::time_t getTimeOut() const;
        const std::string& getRawHeaders() const;
};
