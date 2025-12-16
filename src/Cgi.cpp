#include "Cgi.hpp"
#include <cerrno> // Necesario para errno
#include <cstdio> // Necesario para perror


Cgi::Cgi(HttpRequest &request, std::vector<struct pollfd> &poll_fds, int poll_id, t_server config):
    _request(request), _poll_fds(poll_fds) , _config(config), _poll_id(poll_id){}

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

        if (name == "content-type" || name == "content-length")
        {
            std::string CgiName = _normalizeHeadersName(name, false);
            _envVar[CgiName] = value;
        }
        else
        {
            std::string cgiName = _normalizeHeadersName(name, true);
            _envVar[cgiName] = value;
        }
   }

}

int Cgi::parseRequestToEnv(void)
{
    int status;
    std::map<std::string , std::string> header = (_request.getHeaders()).get_all_http();
    _parseHeaderToCGIEnv(header);
    if((status =_extracScriptName())!= 200)
        return(status);
    _envVar["REQUEST_METHOD"] = _request.getMethod();
    _envVar["SERVER_PORT"] = getServerPort(_poll_fds[_poll_id].fd);
    _envVar["REMOTE_ADDR"] = getClientIP(_poll_fds[_poll_id].fd);
    _envVar["SERVER_PROTOCOL"] = _request.getVersion();
    _envVar["GATEWAY_INTERFACE"] = "CGI/1.1.";
    return (200);
}

