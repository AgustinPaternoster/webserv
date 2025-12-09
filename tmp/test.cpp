#include <iostream>
#include <map>
#include <string>

int main(void)
{
    std::string test = "asdb1234--ff";

    for (std::string::iterator it = test.begin(); it != test.end(); it++)
    {
        *it = toupper(*it);
        if(*it == '-')
            *it = '_';
    }

    std::cout << test << std::endl;
}