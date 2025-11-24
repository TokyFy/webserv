/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/22 21:37:01 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/23 00:53:14 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <string>

int main(int argc , char **argv)
{
    if(argc > 2)
    {
        std::cerr << "Usage : webserv [ config.conf ]" << std::endl;
        return 0;
    }

    std::ifstream config(argv[1]);

    if(!config.is_open())
    {
        std::cerr << "Error : could not read the config" << std::endl;
        return 1;
    }

    std::string token;

    while (std::getline(config , token , ' '))
    {
        std::cerr << token << std::endl;
    }

    config.close();
    return 0;
}
