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
#include <map>

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

    if( access(path, F_OK) == -1)
        return ERR_NOTFOUND;

    if(access(path, R_OK) == -1)
        return ERR_DENIED;

    struct stat info;
    stat(path , &info);

    if(S_ISDIR(info.st_mode))
        return FOLDER;

    static const std::map<std::string, FILE_TYPE > mime = {
        {".html"    , HTML  },
        {".htm"     , HTML  },
        {".txt"     , TEXT  },
        {".css"     , CSS   },
        {".js"      , JS    },
        {".png"     , PNG   },
        {".jpg"     , JPEG  },
        {".jpeg"    , JPEG  },
        {".ico"     , ICO   },
    };

    size_t pos = str.find_last_of('.');
    if (pos == std::string::npos)
        return BINARY;

    std::string ext = str.substr(pos);
    std::map<std::string , FILE_TYPE>::const_iterator it = mime.find(ext);

    return (it != mime.end()) ? it->second : BINARY;
}
