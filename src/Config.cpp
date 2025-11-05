#include "Config.hpp"

std::map<int, std::string> createValidDirectives(void)
{
    std::map<int, std::string> m;
    m[1]  = "listen";
    m[2]  = "root";
    m[3]  = "index";
    m[4]  = "client_max_body_size";
    m[5]  = "error_page";
    m[6]  = "location";
    m[7]  = "allow_methods";
    m[8]  = "autoindex";
    m[9]  = "upload_store";
    m[10]  = "cgi_extension";
    m[11] = "cgi_path";
    m[12] = "return";
    m[13] = "server";
    return (m);
}


const std::map<int,std::string> Config::validDirectives = createValidDirectives();


Config::Config(void)
{
    _openFile(DEFAULTCONFIG);
    _parseFile();
}

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

void Config::_openFile(const char* path)
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
    size_t pos = 0;
    std::size_t start = _configFile.find(validDirectives.at(13), pos);
    if (std::string::npos == start)
        throw std::invalid_argument(CONFIG_NO_SERVER_ERROR);
    while (std::string::npos != start)
    {
        pos = start + 6;
        _parserServerConfig(_extracDirective(_configFile, pos, start + validDirectives.at(12).size() ));
        start = _configFile.find(validDirectives.at(13), pos);
    }
}

void Config::_parserServerConfig(std::string server)
{
    std::string directive;
    size_t pos = 0;
    size_t end;
    t_server serverTmp;
    if (server.empty())
        throw std::invalid_argument(SERVER_CONFIG_ERROR);
    while(pos < server.size())
    {
        while(!isalpha(server[pos]) && pos < server.size())
            pos++;
        if (pos >= server.size())
            break;
        end = server.find(32,pos);
        directive = server.substr(pos, end - pos);
        pos = end;
        _fillServerStruct(pos,serverTmp,server,_getKeyfromValue(directive));

        // if (directive == validDirectives.at(5))  
        // {
        //     _extracDirective(server, pos, pos + validDirectives.at(5).size());
        // }
        pos++;
    }
    _servers.push_back(serverTmp);
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
                int len = pos - (start);
                return(src.substr(start, len));
            }
        }
    }
    return ("");
}

std::string Config::_trimText(std::string src)
{
    while(src[0] == 32)
        src.erase(src.begin());
    while(src[src.size() - 1] == 32)
        src.erase(src.size() - 1);
    return (src);
}

std::vector<t_server> Config::getServers(void)const
{
    return(_servers);
}

void Config::printPorts(void)
{
    for(size_t i = 0; i < _servers.size(); i++)
            std::cout << _servers[i].port << std::endl;
}

 void Config::_fillServerStruct(size_t& pos, t_server& serverTmp, std::string server, int directive)
 {
    size_t end;

    switch (directive)
    {
    case 1:
        end = server.find(';', pos);
        serverTmp.port =_trimText(server.substr(pos, end - pos));
        break;
    case 2:
        end = server.find(';', pos);    
        serverTmp.root = _trimText(server.substr(pos, end - pos));
        break;
    case 3:
        end = server.find(';', pos);
        serverTmp.index = _trimText(server.substr(pos, end - pos));
        break;
    case 4:
        end = server.find(';', pos);
        serverTmp.client_max_body_size = _trimText(server.substr(pos, end - pos));
        break;
    case 5:
        serverTmp.error_page.insert(_extracErrorPage(pos, end, server));
        break;
    case 6:
        _extracDirective(server, pos, end);
        end = pos;
        break;
    case 7:
        //allow methods
        break;
    case 8:
        /* code */
        break;
    case 9:
        /* code */
        break;
    case 10:
        /* code */
        break;
    case 11:
        /* code */
        break;
    case 12:
        /* code */
        break;
    case 13:
        /* code */
        break;
    default:
        break;
    }
    pos = end;
 }

 int Config::_getKeyfromValue(std::string directive)
 {
    for (std::map<int, std::string>::const_iterator it = validDirectives.begin(); it != validDirectives.end(); it++ )
    {
        if((*it).second == directive)
            return ((*it).first);
    }
    return(0);
 }

 std::pair<std::string,std::string> Config::_extracErrorPage(size_t& pos, size_t& end, std::string server)
 {
    std::pair<std::string, std::string> errorPage;
    while(server[pos] == 32)
        pos++;
    end  = server.find(32,pos);
    errorPage.first = _trimText(server.substr(pos, end - pos));
    pos = end;
    end = server.find(';', pos);
    errorPage.second = _trimText(server.substr(pos, end - pos));
    return (errorPage);
 }