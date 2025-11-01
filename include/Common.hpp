#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <exception>
#include <fstream>
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
#include <Socket.hpp>

// Errores
#define ARG_ERROR_MSG "Provide either one argument with the configuration file path, or none to use the default configuration."
#define ERROR_FILE_NOT_OPEN "the specified file could not be opened."
#define CONFIG_NO_SERVER_ERROR "Configuration file must define at least one server."
#define SERVER_CONFIG_ERROR "There is a failure in the server setup."
//volatile int g_signal = true;

#define  DEFAULTCONFIG "./default_config"

enum methods
{
   GET = 0,
   POST = 1,
   DELETE = 2,
};



// estructura location //
typedef struct s_location
{
   std::string path;
   std::vector <int> methods;
}t_location;



/// estructura servers //
 typedef struct s_server
 {
   std::string port;
   std::string root;
   std::string error_page;
   std::vector <t_location> locations;
 }t_server;
 


#endif
