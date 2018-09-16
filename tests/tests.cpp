#include <gtest/gtest.h>
#include <cstdio>
#include <dirent.h>
#include <sstream>
#include <string>
#include <thread>
#include "../src/bulk.hpp"


class TestBulk : public ::testing::Test
{

protected:
    std::mutex pipe_mutex;
    std::mutex pipe_off;
    std::mutex write_off;
    std::mutex test_mutex;
    StatsAccumulator test_stats = StatsAccumulator ( "commands", "blocks" );
    StreamWriter test_writer;
    Sorter sorter               = Sorter ( 2 );
    TeePipe tee;
    std::thread pipe_in;
    std::thread test_log;
    LockingQueue<std::string> 
        pipe_queue = LockingQueue<std::string>                  ( pipe_mutex );
    LockingQueue<StatsAccumulatorWithContent>   
        test_queue = LockingQueue<StatsAccumulatorWithContent>  ( test_mutex );
    
    void
    SetUp ( )
    {
        pipe_off.lock();
        write_off.lock();
        test_stats.set_prefix ( "Test thread" );
        test_writer.set_stats_accumulator ( &test_stats );
        tee.add_output  ( &test_queue );
        tee.add_handler ( &sorter );
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
    
    test_writer.set_ostream( &std::cout );
    pipe_in     = std::thread ( bulkmt::read_to_pipe,
                                std::ref ( tee ), std::ref ( pipe_queue ),
                                std::ref ( pipe_off ) );
    test_log    = std::thread ( bulkmt::write,
                                std::ref( test_writer ), std::ref( test_queue ), 
                                std::ref ( write_off ) );
    pipe_queue.push ( "cat" );
    pipe_queue.push ( "dog" );
    pipe_queue.push ( "tac" );
    pipe_off.unlock ( );
    pipe_in.join    ( );
    write_off.unlock( );
    test_log.join   ( );
    test_stats.print_stats ( );

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac\n"
            "Test thread: 2 blocks 3 commands \n"
    );
}

TEST_F(TestBulk, simple_plus_bulk)
{
    testing::internal::CaptureStdout();

    test_writer.set_ostream( &std::cout );
    pipe_in     = std::thread ( bulkmt::read_to_pipe,
                                std::ref ( tee ), std::ref ( pipe_queue ),
                                std::ref ( pipe_off ) );
    test_log    = std::thread ( bulkmt::write,
                                std::ref( test_writer ), std::ref( test_queue ), 
                                std::ref ( write_off ) );
    pipe_queue.push ( "cat" );
    pipe_queue.push ( "dog" );
    pipe_queue.push ( NEW_BLOCK_INIT );
    pipe_queue.push ( "tac" );
    pipe_queue.push ( "rm -rf" );
    pipe_queue.push ( NEW_BLOCK_CLOSE );
    pipe_off.unlock ( );
    pipe_in.join    ( );
    write_off.unlock( );
    test_log.join   ( );
    test_stats.print_stats ( );

    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
            "bulk: cat, dog\n"
            "bulk: tac, rm -rf\n"
            "Test thread: 2 blocks 4 commands \n"
    );
}
 
TEST_F(TestBulk, idle_flusher)
{
    testing::internal::CaptureStdout();

    pipe_in     = std::thread ( bulkmt::read_to_pipe,
                                std::ref ( tee ), std::ref ( pipe_queue ),
                                std::ref ( pipe_off ) );
    test_log    = std::thread ( bulkmt::write,
                                std::ref( test_writer ), std::ref( test_queue ), 
                                std::ref ( write_off ) );
    pipe_queue.push("cat");
    pipe_queue.push("dog");
    pipe_queue.push(NEW_BLOCK_INIT);
    pipe_queue.push("tac");
    pipe_off.unlock ( );
    pipe_in.join    ( );
    write_off.unlock( );
    test_log.join   ( );
    test_stats.print_stats ( );
    
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output,
        "Test thread: 1 blocks 2 commands \n"
    );
}
 

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
