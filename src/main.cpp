#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

#include "bulk.h"

int
main(int args, char *argv[])
{
    std::mutex rw_mutex;
    std::mutex off;
    off.lock();
    std::queue<std::string> logfile_block_q;
    std::queue<std::string> logfile_buffer_q;
    std::queue<std::string> logfile_cout_q;

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
    ConditionFlusher block_to_file_flusher      (0, true);
    block_to_file_flusher.set_filename_setter   (&set_filename);
    std::thread write_block_to_file(read_and_flush,
            std::ref(block_to_file_flusher), std::ref(logfile_block_q),
            std::ref(rw_mutex), std::ref(off));

    ConditionFlusher buffer_to_file_flusher     (bulk_size, false);
    buffer_to_file_flusher.set_filename_setter  (&set_filename);
    std::thread write_buffer_to_file(read_and_flush,
            std::ref(buffer_to_file_flusher), std::ref(logfile_buffer_q),
            std::ref(rw_mutex), std::ref(off));

    ConditionFlusher all_to_cout_flusher        (bulk_size, true);
    all_to_cout_flusher.set_ostream             (&std::cout);
    std::thread write_all_to_cout(read_and_flush,
            std::ref(all_to_cout_flusher), std::ref(logfile_cout_q),
            std::ref(rw_mutex), std::ref(off));

    ConditionFlusher idle_flusher (bulk_size, true);

	std::string next;
	size_t lines = 0;
    while ( true )
    {
        std::getline(std::cin, next);
        if ( std::cin.good() )
        {
            ++lines;
            rw_mutex.lock();
            logfile_block_q.push(next);
            logfile_buffer_q.push(next);
            logfile_cout_q.push(next);
            idle_flusher.receive(next);
            rw_mutex.unlock();
        }
        else if ( std::cin.eof() )
        {
            rw_mutex.lock();
            std::cout << lines << " lines, ";
            idle_flusher.flush();
            idle_flusher.print_stats();
            rw_mutex.unlock();
            break;
        }
        else                // Unknown failure
        {
            std::cout << "Unknown ERROR" << std::endl;
            break;
        }
    }

    off.unlock();
    write_block_to_file.join();
    write_buffer_to_file.join();
    write_all_to_cout.join();
    return 0;
}
