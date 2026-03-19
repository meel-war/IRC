#include "server/Server.hpp"

int main(int ac, char **av)
{

    if(ac != 3)
    {
        std::cout << "./irc <port> <password>" << std::endl;
        return (1);
    }

    setupSignal();

    try
    {   
        Server server(av[1], av[2]);
        while(g_running)
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
