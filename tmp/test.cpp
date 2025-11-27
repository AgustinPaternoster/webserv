#include <iostream>
#include "../include/Common.hpp"

std::map<std::string, std::string> setupCgiEnvironment(void)
{
    std::map<std::string, std::string> _envVar;
    
    _envVar["REQUEST_METHOD"] = "GET";
    _envVar["SERVER_PROTOCOL"] = "HTTP/1.1";
    _envVar["QUERY_STRING"] = "";
    _envVar["SERVER_PORT"] = "8080";
    _envVar["GATEWAY_INTERFACE"] = "CGI/1.1";
    _envVar["CONTENT_LENGTH"] = "150";
    _envVar["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    return(_envVar);
}

int main(void)
{
    std::map<std::string, std::string> _envVar = setupCgiEnvironment();
    char** envVar = nullptr;
    int i = 0;
    try
    {
        char** envVar = new char*[_envVar.size() + 1];
        for(std::map<std::string, std::string>::iterator  it = _envVar.begin(); it  != _envVar.end(); it++)
        {
            std::string tmp = it->first + "=" + it->second;
            envVar[i] = new char[tmp.size() + 1];
            strcpy(envVar[i], tmp.c_str());
            i++;
        }
        envVar[_envVar.size()] = nullptr;
        i = 0;
        while(envVar[i] != nullptr)
        {
            std::cout << envVar[i] << std::endl;
            i++;
        }
    }
    catch(const std::exception& e)
    {
        //gestionar error new
        std::cerr << e.what() << '\n';
    }

      
}