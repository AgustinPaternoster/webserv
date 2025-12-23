#include "Config.hpp"
#include "HttpUtils.hpp"

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
    _checkduplicatedServer();
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
    std::string tosearch = validDirectives.at(12);
    std::size_t start = _configFile.find(tosearch, pos);
    if (std::string::npos != start)
    {
        if(!_checkRightDirective(start, tosearch.length()))
            throw std::invalid_argument(WRONG_DIRECTIVE);
    }
    while (std::string::npos != start)
    {
        pos = start + validDirectives.at(12).size();
        std::string tmp = _extracDirective(_configFile, pos);
        _parserServerConfig(tmp);
        _useLocatins.clear();
        start = _configFile.find(validDirectives.at(12), pos);
        if(!_checkRightDirective(start, tosearch.length()))
            throw std::invalid_argument(WRONG_DIRECTIVE);
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
        while(!isalpha(server[pos]) && !isdigit(server[pos]) && pos < server.size())
            pos++;
        if (pos >= server.size())
            break;
        end = server.find(32,pos);
        if (std::string::npos == end)
            throw std::invalid_argument(SERVER_CONFIG_ERROR);
        directive = server.substr(pos, end - pos);
        size_t checkDirective = directive.find(';',0);
        if (std::string::npos !=  checkDirective)
            throw std::invalid_argument(SERVER_CONFIG_ERROR);
        pos = end;
        _fillServerStruct(pos,serverTmp,server,_getKeyfromValue(directive));
        pos++;
    }
    _servers.push_back(serverTmp);
    _useDirectives.clear();
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
        _checkDuplicatesDirectives(directive);
        end = server.find(';', pos);
        serverTmp.port =_trimText(server.substr(pos, end - pos));
        _isValidPort(serverTmp.port);
        break;
    case 2:
        _checkDuplicatesDirectives(directive);
        end = server.find(';', pos);    
        serverTmp.root = _trimText(server.substr(pos, end - pos));
        break;
    case 3:
        _checkDuplicatesDirectives(directive);
        end = server.find(';', pos);
        serverTmp.index = _trimText(server.substr(pos, end - pos));
        break;
    case 4:
        _checkDuplicatesDirectives(directive);
        end = server.find(';', pos);
        serverTmp.client_max_body_size = _trimText(server.substr(pos, end - pos));
        break;
    case 5:
        serverTmp.error_page.insert(_extracErrorPage(pos, end, server));
        break;
    case 6:
        tmp = _parseLocationConfig(_extracDirective(server, pos));
        _checkDuplicateLocation(tmp.path);
        serverTmp.locations.push_back(tmp);
        _useDirectivesL.clear();
        end = pos;
        break;
    default:  
        throw std::invalid_argument(WRONG_DIRECTIVE);
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
    locationTmp.actions = STATIC;
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
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.root = _trimText(location.substr(pos, end - pos));
        break;
    case 3:
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.index = _trimText(location.substr(pos, end - pos));
        break;
    case 4:
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.client_max_body_size = _trimText(location.substr(pos, end - pos));
        break;
    case 7:
        end = location.find(';', pos);
        _extracMethods(_trimText(location.substr(pos, end - pos)), locTmp.methods);
        break;
    case 8:
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.autoindex = _trimText(location.substr(pos, end - pos));
        break;
    case 9:
        end = location.find(';', pos);
        locTmp.actions = UPLOAD;
        locTmp.upload_store = _trimText(location.substr(pos, end - pos));
        break;
    case 10:
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.actions = CGI;
        locTmp.cgi_extension =  _ExtracExten(_trimText(location.substr(pos, end - pos)));
        break;
    case 11:
        _checkDuplicatesDirectivesL(directive);
        end = location.find(';', pos);
        locTmp.redirecction = _ExtracExten(_trimText(location.substr(pos, end - pos)));
        break;
    default:
        throw std::invalid_argument(WRONG_DIRECTIVE);
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
    for (size_t i = 0; i < _servers.size(); i++)
    {
        if (_servers[i].port == port)
            return(_servers[i]);
    }
    return (t_server());
}


t_server  Config::locationRouter(std::string port , std::string uri)
{
    t_location best_match_location;
    t_server tmp = _filterServer(port);
    std::string req_location = _getLocation(uri);
    size_t longest_match_len = 0;
    for(std::vector<t_location>::iterator it = tmp.locations.begin(); it != tmp.locations.end(); it++)
    {
        std::string &location_path = (*it).path;
        size_t config_len = location_path.size();
        if(req_location.length() >= config_len)
        {
            std::string locTmp = req_location.substr(0, config_len);
            if(locTmp == location_path)
            {
                if(config_len > longest_match_len)
                {
                    longest_match_len = config_len;
                    best_match_location = *it;
                }
            }
        }
        else if(req_location.length() + 1 == config_len)
        {
            if(location_path[config_len - 1] == '/')
            {
                std::string location_path_no_slash = location_path.substr(0, config_len - 1);
                if(req_location == location_path_no_slash)
                {
                    if (config_len > longest_match_len)
                    {
                        longest_match_len = config_len;
                        best_match_location = *it;
                    }
                }
            }
        }
    }
    tmp.locations.clear();
    if(best_match_location.path.empty() && longest_match_len == 0)
        return(tmp);
    tmp.locations.push_back(best_match_location);      
    return (tmp);
}

