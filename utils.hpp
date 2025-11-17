#include <string>

enum FILE_TYPE { BINARY , FOLDER , TEXT , HTML , CSS , JS , PNG , JPEG , ICO , MP4 , MPEG , ERR_DENIED , ERR_NOTFOUND }; 

std::string hex(size_t value);
std::string getRequestPath(const std::string &header);
FILE_TYPE   mime(const std::string&);
std::string header_builder(int code , FILE_TYPE t);
