#ifndef __CGI_H__
#define __CGI_H__
#include "Common.hpp"
#include "HTTPRequest.hpp"
#include "HttpUtils.hpp"
#include "CgiTasks.hpp"


class Cgi
{
    public:
        Cgi(HttpRequest &request, std::vector<struct pollfd> &poll_fds, int poll_id, t_server config);
        ~Cgi(void);

        void  CgiHandler(CgiTask &cgijobs);
        
        private:
        std::map<std::string, std::string> _envVar;
        HttpRequest& _request;
        t_server _config;
        std::vector<struct pollfd>& _poll_fds;
        int _poll_id;

        char** _getEnvVar(void);
        void _parseHeaderToCGIEnv(std::map<std::string, std::string> &headers);
        std::string _normalizeHeadersName(std::string& name, bool ishppt);
        void _parseRequestToEnv(void);
        void _extracScriptName(void);
};
#endif