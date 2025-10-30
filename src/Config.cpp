#include "Config.hpp"

std::map<int, std::string> createValidDirectives(void)
{
    std::map<int, std::string> m;
    m[0]  = "listen";
    m[1]  = "root";
    m[2]  = "index";
    m[3]  = "client_max_body_size";
    m[4]  = "error_page";
    m[5]  = "location";
    m[6]  = "allow_methods";
    m[7]  = "autoindex";
    m[8]  = "upload_store";
    m[9]  = "cgi_extension";
    m[10] = "cgi_path";
    m[11] = "return";
    return (m);
}


const std::map<int,std::string> Config::validDirectives = createValidDirectives();


Config::Config(char *path)
{
    _openFile(path);
    _parseFile();

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

void Config::_parseFile(void)
{
    int level;
    size_t i = 0;
    size_t pos = 0;
    std::size_t start = _configFile.find("server", i);
    if (std::string::npos == start)
        throw std::invalid_argument(CONFIG_NO_SERVER_ERROR);
;
    while (std::string::npos != start)
    {
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
                    _parserServerConfig(_configFile.substr(start + 6, pos - (start + 6)));
                    break;
                }
            }
        }
        start = _configFile.find("server", pos);
    }
}

void Config::_parserServerConfig(std::string server)
{
    (void)server;
    // std::vector<std::pair<std::string,std::string>> directives;
    // std::string tmp;
    // std::string directive;
    // size_t i = 0;
    // size_t pos;
    // while(i < server.size())
    // {
    //     while(!isalpha(server[i]))
    //         i++;
    //     directive = server.substr(i, server.find(32,i) - i);
    //     if (validDirectives.at(5) == directive)
    //     {   
    //         std::cout << directive;
    //     else
    //         std::cout << directive;
    //     i++;
    // }
}

std::string Config::_extracDirective(std::string& src, size_t &pos, size_t start)
{
    int level = 0;
    
    for (; pos < src.size(); pos++)
    {
        if(src[pos] == '{')
            level++;
        if(src[pos] == '}')
        {
            level--;
            if(level == 0)
            {
                int len = pos - (start + 6);
                return(src.substr(start + 6, len));
            }
        }
    }
    return ("");
}