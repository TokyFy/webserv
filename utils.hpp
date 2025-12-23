# pragma once

#include "HttpServer.hpp"
#include <string>
#include <sys/types.h>

enum FILE_TYPE { BINARY , FOLDER , TEXT , HTML , CSS , JS , PNG , JPEG , ICO , MP4 , MPEG , ERR_DENIED , ERR_NOTFOUND }; 

std::string replaceFirstOccurrence(const std::string &originalString, const std::string &target, const std::string &replacement);
std::string     hex(size_t value);
std::string     getRequestPath(const std::string &header);
std::string     getRequestMethod(const std::string &header);
FILE_TYPE       mime(const std::string&);
std::string     header_builder(int code , FILE_TYPE t);
std::string     to_chuncked(const char* buff , ssize_t len);
int             error_page_builder(int code);
int             indexof(Location& location , std::string path);
