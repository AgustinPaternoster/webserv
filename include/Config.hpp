#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_
#include "Common.hpp"
#include "CgiTasks.hpp"


class Config
{
    public:
        Config(void);
        Config(const char *path);
        Config(const Config& other);
        ~Config(void);
        
        CgiTask &CgiJobs;
        Config& operator=(const Config& other);
        static const std::map<int,std::string> validDirectives;
        std::vector<t_server> getServers(void)const;
        void printPorts(void);
        t_server locationRouter(std::string port, std::string url);
        
    private:
        std::vector<t_server>   _servers;
        std::string             _configFile;
        
        void _openFile(const char* path);
        void _parseFile(void);
        void _parserServerConfig(std::string server);
        std::string _extracDirective(std::string& src , size_t &pos);
        std::string _trimText(std::string src);
        void _fillServerStruct(size_t& pos, t_server& serverTmp, std::string server, int directive);
        void _fillLocationStruct(size_t& pos, t_location& locTmp, std::string location, int directive);
        int _getKeyfromValue(std::string directive);
        std::pair<std::string,std::string> _extracErrorPage(size_t& pos, size_t& end, std::string server);
        t_location _parseLocationConfig(std::string location);
        void _extracMethods(std::string src, std::vector<int>& methods);
        std::pair<std::string, std::string> _ExtracExten(std::string src);
        t_server _filterServer(std::string port);
        std::string _getLocation(std::string uri);
        t_location _findSpecificLocation(std::string location,  t_server &server);
    };

#endif
