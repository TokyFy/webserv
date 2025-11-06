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

HttpClient::HttpClient(int socket_fd , int server_id)
    : HttpAgent(socket_fd, CLIENT) , server_id(server_id)
{
    return;
}
