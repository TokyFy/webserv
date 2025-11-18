/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: franaivo <franaivo@student.42antananarivo  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/08 21:40:52 by franaivo          #+#    #+#             */
/*   Updated: 2025/11/09 00:18:31 by franaivo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <sstream>
#include <string>
#include <unistd.h>
#include "utils.hpp"
#include <sys/types.h>
#include <sys/stat.h>

std::string hex(size_t value) 
{
    std::ostringstream oss;
    oss << std::hex << std::nouppercase << value;
    return oss.str();
}

std::string getRequestPath(const std::string &header)
{
    std::string::size_type methodEnd = header.find(' ');
    if (methodEnd == std::string::npos)
        return "";

    std::string::size_type pathEnd = header.find(' ', methodEnd + 1);
    if (pathEnd == std::string::npos)
        return "";

    return header.substr(methodEnd + 1, pathEnd - methodEnd - 1);
}


FILE_TYPE mime(const std::string& str)
{
    const char *path = str.c_str();

    // Check existence
    if (access(path, F_OK) == -1)
        return ERR_NOTFOUND;

    // Check readability
    if (access(path, R_OK) == -1)
        return ERR_DENIED;

    struct stat info;
    if (stat(path, &info) == -1)
        return BINARY; // fallback

    // Directory
    if (S_ISDIR(info.st_mode))
        return FOLDER;

    // Get file extension
    size_t pos = str.find_last_of('.');
    if (pos == std::string::npos)
        return BINARY;

    std::string ext = str.substr(pos);

    // Map extensions to FILE_TYPE
    if (ext == ".html" || ext == ".htm") return HTML;
    if (ext == ".txt")                   return TEXT;
    if (ext == ".css")                   return CSS;
    if (ext == ".js")                    return JS;
    if (ext == ".png")                   return PNG;
    if (ext == ".jpg" || ext == ".jpeg") return JPEG;
    if (ext == ".ico")                   return ICO;
    if (ext == ".mp4")                   return MP4;
    if (ext == ".mpeg" || ext == ".mpg") return MPEG;

    // Default
    return BINARY;
}


std::string fileTypeToStr(FILE_TYPE t)
{
    switch (t)
    {
        case BINARY:       return "application/octet-stream";
        case FOLDER:       return "text/html";
        case TEXT:         return "text/plain";
        case HTML:         return "text/html";
        case CSS:          return "text/css";
        case JS:           return "application/javascript";
        case PNG:          return "image/png";
        case JPEG:         return "image/jpeg";
        case ICO:          return "image/x-icon";
        case MP4:          return "video/mp4";
        case MPEG:         return "video/mpeg";

        // Errors (you can choose any MIME type; HTML is typical)
        case ERR_DENIED:   return "text/html";
        case ERR_NOTFOUND: return "text/html";
    }

    return "application/octet-stream";
}


std::string itos(int n)
{
    std::stringstream ss;
    ss << n;
    std::string s = ss.str();

    return s;
}

std::string header_builder(int code , FILE_TYPE t)
{

    std::string scode = itos(code); 
    std::string stype = fileTypeToStr(t);

    return "HTTP/1.1 " + scode + " whatever\r\n"
            "Content-Type: " + stype + "\r\n"
            "Transfer-Encoding: chunked\r\n"
            "Connection: close\r\n\r\n";
}

std::string to_chuncked(const char* buff , ssize_t len)
{
    std::string chunk;
    
    chunk += hex(len) + "\r\n";
    chunk += std::string(buff , len) + "\r\n";

    return chunk;
}
