#include <algorithm>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include "bulk.h"

/*
 * Definition of StatsAccumulator methods
 */
StatsAccumulator::StatsAccumulator() : blocks(0), commands(0) {}
void StatsAccumulator::incr_blocks() 	{ ++blocks; }
void StatsAccumulator::incr_commands()	{ ++commands; }

void 
StatsAccumulator::print_stats()	
{ 
	std::cout 
	    << blocks << " blocks, "
	    << commands << " commands"
	    << std::endl; 
}

/*
 * Definition of ConditionFlusher methods
 */
ConditionFlusher::ConditionFlusher ( int buffer_size, bool is_block ) : 
    buffer_size(buffer_size), is_block(is_block), out(nullptr),  __set_filename(nullptr) {}

void ConditionFlusher::print_stats () { accumulator.print_stats(); }
void ConditionFlusher::set_ostream ( std::ostream* out ) { this->out = out; }
void 
ConditionFlusher::set_filename_setter ( std::string (*__set_filename)() ) 
{ 
    this->__set_filename = __set_filename; 
}

void 
ConditionFlusher::flush()
{
    std::stringstream output_sstream;
    int commands_left = commands.size();

    /* Flush if only EOF is correct regarding to block_separators */
    if ( not block_separators.empty() )
    {
        return;
    }
    
    output_sstream << FLUSH_INIT;
    for ( auto command: commands )
    {
        accumulator.incr_commands();
        output_sstream << command << ( commands_left-- > 1 ? ", " : "\n" );
    }

    auto output = output_sstream.str();

    if ( output.compare(FLUSH_INIT) )
    {
        accumulator.incr_blocks();
        if ( out )
        {
            *out << output;
        }
        if ( __set_filename and filename.size() ) 
        {
            std::ofstream logfile;
            logfile.open (filename);
            logfile << output; 
            logfile.close();
            filename.clear();
        }
    }

    commands.clear();
}

void
ConditionFlusher::receive(std::string message) 
{
    if ( message == NEW_BLOCK_INIT )
    {
        if ( block_separators.empty() )
        {
            if ( buffer_size )
            {
                flush();
            }
            if ( is_block and __set_filename )
            {
                filename = __set_filename();
            }
        }
        block_separators.push(NEW_BLOCK_INIT);
    }
    else if ( message == NEW_BLOCK_CLOSE )
    {
        if ( block_separators.empty() )
        {
            throw(std::logic_error("ERROR: Incorrect place of end of sequence"));
        }
        block_separators.pop();
        if ( is_block and block_separators.empty() )
        {
            flush();
        }
    }
    else if ( block_separators.empty() )
    {
        if ( buffer_size )
        {
            if ( commands.empty() and __set_filename )
            {
                filename = __set_filename();
            }
    
            commands.emplace_back(message);
    
            if ( commands.size() == buffer_size )
            {
                flush();
            }
        }
    }
    else if ( is_block )
    {
        commands.emplace_back(message);
    }
}


/*
 * Bulkmt module functions
 */
void
read_and_flush(ConditionFlusher& flusher, std::queue<std::string>& messages, 
                std::mutex& rw_mutex, std::mutex& off)
{
    while ( true )
    {
        if ( rw_mutex.try_lock() )
        {
            std::string message;
            /* Try read from queue if there any messages */
            if ( not messages.empty() )
            {
                message = messages.front();
                messages.pop();
            }
            rw_mutex.unlock();

            /* Perform operations according to condition */
            if ( not message.empty() )
            {
                try
                {
                    flusher.receive(message);
                }
                catch (std::logic_error err)
                {
                    std::cerr << err.what() << std::endl;
                    return;
                }
            }
        }

        /* If `off` mutex is unlocked the thread should exit */
        if ( off.try_lock() )
        {
            off.unlock();
            if ( messages.empty() )
            {
                std::lock_guard<std::mutex> lock(rw_mutex);
                flusher.flush();
                flusher.print_stats();
                return;
            }
        }
    }
}

std::string
set_filename()
{
    std::stringstream filename_sstream;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);

    filename_sstream << "bulk" << microseconds.count() << ".log";
    return filename_sstream.str();
}
