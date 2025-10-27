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
        std::vector<t_server> _servers;
        std::string _configFile;
  
};

#endif