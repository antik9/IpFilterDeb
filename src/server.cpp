#include <iostream>
#include <sstream>
#include <string.h>
#include <thread>

#include "server.h"

namespace join
{

    Server::Server  ( unsigned short port ) : port ( port ) { }

    void
    Server::serve_forever   ( )
    {

        boost::asio::io_service         service;
        boost::asio::ip::tcp::endpoint  ep (
            boost::asio::ip::tcp::v4 ( ),
            port
        );

        boost::asio::ip::tcp::acceptor acc ( service, ep );

        while ( true )
        {
            auto sock = boost::asio::ip::tcp::socket ( service );
            acc.accept ( sock );
            Session session { std::move ( sock ), &database };
            std::thread ( std::move ( session ) ).detach ( );
        }
    }

    Session::Session ( boost::asio::ip::tcp::socket sock, Database* database_ptr ) :
        sock ( std::move ( sock ) ), database_ptr ( database_ptr ) { }

    void
    Session::operator() ( )
    {
        while ( true ) {
            try {
                std::memset ( data, '\0', 128);
                sock.read_some( boost::asio::buffer ( data ) );
                buffer << data;

                for ( int i = 0; i < 128; ++i )
                {
                    if ( data[i] == '\n' )
                    {
                        buffer >> command;
                        std::getline ( buffer, command_args );

                        /* Telnet \r replace */
                        int carriage_return;
                        if ( ( carriage_return = command_args.find ('\r' ) ) != std::string::npos )
                        {
                            command_args.replace ( carriage_return, 1, "" );
                        }

                        int idx = 0;
                        while ( idx < command_args.size ( )
                                and command_args[idx] == join::SPACE_SEPARATOR )
                        {
                            ++idx;
                        }

                        command_args = command_args.substr ( idx );
                        if ( command == "INSERT" )
                        {
                            sock.write_some(
                                boost::asio::buffer ( database_ptr->insert ( command_args ) ) );
                        }
                        else if ( command == "TRUNCATE" )
                        {
                            sock.write_some(
                                boost::asio::buffer ( database_ptr->truncate ( command_args ) ) );
                        }
                        else if ( command == "INTERSECTION" )
                        {
                            sock.write_some(
                                boost::asio::buffer ( database_ptr->inner_join ( ) ) );
                        }
                        else if ( command == "SYMMETRIC_DIFFERENCE" )
                        {
                            sock.write_some(
                                boost::asio::buffer ( database_ptr->anti_join ( ) ) );
                        }
                        else
                        {
                            sock.write_some(
                                boost::asio::buffer ( "Unknown command\n" ) );
                        }
                    }

                }
            }
            catch ( const std::exception &e ) {
                sock.write_some ( boost::asio::buffer ( "UNKNOWN ERROR\n" ) );
                break;
            }
       }
    }
}
