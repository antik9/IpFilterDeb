#include <gtest/gtest.h>
#include <cstdio>
#include <dirent.h>
#include <sstream>
#include <string>
#include "../src/bulk.h"


class TestBulk : public ::testing::Test
{
protected:
    void
    TearDown()
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

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << "tac" << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 2, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
    );
}

TEST_F(TestBulk, simple_plus_bulk)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "tac" << std::endl
        << "rm -rf" << std::endl
        << NEW_BLOCK_CLOSE << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 3, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, rm -rf\n"
    );
}

TEST_F(TestBulk, incorrect_brace)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_CLOSE << std::endl
        << "tac" << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 10, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "ERROR: Incorrect place of end of sequence\n"
    );
}

TEST_F(TestBulk, not_ended_block)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "yes" << std::endl
        << "tac" << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 5, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
    );
}

TEST_F(TestBulk, empty_block_in_the_middle)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << NEW_BLOCK_CLOSE << std::endl
        << "tac" << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 2, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
    );
}

TEST_F(TestBulk, nested_blocks)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "tac" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "open" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "close" << std::endl
        << NEW_BLOCK_CLOSE << std::endl
        << NEW_BLOCK_CLOSE << std::endl
        << NEW_BLOCK_CLOSE << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 2, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, open, close\n"
    );
}


TEST_F(TestBulk, detach_flusher)
{
    testing::internal::CaptureStdout();

    std::stringstream input;
    input
        << "cat" << std::endl
        << "dog" << std::endl
        << NEW_BLOCK_INIT << std::endl
        << "tac" << std::endl
        << "rm -rf" << std::endl
        << NEW_BLOCK_CLOSE << std::endl;

    BulkExecutor executor ( input );
    DefaultFlusher default_flusher( 3, std::cout );
    BlockFlusher block_flusher( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );
    executor.detach ( &block_flusher );
    executor.read_and_execute();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
    );
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
