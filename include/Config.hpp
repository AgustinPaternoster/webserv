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

    private:
        std::vector<t_server>   _servers;
        std::string             _configFile;
        void _openFile(char* path);
        void _parseFile(std::string file);
        void _parserServerConfig(std::string server);
        static const std::map<int,std::string> validDirectives;
};

#endif