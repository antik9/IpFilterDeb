#pragma once

#include <boost/asio.hpp>
#include "async.h"

namespace bulk
{

    struct 
    Session {
        Session ( boost::asio::ip::tcp::socket sock, const async::handle_t& handler );

        void
        operator() ( );

    private:
        boost::asio::ip::tcp::socket    sock;
        const async::handle_t&          handler;
        std::string                     block;
        std::string                     buffer;
        char                            data[128];
        size_t                          block_inits;
        size_t                          eol_index;
    };

    void
    client_session ( Session session );

    class Server
    {
    public:
        Server          ( unsigned short port, size_t bulk_size );
        ~Server         ( );

        void
        serve_forever   ( );

    private:
        unsigned short  port;
        async::handle_t handler;
        bool            __alive;
    };
}
