#pragma once

#include <boost/asio.hpp>
#include "database.h"

namespace join
{
    const std::string INSERT                = "INSERT";
    const std::string INTERSECTION          = "INTERSECTION";
    const std::string SYMMETRIC_DIFFERENCE  = "SYMMETRIC_DIFFERENCE";
    const std::string TRUNCATE              = "TRUNCATE";
    const std::string UNKNOWN_COMMAND       = "Unknown command\n";
    const std::string UNKNOWN_ERROR         = "UNKNOWN ERROR\n";

    struct
    Session {
        Session ( boost::asio::ip::tcp::socket sock, Database* database_ptr );
        Session ( Session&& sess );

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
