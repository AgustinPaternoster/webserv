#include "Common.hpp"




void CgiHandler(void)
{
    // configurar las variables de entorno

    // hacer comprobaciones si el directorio existe o si el archivo existe
    std::map<std::string, std::string> envVar;
    std::string method;
    std::string body;

    pid_t pid;
    int fd[2];
    if(method == "GET")
    {
        close(fd[0]);
        close(fd[1]);
    }
    if(method == "POST")
    {
        close(fd[0]);
        write(fd[1], body.c_str() , body.size());
        close(fd[0]);
    }
    if(pipe(fd) < 0)
        // error
    pid = fork();

    if(pid < 0)
        // error
    if (pid == 0)
    {
        // proceso hijo
    }


    // configurar la response
}