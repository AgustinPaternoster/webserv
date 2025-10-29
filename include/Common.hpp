#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <exception>
#include <fstream>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <csignal>
#include <Socket.hpp>

// Errores
#define ARG_ERROR_MSG "exactly one argument is required — the configuration file."
#define ERROR_FILE_NOT_OPEN "the specified file could not be opened."
#define CONFIG_NO_SERVER_ERROR "Configuration file must define at least one server."
//volatile int g_signal = true;


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
   int port;
   std::string root;
   std::string error_page;
   std::vector <t_location> locations;
 }t_server;
 


#endif
