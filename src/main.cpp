#include <boost/asio.hpp>
#include <iostream>
#include "server.h"


void client_session(boost::asio::ip::tcp::socket sock) {
    while (true) {
        try {
            char data[4];
            size_t len = sock.read_some(boost::asio::buffer(data));
            std::cout << "receive " << len << "=" << std::string{data, len} << std::endl;
            boost::asio::write(sock, boost::asio::buffer("pong\n", 5));
        }
        catch (const std::exception &e) {
            std::cerr << e.what() << std::endl;
            break;
        }
    }
}

int main(int, char *[]) {

    // boost::asio::io_service service;
    // boost::asio::ip::tcp::endpoint ep(
    //     boost::asio::ip::tcp::v4(),
    //     9999
    // );

    // boost::asio::ip::tcp::acceptor acc(service, ep);
    // while (true) {
    //     auto sock = boost::asio::ip::tcp::socket(service);
    //     acc.accept(sock);
    //     std::thread(client_session, std::move(sock)).detach();
    // }

    bulk::Server server ( 9999, 3 );
    server.serve_forever ( );

    return 0;
}
