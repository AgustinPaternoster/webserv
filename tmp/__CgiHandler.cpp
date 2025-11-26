#include "../include/Common.hpp"


std::map<std::string, std::string> setupCgiEnvironment(void)
{
    std::map <std::string, std::string> tmp;
    tmp["REQUEST_METHOD"] = "GET";
    tmp["SERVER_PROTOCOL"] = "HTTP/1.1";
    tmp["QUERY_STRING"] = "";
    tmp["SERVER_PORT"] = "8080";
    tmp["GATEWAY_INTERFACE"] = "CGI/1.1";
    tmp["CONTENT_LENGTH"] = "150";
    tmp["CONTENT_TYPE"] = "application/x-www-form-urlencoded";
}

bool validateCgiResources(t_server server)
{
    std::string uri;
    std::string file = server.locations[0].root + uri.substr(uri.find_last_of('/'));
    if(accept(server.locations[0].root.c_str(), F_OK | X_OK))
        // error directorio no existe o no tienes los permisos
    if()
    
}


void CgiHandler(void)
{
    // configurar las variables de entorno

    // hacer comprobaciones si el directorio existe o si el archivo existe
    std::map<std::string, std::string> envVar = setupCgiEnvironment();
    std::string body;

    pid_t pid;
    int fd[2];
    if(pipe(fd) < 0)
        // error
    pid = fork();
    if(envVar["REQUEST_METHOD"] == "GET")
    {
        close(fd[0]);
        close(fd[1]);
    }
    if(envVar["REQUEST_METHOD"] == "POST")
    {
        close(fd[0]);
        write(fd[1], body.c_str() , body.size());
        close(fd[0]);
    }

    if(pid < 0)
        // error
    if (pid == 0)
    {
        // proceso hijo
    }


    // configurar la response
}