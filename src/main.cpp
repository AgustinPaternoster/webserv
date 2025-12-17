#include "Config.hpp"
#include "Socket.hpp"

int main(int arc, char** argv)
{
    Config conFile;

    try
    {
        switch (arc)
        {
            case 1:
                conFile = Config(DEFAULTCONFIG);
                break;
            case 2:
                conFile = Config(argv[1]);
                break;
            default:
                throw std::invalid_argument(ARG_ERROR_MSG);
                break;
        }
        Socket::webserver_init(conFile);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return (1);
    }
    return (0);
}
