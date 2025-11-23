#include <iostream>
#include "../include/Common.hpp"

int main(void)
{

    std::string uri = "/cgi/test.py";
    int result;
    std::cout << uri.substr(uri.find_last_of('/')) << std::endl;    
}