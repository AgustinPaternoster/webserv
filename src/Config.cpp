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
    m[11] = "return";
    m[12] = "server";
    return (m);
}


const std::map<int,std::string> Config::validDirectives = createValidDirectives();


Config::Config(void){}

Config::Config(const char *path)
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
    std::size_t start = _configFile.find(validDirectives.at(12), pos);
    if (std::string::npos == start)
        throw std::invalid_argument(CONFIG_NO_SERVER_ERROR);
    while (std::string::npos != start)
    {
        pos = start + validDirectives.at(12).size();
        _parserServerConfig(_extracDirective(_configFile, pos));
        start = _configFile.find(validDirectives.at(12), pos);
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
        pos++;
    }
    _servers.push_back(serverTmp);
}

std::string Config::_extracDirective(std::string& src, size_t &pos)
{
    int level = 0;
    size_t start = pos;
    
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
    size_t end = 0;
    t_location tmp;

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
        tmp = _parseLocationConfig(_extracDirective(server, pos));
        serverTmp.locations.push_back(tmp);
        end = pos;
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

 t_location Config::_parseLocationConfig(std::string location)
 {
    std::string directive;
    size_t pos = 0;
    size_t end;
    t_location locationTmp;
    while(location[pos] == 32)
        pos++;
    end = location.find("{");
    locationTmp.path = _trimText(location.substr(pos, end - pos));
    pos = end;
    while(pos < location.size())
    {
        while(!isalpha(location[pos]) && pos < location.size())
            pos++;
        if (pos >= location.size())
                break;
        end = location.find(32, pos);
        directive = location.substr(pos, end - pos);
        pos = end;
        _fillLocationStruct(pos, locationTmp, location, _getKeyfromValue(directive));
    }
    return (locationTmp);
 }

void Config::_fillLocationStruct(size_t& pos, t_location& locTmp, std::string location, int directive)
{
    size_t end = 0;
    switch (directive)
    {
    case 2:
        end = location.find(';', pos);
        locTmp.root = _trimText(location.substr(pos, end - pos));
        break;
    case 7:
        end = location.find(';', pos);
        _extracMethods(_trimText(location.substr(pos, end - pos)), locTmp.methods);
        break;
    case 8:
        end = location.find(';', pos);
        locTmp.autoindex = _trimText(location.substr(pos, end - pos));
        break;
    case 9:
        end = location.find(';', pos);
        locTmp.upload_store = _trimText(location.substr(pos, end - pos));
        break;
    case 10:
        end = end = location.find(';', pos);
        locTmp.cgi_extension =  _ExtracExten(_trimText(location.substr(pos, end - pos)));
    default:
        break;
    }
    pos = end;    

} 

void Config::_extracMethods(std::string src, std::vector<int>& a_methods)
{
    std::vector<int> tmp;
    std::string method;
    std::map<std::string, methods> m;
    m["GET"] = GET;
    m["POST"] = POST;
    m["DELETE"] = DELETE;
    m["PUT"] = PUT;
    m["PATCH"] = PATCH;

    for (size_t i = 0; i < src.size(); i++)
    {
        if(isalpha(src[i]))
            method += src[i];
        if(src[i] == 32 && !method.empty())
        {
            for(std::map<std::string, methods>::iterator it = m.begin(); it != m.end(); it++)
            {
                if((*it).first == method)
                {   a_methods.push_back((*it).second);
                    method.clear();
                }   
            }
        }
    }
    if(!method.empty())
    {
        for(std::map<std::string, methods>::iterator it = m.begin(); it != m.end(); it++)
        {
            if((*it).first == method)
               a_methods.push_back((*it).second);
        }
    }
}

std::pair<std::string, std::string> Config::_ExtracExten(std::string src)
{

    std::pair<std::string , std::string> tmp;
    tmp.first = _trimText(src.substr(0, src.find(32)));
    tmp.second = _trimText(src.substr(src.find(32)));
    return (tmp);
}

t_server Config::_filterServer(std::string port)
{
    for (int i = 0; i < _servers.size(); i++)
    {
        if (_servers[i].port == port)
            return(_servers[i]);
    }
    return (t_server());
}

static std::string trim_path(const std::string& str) 
{
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) 
    {
        return str;
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

t_server  Config::locationRouter(std::string port , std::string uri)
{
    int     best_index = -1;
    size_t  best_len = 0;
    t_server tmp = _filterServer(port);
    std::string location;
    
    for (size_t i = 0; i < tmp.locations.size(); i++)
    {
        std::string path = trim_path(tmp.locations[i].path);
        if (uri.find(path) == 0)
        {
            if (path.size() > best_len)
            {
                best_len = path.size();
                best_index = i;
            }
        }
    }
    t_server result = tmp;
    result.locations.clear();
    if (best_index != -1)
        result.locations.push_back(tmp.locations[best_index]);
    return (result);
}