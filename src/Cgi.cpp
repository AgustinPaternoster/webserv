#include "Cgi.hpp"


Cgi::Cgi(HttpRequest &request, std::vector<struct pollfd> &poll_fds, int poll_id, t_server config):
    _request(request), _poll_fds(poll_fds) , _config(config), _poll_id(poll_id)
{
    _parseRequestToEnv();
    /// borrar 
    for(std::map<std::string, std::string>::iterator it = _envVar.begin(); it != _envVar.end(); it++)
        std::cout << (*it).first << "=" << (*it).second << std::endl;
}

Cgi::~Cgi(void){}

std::string Cgi::_normalizeHeadersName(std::string& name, bool ishppt)
{
    std::string NormalizedName = name;
    for (std::string::iterator it = NormalizedName.begin(); it != NormalizedName.end(); it++)
    {
        *it = toupper(*it);
        if(*it == '-')
             *it = '_';
    }
    if (ishppt)
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

void Cgi::_parseRequestToEnv(void)
{
    std::map<std::string , std::string> header = (_request.getHeaders()).get_all_http();
    _parseHeaderToCGIEnv(header);
    _extracScriptName();
    _envVar["REQUEST_METHOD"] = _request.getMethod();
    _envVar["SERVER_PORT"] = getServerPort(_poll_fds[_poll_id].fd);
    _envVar["REMOTE_ADDR"] = getClientIP(_poll_fds[_poll_id].fd);
    _envVar["SERVER_PROTOCOL"] = _request.getVersion();
    _envVar["GATEWAY_INTERFACE"] = "CGI/1.1.";

}

char** Cgi::_getEnvVar(void)
{
    char** envVar = NULL;
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
        envVar[_envVar.size()] = NULL;
    }
    catch(const std::exception& e)
    {
        //gestionar error new // sistem error
        std::cerr << e.what() << '\n';
    }
    return(envVar);
}

void Cgi::_extracScriptName(void)
{
    std::string scrip_name = "";
    std::string uri = _request.getUri();
    std::string cgi_prefix = _config.locations[0].path;
    std::string cgi_ext = _config.locations[0].cgi_extension.first;

    if(uri.size() < cgi_prefix.size() || uri.substr(0, cgi_prefix.size()) != cgi_prefix)
    { 
        // error
        return;
    }

    size_t ext_pos = uri.find(cgi_ext, cgi_prefix.size());
    if(ext_pos == std::string::npos)
    {
        // error 404/403
        return;
    }

    size_t script_end = ext_pos + cgi_ext.size();
    if(script_end < uri.size())
    {
        char next_char = uri[script_end];
        if(next_char != '/' && next_char != '?')
        {
                //error ( 404 0 400)
                return;
        }
        else
        {
            _envVar["SCRIPT_NAME"] = uri.substr(0, script_end);
            std::string remaining_path = uri.substr(script_end);
            size_t query_pos = remaining_path.find('?');

            if(query_pos != std::string::npos)
            {
                _envVar["PATH_INFO"] = remaining_path.substr(0, query_pos);
                _envVar["QUERY_STRING"] = remaining_path.substr(query_pos + 1);
            }
            else
            {
                _envVar["PATH_INFO"] = remaining_path;
                _envVar["QUERY_STRING"] = "";
            }
        }   
    }
    else
    {
        _envVar["SCRIPT_NAME"] = uri.substr(0, script_end);
        _envVar["PATH_INFO"] = "";
        _envVar["QUERY_STRING"] = "";
    }
}

void Cgi::CgiHandler(CgiTask &cgijobs)
{
    // comprobar si los metodos son los permitidos
    
    struct pollfd cgi_poll_item;
    t_cgi_job cgiTask;
    

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

            cgi_poll_item.fd = pipe_out[0];
            cgi_poll_item.events = POLLIN;
            cgi_poll_item.revents = 0;
            _poll_fds.push_back(cgi_poll_item);

            cgiTask.cgi_read_fd = pipe_out[0];
            cgiTask.cgi_write_fd = -1;
            cgiTask.client_fd = _poll_fds[_poll_id].fd;
            cgiTask.pid = pid;
            cgiTask.header_parsed = false;
            cgiTask.body_written = true;

            cgijobs.AddNewCgiTask(pipe_out[0], cgiTask);
            

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
        {
               //error
               exit(EXIT_FAILURE);
        }
        if(dup2(pipe_out[1], STDOUT_FILENO))
        {
            //error
            exit(EXIT_FAILURE);
        }
        close(pipe_in[0]);
        close(pipe_in[1]);
        close(pipe_out[0]);
        close(pipe_out[1]);
        _closeAllFd();
        
    }
};


void Cgi::_closeAllFd(void)
{
    for(int i = 0; i < _poll_fds.size(); i++)
    {
        int fdToclose = _poll_fds[i].fd;
        if(fdToclose != STDERR_FILENO && fdToclose != STDIN_FILENO && fdToclose != STDOUT_FILENO)
            close(fdToclose);
    }
}

void Cgi::_executeCgi(void)
{
    const char*cgi_executable = _config.locations[0].cgi_extension.second.c_str();
    
    char *const argv[] = {
        const_cast<char*>(cgi_executable),
        const_cast<char*>(_envVar["SCRIPT_NAME"])
    }
}
