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
#include <Socket.hpp>

#define ARG_ERROR_MSG "exactly one argument is required — the configuration file."

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
