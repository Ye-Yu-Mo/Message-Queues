#include "../server/broker.hpp"

int main()
{
    XuMQ::Server server(8888, "./data/");
    server.start();
    return 0;
}