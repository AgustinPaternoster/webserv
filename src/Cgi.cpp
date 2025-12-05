#include "Cgi.hpp"

Cgi::Cgi(HttpRequest &par, std::vector<struct pollfd> &poll_fds): _request(par)
{
    (void)par;
    (void)poll_fds;
}

Cgi::~Cgi(void){}

std::map<std::string, std::string> Cgi::setupCgiEnvironment(void)
{
    _envVar["REQUEST_METHOD"] = "GET";
    _envVar["SERVER_PROTOCOL"] = "HTTP/1.1";
    _envVar["QUERY_STRING"] = "";
    _envVar["SERVER_PORT"] = "8080";
    _envVar["GATEWAY_INTERFACE"] = "CGI/1.1";
    _envVar["CONTENT_LENGTH"] = "150";
    _envVar["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
    _envVar["SCRIPT_NAME"]="-";
    _envVar["REMOTE_ADDR"]="-";
}


char** Cgi::_getEnvVar(void)
{
    char** envVar = nullptr;
    int i = 0;
    try
    {
        char** envVar = new char*[_envVar.size()];
        for(std::map<std::string, std::string>::iterator  it = _envVar.begin(); it  < _envVar.end(); it++)
        {
            std::string tmp = it->first + "=" + it->second;
            envVar[i] = new char[tmp.size() + 1];
            strcpy(envVar[i], tmp.c_str());
            i++;
        }

    }
    catch(const std::exception& e)
    {
        //gestionar error new
        std::cerr << e.what() << '\n';
    }
    
    
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
