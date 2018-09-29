#include <gtest/gtest.h>
#include <cstdio>
#include <dirent.h>
#include <sstream>
#include <stdlib.h>
#include <string>
#include <thread>
#include "../src/server.h"


class TestBulk : public ::testing::Test
{

protected:
    void
    SetUp ( )
    {

    }

    void
    TearDown ( )
    {
        DIR* test_dir = opendir(".");
        struct dirent* file_;
        std::string LOG_BEGIN_PATTERN   = "bulk";
        std::string LOG_END_PATTERN     = ".log";

        while ( (file_ = readdir (test_dir)) != NULL )
        {
            std::string filename = file_->d_name;
            if ( LOG_BEGIN_PATTERN.compare(filename.substr(0, 4)) == 0 &&
                    LOG_END_PATTERN.compare(filename.substr(filename.size() - 4)) == 0 )
            {
                std::cout << "rm " << filename << std::endl;
                std::remove(filename.c_str());
            }
        }

        closedir(test_dir);
    }
};

TEST_F(TestBulk, two_simple_bulks)
{
    testing::internal::CaptureStdout();

    auto handle         = async::connect ( 2 );
    async::receive      ( handle, "cat\ndog\ntac\n", 12 );
    async::disconnect   ( handle );

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
    );
}

TEST_F(TestBulk, three_handlers)
{
    testing::internal::CaptureStdout();

    auto handle_1       = async::connect ( 2 );
    async::receive      ( handle_1, "cat\ndog\ntac\n", 12 );

    auto handle_2       = async::connect ( 3 );
    async::receive      ( handle_2, "cat\ndog\ntac\n", 12 );

    auto handle_3       = async::connect ( 4 );
    async::receive      ( handle_3, "cat\ndog\ntac\nyes\n", 16 );

    async::disconnect   ( handle_1 );
    std::this_thread::sleep_for ( std::chrono::milliseconds ( 10 ) );
    async::disconnect   ( handle_2 );
    std::this_thread::sleep_for ( std::chrono::milliseconds ( 10 ) );
    async::disconnect   ( handle_3 );
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
            "bulk: cat, dog, tac\n"
            "bulk: cat, dog, tac, yes\n"
    );
}

TEST_F(TestBulk, fragmented_receive)
{
    testing::internal::CaptureStdout();

    auto handle = async::connect ( 2 );
    async::receive      ( handle, "cat\ndog\ntac", 11 );
    async::receive      ( handle, "\nyes\n", 5 );
    async::disconnect   ( handle );

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, yes\n"
    );
}

TEST_F(TestBulk, blocks)
{
    testing::internal::CaptureStdout();

    auto handle = async::connect ( 2 );
    async::receive      ( handle, "cat\ndog\ntac\n{\nyes\nhelp\n}\n", 25 );
    async::disconnect   ( handle );

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
            "bulk: yes, help\n"
    );
}

class TestServer: public ::testing::Test
{
protected:
    boost::asio::ip::tcp::endpoint  ep;
    unsigned short  PORT        = 13095;
    size_t          BULK_SIZE   = 2;

    void
    SetUp ( )
    {
        ep = boost::asio::ip::tcp::endpoint (
                boost::asio::ip::address::from_string ( "127.0.0.1" ),
                PORT
        );
    }

    void
    TearDown ( )
    {
        DIR* test_dir = opendir(".");
        struct dirent* file_;
        std::string LOG_BEGIN_PATTERN   = "bulk";
        std::string LOG_END_PATTERN     = ".log";

        while ( (file_ = readdir (test_dir)) != NULL )
        {
            std::string filename = file_->d_name;
            if ( LOG_BEGIN_PATTERN.compare(filename.substr(0, 4)) == 0 &&
                    LOG_END_PATTERN.compare(filename.substr(filename.size() - 4)) == 0 )
            {
                std::cout << "rm " << filename << std::endl;
                std::remove(filename.c_str());
            }
        }

        closedir(test_dir);
    }
};

void
__write ( boost::asio::ip::tcp::socket sock, boost::asio::ip::tcp::endpoint& ep,
            std::string buffer, size_t timeout )
{
    std::this_thread::sleep_for ( std::chrono::milliseconds ( timeout ) );
    sock.connect ( ep );
    boost::asio::write ( sock, boost::asio::buffer ( buffer, buffer.size ( ) ) );
}

TEST_F ( TestServer, simple_package )
{
    setenv("TEST_BULK", "1", 1);
    testing::internal::CaptureStdout();


    boost::asio::io_service client_1;
    boost::asio::ip::tcp::socket sock ( client_1 );

    std::thread (
        __write, std::move ( sock ), std::ref ( ep ),
        "cat\ndog\ntac\n{\nyes\nhelp\n}\n", 100
    ).detach ( );

    {
        bulk::Server server ( PORT, BULK_SIZE );
        server.serve_forever ( );
    }

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
            "bulk: yes, help\n"
    );
}

TEST_F ( TestServer, two_clients )
{
    setenv("TEST_BULK", "2", 1);
    testing::internal::CaptureStdout();


    boost::asio::io_service client_1;
    boost::asio::ip::tcp::socket sock_1 ( client_1 );
    boost::asio::io_service client_2;
    boost::asio::ip::tcp::socket sock_2( client_2 );

    std::thread (
        __write, std::move ( sock_1 ), std::ref ( ep ),
        "ls\n", 100
    ).detach ( );

    std::thread (
        __write, std::move ( sock_2 ), std::ref ( ep ),
        "cat\ndog\n{\nyes\nhelp\n}\n", 200
    ).detach ( );

    {
        bulk::Server server ( PORT, BULK_SIZE );
        server.serve_forever ( );
    }

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: ls, cat\n"
            "bulk: dog\n"
            "bulk: yes, help\n"
    );
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
