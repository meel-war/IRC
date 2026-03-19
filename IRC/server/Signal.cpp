#include "Server.hpp"

volatile sig_atomic_t g_running = 1;

void handleSignal(int sig)
{
    (void)sig;
    std::cout << std::endl;
    //std::cout << "Signal received\n"; // pour tester;
    g_running = 0;
}

void setupSignal()
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handleSignal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    struct sigaction ign;
    sigemptyset(&ign.sa_mask);
    ign.sa_flags = 0;
    ign.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &ign, NULL);
}