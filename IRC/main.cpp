#include "server/Server.hpp"

int main(int ac, char **av)
{

    if(ac != 3)
    {
        std::cout << "./irc <port> <password>" << std::endl;
        return (1);
    }
    try
    {
        Server server(av[1], av[2]);

        char* endptr;
        long int port = strtol(av[1], &endptr, 10);
        if(port < 1024 || port > 65535)
            throw std::runtime_error("Port must be a number between 1024 and 65535"); // Surement a rendre plus clean (temp)

        while(true)
        {
            server.pollClients();
        }
    }
    catch(const std::runtime_error &e)
    {
        std::cerr << "Server error: " << e.what() << std::endl;
        return (1);
    }
    return(0);
}