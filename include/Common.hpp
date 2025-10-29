#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <poll.h>
#include <vector>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <errno.h>
#include <fcntl.h>
#include <csignal>
#include <Socket.hpp>

#define ARG_ERROR_MSG "exactly one argument is required — the configuration file."


/*
typedef enum methods
{
   GET = 0,
   POST = 1,
   DELETE = 2,
	////STRUCT COMMENTED SINCE IS SHOWING AN ERROR IN TERMINAL
	// AND NOT NECESSARY FOR THE MOMENT
};
*/


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
