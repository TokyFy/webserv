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

#include <algorithm>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
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

    std::string line;

    while (std::getline(config , line))
    {
        const char *c = line.c_str();
        std::string token;

        while (*c) {
            if(std::isspace(*c))
            {
                std::cerr << token << std::endl; token.clear();
                while (std::isspace(*c)) {
                    c++;
                }
                continue;
            }

            if( *c == '\n' || *c == ';' )
            {
                std::cerr << token << std::endl ; token.clear();
            }

            token += *c;
            c++;
        }

        if(token.length())
            std::cerr << token << std::endl;

    }

    config.close();
    return 0;
}
