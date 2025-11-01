#include "Common.hpp"
#include "Config.hpp"

int main(int arc, char** argv)
{
    try
    {

        if (arc < 2 || arc > 3)
            throw std::invalid_argument(ARG_ERROR_MSG);
        Config conFile(argv[1]);
        //Socket::webserver_init();
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return (1);
    }
    return (0);
}
