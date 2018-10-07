#pragma once

#include <boost/asio.hpp>
#include "join.h"

namespace join 
{

    struct
    Session {
        Session ( boost::asio::ip::tcp::socket sock, Database* database_ptr );

        void
        operator() ( );

    private:
        boost::asio::ip::tcp::socket    sock;
        Database*                       database_ptr;
        std::stringstream               buffer;
        std::string                     command;
        std::string                     command_args;
        char                            data[128];
    };

    void
    client_session ( Session session );

    class Server
    {
    public:
        Server          ( unsigned short port );

        void
        serve_forever   ( );

    private:
        unsigned short  port;
        Database database;
    };
}
