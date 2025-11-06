/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Poll.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 09:05:16 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 09:54:54 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpAgent.hpp"
#include "Pool.hpp"
#include <cstddef>
#include <stdexcept>
#include <sys/select.h>

Pool::Pool()
{
    return;
}

Pool::~Pool()
{
    return;
}

void Pool::add(unsigned int id , HttpAgent *agent)
{
    if(id >= FD_SETSIZE)
        throw std::runtime_error("FD_SETSIZE violated");

    agents[id] = agent;
    return;
}

HttpAgent* Pool::pull(unsigned int id) const 
{
    if(id >= FD_SETSIZE)
        throw std::runtime_error("FD_SETSIZE violated");
    
    if(agents.find(id) != agents.end())
        return agents.at(id);
    
    return NULL;
}


void Pool::erase(unsigned int id)
{
    HttpAgent* agent = this->pull(id);

    if(!agent)
        return;

    delete agent;
    agents.erase(id);
}
