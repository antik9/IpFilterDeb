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
        std::string log_pattern = ".log";

        while ( (file_ = readdir (test_dir)) != NULL ) 
        {
            std::string filename = file_->d_name;
            if ( filename.size() == 14 and 
                    log_pattern.compare(filename.substr(filename.size() - 4)) == 0 )
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 2, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 3, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 10, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 5, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 2, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
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
    
    BulkExecutor executor(input);
    DefaultFlusher default_flusher(executor, 2, std::cout);
    BlockFlusher block_flusher(executor, std::cout);
    executor.read_and_execute();
    
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, open, close\n"
    );
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
