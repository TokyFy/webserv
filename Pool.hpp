/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pool.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/06 09:05:19 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/06 09:55:28 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include "HttpAgent.hpp"

class Pool 
{
    private:
        std::map<int, HttpAgent*> agents; 

    public:
        Pool();
        ~Pool();
        void add(unsigned int id , HttpAgent *agent);
        HttpAgent* pull(unsigned int id) const;
};
