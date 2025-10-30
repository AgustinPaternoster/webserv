#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include "Common.hpp"

class Config
{
    public:
        Config(char *path);
        Config(const Config& other);
        ~Config(void);

        Config& operator=(const Config& other);
        static const std::map<int,std::string> validDirectives;

    private:
        std::vector<t_server>   _servers;
        std::string             _configFile;
        void _openFile(char* path);
        void _parseFile(void);
        void _parserServerConfig(std::string server);
        std::string _extracDirective(std::string& src , size_t &pos, size_t start);

};

#endif