#include "Cgi.hpp"

Cgi::Cgi(HttpRequest &par, std::vector<struct pollfd> &poll_fds): _request(par)
{
    (void)par;
    
    (void)poll_fds;
}

Cgi::~Cgi(void){}

std::string _normalizeHeadersName(const std::string& name, bool ishttp)
{
    std::string NormalizedName = name;
    for (std::string::iterator it = NormalizedName.begin(); it != NormalizedName.end(); it++)
    {
        *it = toupper(*it);
        if(*it == '-')
             *it = '_';
    }
    if (ishttp)
        return ("HTTP_" + NormalizedName);
    return (NormalizedName);


}

void Cgi::_parseHeaderToCGIEnv(std::map<std::string, std::string> &headers)
{
   for(std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++)
   {
        std::string name = it->first;
        std::string value = it->second;

        if (name == "Content-Type" || name == "Content-Length")
        {
            std::string CgiName = _normalizeHeadersName(name, false);
        }
        else
        {
            std::string cgiName = _normalizeHeadersName(name, true);
            _envVar[cgiName] = value;
        }
   }
}


char** Cgi::_getEnvVar(void)
{
    char** envVar = nullptr;
    int i = 0;
    try
    {
        char** envVar = new char*[_envVar.size() + 1];
        for(std::map<std::string, std::string>::iterator  it = _envVar.begin(); it  != _envVar.end(); it++)
        {
            std::string tmp = it->first + "=" + it->second;
            envVar[i] = new char[tmp.size() + 1];
            strcpy(envVar[i], tmp.c_str());
            i++;
        }
        envVar[_envVar.size()] = nullptr;
    }
    catch(const std::exception& e)
    {
        //gestionar error new // sistem error
        std::cerr << e.what() << '\n';
    }
    return(envVar);
}

void Cgi::CgiHandler(void)
{
    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;

    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
        //error pipe
    pid = fork();
    if( pid < 0)
        //error;
    if(pid != 0)
    {
        if(_envVar["REQUEST_METHOD"] == "GET")
        {
            close(pipe_in[0]);
            close(pipe_out[1]);
            close(pipe_in[1]);
        }
        if(_envVar["REQUEST_METHOD"] == "POST")
        {
                close(pipe_in[0]);
                // getionar para multiplexing
                write(pipe_in[1], (_request.getBody()).c_str(), (_request.getBody()).size());
                close(pipe_in[1]);
                close(pipe_out[1]);
        }
    }
    if(pid == 0)
    {
        if(dup2(pipe_in[0], STDIN_FILENO) < 0)
            //error
        if(dup2(pipe_out[1], STDOUT_FILENO))
            //error
        close(pipe_out[1]);
        close(pipe_in[0]);
        
    }
};
