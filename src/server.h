#pragma once

#include <boost/asio.hpp>
#include "async.h"

namespace bulk
{
    void
    client_session ( boost::asio::ip::tcp::socket sock, async::handle_t& handler );

    class Server
    {
    public:
        Server          ( unsigned short port, size_t bulk_size );
        ~Server         ( );

        void
        serve_forever   ( );

        void
        stop            ( );

    private:
        unsigned short  port;
        async::handle_t handler;
        bool            __alive;
    };
}
