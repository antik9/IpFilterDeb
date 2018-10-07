#include <csignal>
#include <iostream>
#include "server.h"

int
main ( int argc, char **argv )
{
    if ( argc != 2 )
    {
        std::cout << "Usage: join_server <port>\n";
        return 1;
    }

    unsigned short port = std::atoi ( argv[1] );

    if ( port == 0 )
    {
        std::cout << "Port cannot be `0`\n";
        return 2;
    }

    if ( ::fork ( ) == 0 )
    {
        join::Server server ( port  );
        server.serve_forever ( );
    }

    return 0;
}
