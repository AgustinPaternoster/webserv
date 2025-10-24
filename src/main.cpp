#include "Common.hpp"

int main(int arc, char** argv)
{
    try
    {
		(void)argv;
        if (arc < 2 || arc > 3)
            throw std::invalid_argument(ARG_ERROR_MSG);
        Socket::webserver_init();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return (1);
    }
    return (0);
}
