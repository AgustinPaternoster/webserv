#include "Config.hpp"

const std::map<int,std::string> validDirecvalidDirectives =
{
    {0, "listen"},
    {1, "root"},
    {2, "index"},
    {3, "client_max_body_size"},
    {4, "error_page"},
    {5, "location"},
    {6, "allow_methods"},
    {7, "autoindex"},
    {8, "upload_store"},
    {9, "cgi_extension"},
    {10, "cgi_path"},
    {11, "return"}
};

Config::Config(char *path)
{
    _openFile(path);


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

void Config::_openFile(char* path)
{
    std::ifstream file(path);
    if(!file.is_open())
        throw std::invalid_argument(ERROR_FILE_NOT_OPEN);
    std::istreambuf_iterator<char> init(file);
    std::istreambuf_iterator<char> end;
    _configFile = std::string(init,end);
}

void Config::_parseFile(std::string path)
{
    int level;
    int i = 0;
    int pos = 0;
    std::size_t start;

    while (i < _configFile.size())
    {
        start = _configFile.find("server", i);
        if (std::string::npos == start)
            throw std::invalid_argument(CONFIG_NO_SERVER_ERROR);
        pos = start + 6;
        for (;pos < _configFile.size(); pos++)
        {
            if(_configFile[pos] == '{')
                level++;
            if(_configFile[pos] == '}')
            {
                level--;
                if(level == 0)
                {
                    /// funcion parser server;
                    break;
                }
            }
        }
        i = pos;
    }
  
}

void Config::_parserServerConfig(std::string server)
{
    
}
