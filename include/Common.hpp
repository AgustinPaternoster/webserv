#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <cstdlib>
#include <sstream>
#include <string>
#include <exception>
#include <fstream>
#include <poll.h>
#include <vector>
#include <map>
#include <ctype.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <csignal>
#include <sys/stat.h>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <ctime>
#include <iomanip>

// Errores
#define ARG_ERROR_MSG "Provide either one argument with the configuration file path, or none to use the default configuration."
#define ERROR_FILE_NOT_OPEN "the specified file could not be opened."
#define CONFIG_NO_SERVER_ERROR "Configuration file must define at least one server."
#define SERVER_CONFIG_ERROR "There is a failure in the server setup."
#define SERVER_DUPLICATED_ERROR "There is a duplicated server port."
//volatile int g_signal = true;

#define  DEFAULTCONFIG "./default_config"

enum methods
{
   GET = 0,
   POST = 1,
   DELETE = 2,
   PUT = 3,
   PATCH = 4,
   UNKNOWN = -1
};

enum actions
{
   STATIC = 0,
   CGI = 1,
   UPLOAD = 2
};

//estructura cgiJobs

// estructura location //
typedef struct s_location
{
   std::string path;
   std::string root;
   std::vector <int> methods;
   std::string autoindex;
   std::string upload_store;
   std::string client_max_body_size;
   std::string index;
   std::pair<std::string, std::string> cgi_extension;
   std::pair<std::string, std::string> redirecction;
   int actions;
}t_location;



/// estructura servers //
typedef struct s_server
{
   std::string port;
   std::string root;
   std::string index;
   std::map<std::string,std::string> error_page;
   std::string client_max_body_size;
   std::vector <t_location> locations;
}t_server;

typedef struct 
{
    int             cgi_read_fd;     
    int             cgi_write_fd;     
    int             client_fd;        
    pid_t           pid;              
    std::string     cgi_output_buffer;
    bool            header_parsed;    
    bool            body_written;
    t_server        server;
    time_t          start_time;
} t_cgi_job;

#endif
