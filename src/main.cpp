#include "test.hpp"
#include <iostream>

int main(int ar, char** argv)
{
    
    (void)ar;
    (void)argv;   
    test a(4);
    std::cout << a.getNb() << std::endl;
        std::cout << "test" << std::endl;
    return (0);
}