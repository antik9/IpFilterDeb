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

    ConditionFlusher flusher (2, true);
    flusher.set_ostream(&std::cout);
    flusher.receive("cat");
    flusher.receive("dog");
    flusher.receive("tac");
    flusher.flush();
    flusher.print_stats();
    
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
            "2 blocks, 3 commands\n"
    );
}

TEST_F(TestBulk, simple_plus_bulk)
{
    testing::internal::CaptureStdout();

    ConditionFlusher flusher (2, true);
    flusher.set_ostream(&std::cout);
    flusher.receive("cat");
    flusher.receive("dog");
    flusher.receive(NEW_BLOCK_INIT);
    flusher.receive("tac");
    flusher.receive("rm -rf");
    flusher.receive(NEW_BLOCK_CLOSE);
    flusher.flush();
    flusher.print_stats();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, rm -rf\n"
            "2 blocks, 4 commands\n"
    );
}
 
TEST_F(TestBulk, idle_flusher)
{
    testing::internal::CaptureStdout();

    ConditionFlusher flusher (2, true);
    flusher.receive("cat");
    flusher.receive("dog");
    flusher.receive(NEW_BLOCK_INIT);
    flusher.receive("tac");
    flusher.flush();
    flusher.print_stats();

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
        "1 blocks, 2 commands\n"
    );
}
 

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
