#include "Config.hpp"

Config::Config(char *path)
{
    std::ifstream file(path);
    if(!file.is_open())
        throw std::invalid_argument(ERROR_FILE_NOT_OPEN);
    std::istreambuf_iterator<char> init(file);
    std::istreambuf_iterator<char> end;
    _configFile = std::string(init,end);
    std::cout << _configFile;

}

Config::Config(const Config& other)
{
    (*this) = other;
}

Config::~Config(void){}

Config& Config::operator=(const Config& other)
{
    if (this != &other)
    {
        _servers = other._servers;
        _configFile = other._configFile;
    }
    return(*this);
}

void Config::_parseFile(std::string file)
{
    std::string serverTmp;
    while(c)
}
