#include "Common.hpp"
#include "HTTPRequest.hpp"

class Cgi
{
    public:
        Cgi(HttpRequest &par, std::vector<struct pollfd> &poll_fds);
        ~Cgi(void);

        void  CgiHandler(void);
        std::map<std::string, std::string> setupCgiEnvironment(void);
    private:
        std::map<std::string, std::string> _envVar;
        HttpRequest& _request;
        char** _getEnvVar(void);        
};