char** Cgi::_getEnvVar(void)
{
    char** envVar = NULL;
    int i = 0;
    try
    {
        envVar = new char*[_envVar.size() + 1];
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

int Cgi::_extracScriptName(void)
{
    std::string scrip_name = "";
    std::string uri = _request.getUri();
    std::string cgi_prefix = _config.locations[0].path;
    std::string cgi_ext = _config.locations[0].cgi_extension.first;

    if(uri.size() < cgi_prefix.size() || uri.substr(0, cgi_prefix.size()) != cgi_prefix)
    { 
        return (404);
    }

    size_t ext_pos = uri.find(cgi_ext, cgi_prefix.size());
    if(ext_pos == std::string::npos)
    {
        return(404);
    }

    size_t script_end = ext_pos + cgi_ext.size();
    if(script_end < uri.size())
    {
        char next_char = uri[script_end];
        if(next_char != '/' && next_char != '?')
        {
                return(404);
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
    return (200);
}

int Cgi::CgiHandler(CgiTask &cgijobs)
{
    

    struct pollfd cgi_poll_item;
    t_cgi_job cgiTask;
    

    int pipe_in[2];
    int pipe_out[2];
    pid_t pid;

    if (pipe(pipe_in) < 0 || pipe(pipe_out) < 0)
    {
        std::cerr << "CGI Error: Fallo al crear pipes." << std::endl;
        int client_fd = _poll_fds[_poll_id].fd;


    }        
    
    pid = fork();
    if( pid < 0)
    {
        //error;
    }    
    if(pid != 0)
    {
        close(pipe_in[0]);
        close(pipe_out[1]);
        
        cgiTask.cgi_read_fd = pipe_out[0];
        cgiTask.client_fd = _poll_fds[_poll_id].fd;
        cgiTask.pid = pid;
        cgiTask.header_parsed = false;
        
        if(_envVar["REQUEST_METHOD"] == "GET")
        {
            close(pipe_in[1]);
            cgiTask.cgi_write_fd = -1;
            cgiTask.body_written = true;
        }   
        if(_envVar["REQUEST_METHOD"] == "POST")
        {
            const std::string body_data = _request.getBody();
            ssize_t total_written = 0;
            size_t body_len = body_data.length();
            while (total_written < (ssize_t)body_len) 
            {
                ssize_t bytes_sent = write(
                    pipe_in[1], 
                    body_data.c_str() + total_written, 
                    body_len - total_written
                );
                if (bytes_sent < 0) 
                {
                    std::cerr << "Error escribiendo body POST al CGI. " << std::endl;
                    // matar el proceso CGI y enviar 500 al cliente//
                    break; 
                }
                total_written += bytes_sent;            
            }
            close(pipe_in[1]);
            cgiTask.cgi_write_fd = -1;
            cgiTask.body_written = true;

        }

        int flags = fcntl(pipe_out[0], F_GETFL, 0);
        if (flags == -1 || fcntl(pipe_out[0], F_SETFL, flags | O_NONBLOCK) == -1) 
        { 
            /* Manejar error */ 
        }

        cgi_poll_item.fd = pipe_out[0];
        cgi_poll_item.events = POLLIN;
        cgi_poll_item.revents = 0;
        _poll_fds.push_back(cgi_poll_item);
        cgijobs.AddNewCgiTask(pipe_out[0], cgiTask);         
    }
        if(pid == 0)
        {
            if(dup2(pipe_in[0], STDIN_FILENO) < 0)
            {
                //error
                exit(EXIT_FAILURE);
            }
            if(dup2(pipe_out[1], STDOUT_FILENO) < 0)
            {
                //error
                exit(EXIT_FAILURE);
            }
            _closeAllFd();
            close(pipe_in[0]);
            close(pipe_in[1]);
            close(pipe_out[0]);
            close(pipe_out[1]);
            _executeCgi();
        }
        return (200);
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
    const char *script_name = _getScriptFileName();
    const char*directory_path = _getDirectoryPath();
    const char*cgi_executable = _config.locations[0].cgi_extension.second.c_str();
    char **envp = _getEnvVar();
    
    char *const argv[] = {
        const_cast<char*>(cgi_executable),
        const_cast<char*>(script_name),
        NULL
    };
    if(chdir(directory_path) < 0)
    {
        _freeCGIResources(envp,script_name,directory_path);
        exit(EXIT_FAILURE);
    }
    if(execve(cgi_executable, argv, envp) == -1)
    {
        _freeCGIResources(envp,script_name,directory_path);
        exit(EXIT_FAILURE);
    }
}

const char* Cgi::_getScriptPath(void)
{
    std::string script_uri = _envVar.at("SCRIPT_NAME");
    std::string location_path = _config.locations[0].path;
    std::string location_root = _config.locations[0].root;
    std::string relative_path_segment;

    if (script_uri.find(location_path) == 0)
        relative_path_segment = script_uri.substr(location_path.length());
    else 
        relative_path_segment = script_uri;
    
    std::string full_path_temp = location_root;

    if (full_path_temp.empty() || full_path_temp[full_path_temp.length() - 1] != '/')
        full_path_temp += "/";
    full_path_temp += relative_path_segment;
    
    char* script_path = new char[full_path_temp.length() + 1];
    std::strcpy(script_path, full_path_temp.c_str());
    
    return script_path;
}

const char* Cgi::_getDirectoryPath(void)
{
    std::string script_uri = _envVar.at("SCRIPT_NAME");
    std::string location_path = _config.locations[0].path;
    std::string location_root = _config.locations[0].root;
    std::string relative_path_segment;

    if (script_uri.find(location_path) == 0)
        relative_path_segment = script_uri.substr(location_path.length());
    else 
        relative_path_segment = script_uri;
    
    std::string full_path_temp = location_root;

    if (full_path_temp.empty() || full_path_temp[full_path_temp.length() - 1] != '/')
        full_path_temp += "/";
    full_path_temp += relative_path_segment;
    std::string dir_path_temp;
    size_t last_slash = full_path_temp.find_last_of('/');
    
    if (last_slash == std::string::npos)
        dir_path_temp = ".";
    else 
        dir_path_temp = full_path_temp.substr(0, last_slash);
    char* directory_path = new char[dir_path_temp.length() + 1];
    std::strcpy(directory_path, dir_path_temp.c_str());
    
    return directory_path;
}

void Cgi::_freeCGIResources(char** envp, const char* script_path, const char* directory_path)
{
     if (envp)
    {
        for (int i = 0; envp[i] != NULL; ++i)
            delete[] envp[i];
        delete[] envp;
    }
    if (script_path)
        delete[] const_cast<char*>(script_path);
    if (directory_path)
        delete[] const_cast<char*>(directory_path);
}

const char* Cgi::_getScriptFileName(void)
{
    std::string script_uri = _envVar.at("SCRIPT_NAME");
    std::string location_path = _config.locations[0].path;
    std::string location_root = _config.locations[0].root;
    std::string relative_path_segment;

    if (script_uri.find(location_path) == 0)
        relative_path_segment = script_uri.substr(location_path.length());
    else 
        relative_path_segment = script_uri;
    
    std::string full_path_temp = location_root;
    if (full_path_temp.empty() || full_path_temp[full_path_temp.length() - 1] != '/')
        full_path_temp += "/";
    full_path_temp += relative_path_segment;
       
    size_t last_slash = full_path_temp.find_last_of('/');
    std::string script_filename_str;
    
    if (last_slash == std::string::npos)
        script_filename_str = full_path_temp;
    else 
        script_filename_str = full_path_temp.substr(last_slash + 1);
 
    char* script_filename = new char[script_filename_str.length() + 1];
    std::strcpy(script_filename, script_filename_str.c_str());
    
    return script_filename; 
}