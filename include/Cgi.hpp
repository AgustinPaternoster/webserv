#include "Common.hpp"
#include "HTTPRequest.hpp"

class Cgi
{
    public:
        Cgi(HttpRequest &request, std::vector<struct pollfd> &poll_fds);
        ~Cgi(void);

        void  CgiHandler(void);
        
        private:
        std::map<std::string, std::string> _envVar;
        HttpRequest& _request;
        t_server _config;

        char** _getEnvVar(void);
        void _parseHeaderToCGIEnv(std::map<std::string, std::string> &headers);
        std::string _normalizeHeadersName(std::string& name, bool hppt);
        void _parseRequestToEnv(HttpRequest &request);
        void _extracScriptName(void);


};