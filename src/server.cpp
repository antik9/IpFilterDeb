#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "async.h"
#include "server.h"

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
            port
        );

        boost::asio::ip::tcp::acceptor acc ( service, ep );

        /* TEST branch */
        auto test_enabled = getenv ( "TEST_BULK" );
        if ( test_enabled )
        {
            for ( int i = 0; i < atoi ( test_enabled ); ++i )
            {
                auto sock = boost::asio::ip::tcp::socket ( service );
                acc.accept ( sock );
                Session session { std::move ( sock ), std::ref ( handler ) };
                std::thread ( std::move ( session ) ).detach ( );
            }
        }
        /* MAIN branch */
        else
        {
            while ( true )
            {
                auto sock = boost::asio::ip::tcp::socket ( service );
                acc.accept ( sock );
                Session session { std::move ( sock ), std::ref ( handler ) };
                std::thread ( std::move ( session ) ).detach ( );
            }
        }
    }

    Session::Session ( boost::asio::ip::tcp::socket sock, const async::handle_t& handler ) :
        sock ( std::move ( sock ) ), handler ( handler ), block_inits ( 0 ) { }

    void
    Session::operator() ( )
    {
        while ( true ) {
            try {
                std::memset ( data, '\0', 128);
                sock.read_some( boost::asio::buffer ( data ) );
                buffer += data;
                auto carriage_return_it = std::remove_if ( buffer.begin ( ), buffer.end ( ),
                                            [] ( char ch ) { return ch == '\r'; } );
                buffer.erase ( carriage_return_it, buffer.end ( ) );

                while ( ( eol_index = buffer.find ( '\n' ) ) != std::string::npos )
                {
                    if ( buffer.size ( ) > 1 and buffer.substr ( 0, 2 ) == NEW_BLOCK_INIT_EOL )
                    {
                        ++block_inits;
                        block += NEW_BLOCK_INIT_EOL;
                    }
                    else if ( buffer.size ( ) > 1 and buffer.substr ( 0, 2 ) == NEW_BLOCK_CLOSE_EOL )
                    {
                        --block_inits;
                        block += NEW_BLOCK_CLOSE_EOL;
                        if ( block_inits == 0 and not block.empty ( ) )
                        {
                            async::receive ( handler, block.c_str ( ), block.size ( ) );
                            block.clear ( );
                        }
                    }
                    else if ( block_inits )
                    {
                        block += buffer.substr ( 0, eol_index + 1 );
                    }
                    else
                    {
                        async::receive (
                                handler, buffer.substr ( 0, eol_index + 1 ).c_str ( ),
                                eol_index + 1 );
                    }
                    buffer = buffer.substr ( eol_index + 1 );
                }
            }
            catch ( const std::exception &e ) {
                break;
            }
       }
    }
}
