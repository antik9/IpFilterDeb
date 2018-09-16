#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "bulk.hpp"

int
main(int args, char *argv[])
{
    size_t bulk_size;
    if ( args != 2 )
    {
        std::cout << "Run command as `bulk <bulk_size>`" << std::endl;
        exit(1);
    }
    else
    {
        bulk_size = atoi(argv[1]);
        if ( bulk_size == 0 )
        {
            std::cout << "Provide a positive number <bulk_size>" << std::endl;
            exit(1);
        }
    }

    std::mutex main_mutex;
    std::mutex pipe_mutex;
    std::mutex cout_mutex;
    std::mutex fwrite_mutex;
    std::mutex pipe_off;
    std::mutex write_off;

    pipe_off.lock();
    write_off.lock();
    LockingQueue<std::string>                   pipe_queue      ( pipe_mutex );
    LockingQueue<StatsAccumulatorWithContent>   main_queue      ( main_mutex );
    LockingQueue<StatsAccumulatorWithContent>   cout_queue      ( cout_mutex );
    LockingQueue<StatsAccumulatorWithContent>   fwrite_queue    ( fwrite_mutex );

    StatsAccumulator main_stats ( "commands", "blocks" );
    main_stats.set_prefix ( "Main thread" );
    StatsAccumulator stream_stats ( "commands", "blocks" );
    stream_stats.set_prefix ( "Cout thread" );
    StatsAccumulator file_1_stats ( "commands", "blocks" );
    file_1_stats.set_prefix ( "File 1 thread" );
    StatsAccumulator file_2_stats ( "commands", "blocks" );
    file_2_stats.set_prefix ( "File 2 thread" );

    StreamWriter main_writer;
    main_writer.set_stats_accumulator ( &main_stats );
    StreamWriter stream_writer;
    stream_writer.set_ostream( &std::cout );
    stream_writer.set_stats_accumulator ( &stream_stats );
    stream_writer.set_stats_accumulator ( &stream_stats );
    FileWriter   file_writer_1;
    file_writer_1.set_stats_accumulator ( &file_1_stats );
    FileWriter   file_writer_2;
    file_writer_2.set_stats_accumulator ( &file_2_stats );
    
    Sorter sorter ( bulk_size );

    TeePipe tee;
    tee.add_output  ( &main_queue );
    tee.add_output  ( &cout_queue );
    tee.add_output  ( &fwrite_queue );
    tee.add_handler ( &sorter );

    std::thread pipe_in ( bulkmt::read_to_pipe,
        std::ref ( tee ), std::ref ( pipe_queue ), std::ref ( pipe_off ) );
    std::thread main_log ( bulkmt::write,
        std::ref( main_writer ), std::ref( main_queue ), std::ref ( write_off ) );
    std::thread cout_log ( bulkmt::write,
        std::ref( stream_writer ), std::ref( cout_queue ), std::ref ( write_off ) );
    std::thread file_1_log ( bulkmt::write,
        std::ref( file_writer_1 ), std::ref( fwrite_queue ), std::ref ( write_off ) );
    std::thread file_2_log ( bulkmt::write,
        std::ref( file_writer_2 ), std::ref( fwrite_queue ), std::ref ( write_off ) );

	std::string next;
	size_t lines = 0;

    while ( true )
    {
        std::getline( std::cin, next );
        if ( std::cin.good ( ) )
        {
            ++lines;
            pipe_queue.push     ( next );
        }
        else if ( std::cin.eof() )
        {
            break;
        }
        else                // Unknown failure
        {
            std::cout << "Unknown ERROR" << std::endl;
            break;
        }
    }
    
    main_stats.accumulate ( "lines", lines );

    pipe_off.unlock     ( );
    pipe_in.join        ( );
    write_off.unlock    ( );
    cout_log.join       ( );
    file_1_log.join     ( );
    file_2_log.join     ( );
    main_log.join       ( );
    
    return 0;
}
