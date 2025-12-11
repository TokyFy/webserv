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
#include <exception>
#include <fstream>
#include <locale>
#include <stdexcept>
#include <string>
#include <sys/types.h>
#include <utility>
#include <vector> 
#include <iostream>
#include <sstream>
#include <climits>
#include <cstdlib>
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

typedef std::pair<TokenType, std::string> t_token;

int safe_atoi(const std::string &s) {
    char *end;
    long value = std::strtol(s.c_str(), &end, 10);

    // If end doesn't point to the end of the string → invalid characters
    if (*end != '\0')
        throw std::runtime_error("Invalid number");

    // Check overflow for int
    if (value < INT_MIN || value > INT_MAX)
        throw std::runtime_error("Number out of range");

    if(value < 0)
        throw std::runtime_error("Nagative number");

    return static_cast<int>(value);
}


std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter))
        result.push_back(item);

    return result;
}

// Parsing
void parse_listen(HttpServer *server , std::vector<t_token> &tokens)
{
    if(tokens[0].first != TOKEN_LISTEN)
        throw std::runtime_error("Listen token fatal error");

    tokens.erase(tokens.begin());

    if(tokens[0].first != TOKEN_WORDS)
        throw std::runtime_error("Listen should have a value");

    std::vector<std::string> splited = split(tokens[0].second , ':');

    if(splited.size() != 2)
        throw std::runtime_error("Listed syntax : eg : 0.0.0.0:8080");

    server->setInterface(splited[0]);
    
    try 
    {
        server->setPort(safe_atoi(splited[1]));
    }
    catch(std::exception &e)
    {
        throw std::runtime_error("Port : " + std::string(e.what()));
    }

    tokens.erase(tokens.begin());
    
    if(tokens[0].first != TOKEN_SEMICOLON)
        throw std::runtime_error("Missing ; in the end of Listed");

    tokens.erase(tokens.begin());

    std::cerr << "Interface : " << server->getInterface() << std::endl;
    std::cerr << "Port : " << server->getPort() << std::endl;
}

long parse_client_max_body_size(const std::string &s) {
    if (s.empty())
        throw std::runtime_error("Empty size string");

    char *end;
    long value = std::strtol(s.c_str(), &end, 10);

    if (end == s.c_str())
        throw std::runtime_error("Invalid size: no digits");

    long multiplier = 1;

    if (*end != '\0') {
        if (*end == 'M') {
            multiplier = 1024L * 1024L; // 1 MB
        } else {
            throw std::runtime_error("Invalid suffix: only 'M' allowed");
        }
        if (*(end + 1) != '\0')
            throw std::runtime_error("Invalid characters after suffix");
    }
    if (value < 0 || value > LONG_MAX / multiplier)
        throw std::runtime_error("Size too large");

    return value * multiplier;
}

void parse_client_max_body_size(HttpServer *server , std::vector<t_token> &tokens)
{
    tokens.erase(tokens.begin());

    try 
    {
        ssize_t max_body = parse_client_max_body_size(tokens[0].second);

        if(max_body < 0)
            throw std::runtime_error("negative max_body size");

        server->setClientMaxBodySize(max_body);
    } 
    catch (std::exception &e) {
        throw std::runtime_error("client_max_body_size : " + std::string(e.what()));
    }

    tokens.erase(tokens.begin());

    if(tokens[0].first != TOKEN_SEMICOLON)
        throw std::runtime_error("client_max_body_size should end with ;");

    tokens.erase(tokens.begin());

    std::cout << "client_max_body_size : " << server->getClientMaxBodySize() << std::endl;
}

void parse_error_page(HttpServer *server , std::vector<t_token> &tokens)
{
    tokens.erase(tokens.begin());
    int error = -1; 
    try 
    {
        error = safe_atoi(tokens[0].second);
    }
    catch (std::exception &e)
    {
        throw std::runtime_error("Error page : " + std::string(e.what()));
    }

    tokens.erase(tokens.begin());

    if(tokens[0].first != TOKEN_WORDS)
        throw std::runtime_error("Missing error path");

    server->setErrorPage(error, tokens[0].second);

    tokens.erase(tokens.begin());

    if(tokens[0].first != TOKEN_SEMICOLON)
        throw std::runtime_error("Missing ; after error page");
    
    tokens.erase(tokens.begin());

    std::cout << "Error_page " << error << " " << server->getErrorPage(error) << std::endl;
    return;
}

void parse_location(HttpServer *server , std::vector<t_token> &tokens)
{
    int location_count = -1;

    tokens.erase(tokens.begin());

    if(tokens[0].first != TOKEN_WORDS)
        throw std::runtime_error("Missing source for location");

    // PARSE LOCATION

    return ; 
}

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

    std::vector< t_token > tokens = tokenizer(config);
    
    PARSER_STATE state = NONE;
    HttpServer* servers[0xA];

    int server_cout = 0;
    std::memset(servers , 0 , sizeof(servers) );

    while(!tokens.empty())
    {
        TokenType type      = tokens[0].first;
        std::string value   = tokens[0].second;

        // std::cout << token_type_to_string(type) << " " << value << std::endl;

        if(type == TOKEN_RETURN)
        {
            tokens.erase(tokens.begin());
            continue;
        }
        
        if(type == TOKEN_COMMENT)
        {
            while(tokens[0].first != TOKEN_RETURN)
            {
                if(tokens[0].first == TOKEN_EOF)
                    break;
                tokens.erase(tokens.begin());
            }

            continue;
        }

        if(state == NONE)
        {
            if(type != TOKEN_WORDS)
                throw std::runtime_error("Special token should be inside a server");

            server_cout++;

            servers[server_cout - 1] = new HttpServer(-1 , value);
            state = BLOCK_SERVER_ENTER;

            tokens.erase(tokens.begin());
            continue;
        }

        if(state == BLOCK_SERVER_ENTER)
        {
            if(type != TOKEN_LBRACE)
                throw std::runtime_error("Block server should be surounded by {*}");
            
            tokens.erase(tokens.begin());
            state = BLOCK_SERVER;
            continue;
        }

        if(state == BLOCK_SERVER)
        {
            if(type == TOKEN_EOF)
                throw std::runtime_error("Missing } on the server");

            if(type == TOKEN_WORDS)
                throw std::runtime_error("Unkown configuration : " + value);

            if(type == TOKEN_LISTEN)
                parse_listen(servers[server_cout - 1], tokens);

            if(type == TOKEN_CLIENT_MAX_BODY_SIZE)
                parse_client_max_body_size(servers[server_cout - 1] , tokens);

            if(type == TOKEN_ERROR_PAGE)
                parse_error_page(servers[server_cout - 1] , tokens);

            if(type == TOKEN_LOCATION)
                parse_location(servers[server_cout - 1] , tokens); 
                
            if(type == TOKEN_RBRACE)
                state = NONE;
        }
        tokens.erase(tokens.begin());
    }


    return 0;
}