t_location Config:: _findSpecificLocation(std::string req_location, t_server &server)
{
    t_location best_match_location;
    size_t longest_match_len = 0;
    for(std::vector<t_location>::iterator it = server.locations.begin(); it != server.locations.end(); it++)
    {
        std::string &location_path = (*it).path;
        size_t config_len = location_path.size();
        if(req_location.length() >= config_len && req_location.substr(0, config_len) == location_path)
        {
            if(config_len > longest_match_len)
            {
                longest_match_len = config_len;
                best_match_location = *it;
            }
        }
    }
    return(best_match_location);
}

std::string Config::_getLocation(std::string uri)
{
    size_t query_start_pos = uri.find('?');
    if (query_start_pos == std::string::npos) 
        return uri;
    return uri.substr(0, query_start_pos);
}

void Config::_checkduplicatedServer(void)
{
    std::string port;
    for(size_t i = 0; i < _servers.size() ; i++)
    {
        port = _servers[i].port;
        for (size_t j = 0; j < i ; j++)
        {
            if(_servers[j].port == port)
                throw std::invalid_argument(SERVER_DUPLICATED_ERROR);
        }
    }
    for (size_t i = 0; i < _servers.size(); ++i) {
        if (_servers[i].port.empty()) {
            throw std::invalid_argument(SERVER_NO_PORT);
        }
    }
}

void Config::_checkDuplicatesDirectives(int key)
{
    
    std::string directive;
    std::map<int, std::string>::const_iterator it = validDirectives.find(key);
    if (it != validDirectives.end()) {
        directive = it->second;
    }
    else
        throw std::invalid_argument(DUPLICATES_DIRECTIVE);
    for (size_t i = 0; i < _useDirectives.size(); ++i) {
        if (_useDirectives[i] == directive) {
            throw std::invalid_argument(DUPLICATES_DIRECTIVE);
        }
    }
    _useDirectives.push_back(directive);
}

void Config::_checkDuplicatesDirectivesL(int key)
{
    
    std::string directive;
    std::map<int, std::string>::const_iterator it = validDirectives.find(key);
    if (it != validDirectives.end()) {
        directive = it->second;
    }
    else
        throw std::invalid_argument(DUPLICATES_DIRECTIVE);
    for (size_t i = 0; i < _useDirectivesL.size(); ++i) {
        if (_useDirectivesL[i] == directive) {
            throw std::invalid_argument(DUPLICATES_DIRECTIVE);
        }
    }
    _useDirectivesL.push_back(directive);
}
// cambio // 
int Config::_checkRightDirective(size_t pos, size_t wordLen)
 {
    if (pos >= _configFile.length()) {
        return true; 
    }
    bool prevOk = (pos == 0 || isspace(_configFile[pos - 1]));
    size_t endPos = pos + wordLen;
    bool nextOk = (endPos == _configFile.length() || 
                   isspace(_configFile[endPos]) || 
                   _configFile[endPos] == '{' || 
                   _configFile[endPos] == ';');

    return (prevOk && nextOk);
 }

// cambio //
 int Config::_checkdirective(std::string directive)
 {
    std::map<int, std::string>::const_iterator it;
    for (it = validDirectives.begin(); it != validDirectives.end(); ++it) {
        if (it->second == directive) {
            return true;
        }
    }
    return false;
 }

 void Config::_checkDuplicateLocation(std::string path)
 {
    
    for (size_t i = 0; i < _useLocatins.size(); ++i) {
        if (_useLocatins[i] == path) {
            throw std::invalid_argument(DUPLICATES_LOCATION);
        }
    }
    _useLocatins.push_back(path);
}

void Config::_isValidPort(const std::string& portStr)
{
    if (portStr.empty()) {
        throw std::invalid_argument(WRONG_PORT);
    }
    for (size_t i = 0; i < portStr.length(); ++i) {
        if (!isdigit(portStr[i])) {
            throw std::invalid_argument(WRONG_PORT);
        }
    }

    long portNum = std::atol(portStr.c_str());
    if (portNum < 0 || portNum > 65535) {
        throw std::invalid_argument(WRONG_PORT);
    }
    if (portStr.length() > 5 && portNum > 65535) {
        throw std::invalid_argument(WRONG_PORT);
    }

}