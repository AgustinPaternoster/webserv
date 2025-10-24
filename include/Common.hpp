#ifndef _COMMON_HPP_
#define _COMMON_HPP_

#include <iostream>
#include <exception>
#include <vector>

#define ARG_ERROR_MSG "exactly one argument is required — the configuration file."

typedef enum methods
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