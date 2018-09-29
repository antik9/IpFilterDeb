#include <iostream>
#include "async.h"
#include "server.h"
#include <string.h>

namespace bulk
{
    Server::Server  ( unsigned short port, size_t bulk_size ) :
        port ( port ), handler ( async::connect ( bulk_size ) ), __alive ( false ) { }
    Server::~Server ( ) { async::disconnect ( handler ); }

    void
    Server::serve_forever   ( )
    {

        boost::asio::io_service         service;
        boost::asio::ip::tcp::endpoint  ep (
            boost::asio::ip::tcp::v4 ( ),
            9999
        );

        boost::asio::ip::tcp::acceptor acc ( service, ep );
        while ( true )
        {
            auto sock = boost::asio::ip::tcp::socket ( service );
            acc.accept ( sock );
            std::thread ( bulk::client_session, std::move ( sock ), std::ref ( handler ) ).detach ( );
        }
    }

    void
    client_session ( boost::asio::ip::tcp::socket sock, async::handle_t& handler )
    {
        while ( true ) {
            try {
                char data[128];
                std::memset ( data, '\0', 128);
                size_t len = sock.read_some ( boost::asio::buffer ( data ) );
                async::receive ( handler, data, len );
            }
            catch ( const std::exception &e ) {
                std::cerr <<  "__exc: " << e.what ( ) << std::endl;
                break;
            }
       }
    }

    void
    Server::stop            ( )
    {
    }
}
