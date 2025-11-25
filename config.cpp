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

#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

enum TokenType {
    // Structural tokens
    TOKEN_LBRACE,               // {
    TOKEN_RBRACE,               // }
    TOKEN_SEMICOLON,            // ;
    
    // Comments
    TOKEN_COMMENT,              // # This is a comment

    // Keywords (directives)
    TOKEN_LOCATION,             // location
    TOKEN_LISTEN,               // listen
    TOKEN_ROOT,                 // root
    TOKEN_INDEX,                // index
    TOKEN_CLIENT_MAX_BODY_SIZE, // client_max_body_size
    TOKEN_ERROR_PAGE,           // error_page
    TOKEN_ALLOW_METHODS,        // allow_methods
    TOKEN_AUTOINDEX,            // autoindex
    TOKEN_CGI_PATH,             // cgi_path
    TOKEN_CGI_EXT,              // cgi_ext

    // Values
    TOKEN_WORDS
};

enum PARSER_STATE { NONE , SERVER , LOCATION };

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

            if( *c == '\n' || *c == ';')
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
