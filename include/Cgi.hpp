#include "Common.hpp"
#include "HTTPRequest.hpp"

class Cgi
{
    public:
        Cgi(HttpRequest &par, std::vector<struct pollfd> &poll_fds);
        ~Cgi(void);

        void  CgiHandler(void);
        
        private:
        std::map<std::string, std::string> _envVar;
        HttpRequest& _request;
        char** _getEnvVar(void);
        void _parseHeaderToCGIEnv(std::map<std::string, std::string> &headers);
        std::string _normalizeHeadersName(std::string& name, bool hppt);


};