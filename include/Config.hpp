#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include "Common.hpp"

class Config
{
    public:
        Config(void);
        Config(char *path);
        Config(const Config& other);
        ~Config(void);

        Config& operator=(const Config& other);
        static const std::map<int,std::string> validDirectives;
        std::vector<t_server> getServers(void)const;
        void printPorts(void);
        
    private:
        std::vector<t_server>   _servers;
        std::string             _configFile;
        void _openFile(const char* path);
        void _parseFile(void);
        void _parserServerConfig(std::string server);
        std::string _extracDirective(std::string& src , size_t &pos, size_t start);
        std::string _trimText(std::string src);
        
};

#endif
