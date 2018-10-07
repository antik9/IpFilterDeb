#include <csignal>
#include <iostream>
#include "server.h"

bulk::Server* server_ptr = nullptr;

void
signalHandler ( int signum )
{
    if ( server_ptr )
    {
        server_ptr->~Server ( );
    }
    exit(signum);
}

int
main ( int argc, char **argv )
{
    if ( argc != 3 )
    {
        std::cout << "Usage: bulk_server <port> <bulk_size>\n";
        return 1;
    }

    unsigned short port = std::atoi ( argv[1] );
    size_t  bulk_size   = std::atoi ( argv[2] );

    if ( port == 0 or bulk_size == 0 )
    {
        std::cout << "Nor port nor bulk_size cannot be `0`\n";
        return 2;
    }

    if ( ::fork ( ) == 0 )
    {
        bulk::Server server ( port, bulk_size );
        signal ( SIGINT, signalHandler );
        signal ( SIGTERM, signalHandler );
        server_ptr = &server;
        server.serve_forever ( );
    }

    return 0;
}
