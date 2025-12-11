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
#include <stdexcept>
#include <string>
#include <utility>
#include <vector> 
#include <iomanip>
#include "HttpServer.hpp"

struct s_location_config
{
    std::string index;
    bool        autoindex;
    std::string root;
    int         allow_methods;
    std::string cgi_path;
    std::string cgi_ext;
    std::string error_pages[0x255];
};

struct s_server_config
{
    ssize_t     client_max_body_size;
    int         port;
    std::string interface;
    s_location_config*   locations[0x0F];
};

enum TokenType {
                                // Structural tokens
    TOKEN_LBRACE,               // {
    TOKEN_RBRACE,               // }
    TOKEN_SEMICOLON,            // ;
    TOKEN_RETURN,               // \n
    
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
    TOKEN_WORDS,
    TOKEN_EOF
};


unsigned int keyword_hash(const std::string& s)
{
    unsigned int h = 0;

    for (size_t i = 0; i < s.size(); i++)
        h = h * 31u + (unsigned char)s[i];
    return h;
}

const char* token_type_to_string(TokenType type)
{
    switch (type) {
        case TOKEN_ROOT:                  return "TOKEN_ROOT";
        case TOKEN_INDEX:                 return "TOKEN_INDEX";
        case TOKEN_AUTOINDEX:             return "TOKEN_AUTOINDEX";
        case TOKEN_LISTEN:                return "TOKEN_LISTEN";
        case TOKEN_LBRACE:                return "TOKEN_LBRACE";
        case TOKEN_RBRACE:                return "TOKEN_RBRACE";
        case TOKEN_RETURN:                return "TOKEN_RETURN";
        case TOKEN_COMMENT:               return "TOKEN_COMMENT";
        case TOKEN_CGI_EXT:               return "TOKEN_CGI_EXT";
        case TOKEN_CGI_PATH:              return "TOKEN_CGI_PATH";
        case TOKEN_LOCATION:              return "TOKEN_LOCATION";
        case TOKEN_SEMICOLON:             return "TOKEN_SEMICOLON";
        case TOKEN_ERROR_PAGE:            return "TOKEN_ERROR_PAGE";
        case TOKEN_ALLOW_METHODS:         return "TOKEN_ALLOW_METHODS";
        case TOKEN_CLIENT_MAX_BODY_SIZE:  return "TOKEN_CLIENT_MAX_BODY_SIZE";
        case TOKEN_WORDS:                 return "TOKEN_WORDS";
        case TOKEN_EOF:                 return "TOKEN_EOF";
    }
    return "UNKNOWN_TOKEN";
}

TokenType get_token_type(std::string &token)
{
    switch (keyword_hash(token)) {
        case 0x003580E2u    : return TOKEN_ROOT;
        case 0x05FB28D2u    : return TOKEN_INDEX;
        case 0xBE490DC7u    : return TOKEN_LISTEN;
        case 0x0000007Bu    : return TOKEN_LBRACE;
        case 0x0000007Du    : return TOKEN_RBRACE;
        case 0x0000000Au    : return TOKEN_RETURN;
        case 0x00000023u    : return TOKEN_COMMENT;
        case 0x2AC06F67u    : return TOKEN_CGI_EXT;
        case 0x2D52279Fu    : return TOKEN_CGI_PATH;
        case 0x714F9FB5u    : return TOKEN_LOCATION;
        case 0x0000003Bu    : return TOKEN_SEMICOLON;
        case 0x61844E66u    : return TOKEN_ERROR_PAGE;
        case 0x638F8083u    : return TOKEN_AUTOINDEX;
        case 0x0C3E3CBCu    : return TOKEN_ALLOW_METHODS;
        case 0x74F60DEFu    : return TOKEN_CLIENT_MAX_BODY_SIZE;
        
        default             : return TOKEN_WORDS;
    }
}

std::vector<std::pair<TokenType, std::string> > tokenizer(std::ifstream &config)
{
    std::vector<std::pair<TokenType, std::string> > tokens;

    if(!config.is_open())
        return tokens;


    std::string line;

    while (std::getline(config , line))
    {
        const char *c = line.c_str();
        std::string token; 
        
        while (std::isspace(*c)) {
                    c++;
        }

        while (*c) {
            if(*c == ';' || *c == '\n' || *c == '#' || *c == '{' || *c == '}')
            {
                if(token.size() != 0)
                {
                    tokens.push_back(std::make_pair( get_token_type(token) , token)); 
                    token.clear();
                }
                token += *c;
                tokens.push_back(std::make_pair( get_token_type(token) , token)); 
                token.clear();

                c++;
                continue;
            }
            if(std::isspace(*c))
            {
                tokens.push_back(std::make_pair( get_token_type(token) , token)); 
                token.clear();
                while (std::isspace(*c)) {
                    c++;
                }
                continue;
            }
            token += *c;
            c++;
        }

        if(token.length())
        {
            tokens.push_back(std::make_pair( get_token_type(token) , token)); 
            token.clear();
        }

        tokens.push_back(std::make_pair(TOKEN_RETURN , token));
    }

    tokens.push_back(std::make_pair(TOKEN_EOF, "EOF"));
    return tokens;
}

enum PARSER_STATE { NONE , BLOCK_SERVER , BLOCK_SERVER_ENTER , BLOCK_LOCATION , BLOCK_LOCATION_ENTER};

int main(int argc , char **argv)
{


    if(argc > 2)
    {
        return 0;
    }

    std::ifstream config(argv[1]);

    if(!config.is_open())
    {
        return 1;
    }

    std::vector<std::pair<TokenType, std::string> > tokens = tokenizer(config);
    
    PARSER_STATE state = NONE;
    HttpServer* servers[0xA];
    int         server_count = 0;
    std::memset(servers , 0 , sizeof(servers) );

    int i = 0;

    while(i < tokens.size())
    {
        TokenType type      = tokens[i].first;
        std::string value   = tokens[i].second;

        if(state == NONE)
        {
            if(type != TOKEN_WORDS)
                throw std::runtime_error("Special token should be inside a server");

            state = BLOCK_SERVER_ENTER;
            i++;
            continue;
        }

        if(state == BLOCK_SERVER_ENTER)
        {
            if(type != TOKEN_LBRACE)
                throw std::runtime_error("Block server should be surounded by {*}");


            servers[server_count] = new HttpServer(-1 , "server");

            state = BLOCK_SERVER;
            i++;
            continue;
        }

        if(state == BLOCK_SERVER)
        {
            
        }

        i++;
    }


    return 0;
}
