#include "Server.hpp"

volatile sig_atomic_t g_running = 1;

void handleSignal(int sig)
{
    (void)sig;
    std::cout << std::endl;
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
}