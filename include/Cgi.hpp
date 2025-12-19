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

        int  CgiHandler(CgiTask &cgijobs);
        int  parseRequestToEnv(void);
        
    private:
        std::map<std::string, std::string> _envVar;
        HttpRequest& _request;
        t_server _config;
        std::vector<struct pollfd>& _poll_fds;
        int _poll_id;

        char** _getEnvVar(void);
        void _parseHeaderToCGIEnv(std::map<std::string, std::string> &headers);
        std::string _normalizeHeadersName(std::string& name, bool ishppt);
        int _extracScriptName(void);
        void _closeAllFd(void);
        void _executeCgi(void);
        const char* _getScriptPath(void);
        const char* _getDirectoryPath(void);
        void _freeCGIResources(char** envp, const char* script_path, const char* directory_path);
        const char* _getScriptFileName(void);
        int _closeAndClean(struct pollfd & poll);
        int _allowMethod(void);
        methods _convertStringToEnum(const std::string& method);
        int _checkFilepath(void);
        bool _checkValidExtension(std::string path);


};

#endif