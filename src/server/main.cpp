#include <iostream>

#include "arc_server.hpp"

int main()
{
    arc::server server(8585);

    std::cin.get();

    return 0;
}

