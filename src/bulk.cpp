#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

#include "bulk.h"

/* 
 * Time offset from localtime to GMT
 */
const int 
get_time_offset()
{
    time_t local_epoch  = time(NULL);
    
    tm* gm_time         = gmtime(&local_epoch);
    time_t gm_epoch     = mktime(gm_time);
    
    return static_cast<int>(local_epoch - gm_epoch);
}

static const int timezone_diff_seconds = get_time_offset();
   

/*
 * Definition of BulkExecutor methods
 */
void
BulkExecutor::attach ( Flusher* flusher_ptr )
{
   flushers_ptrs.emplace_back(flusher_ptr);
}

void
BulkExecutor::detach ( Flusher* flusher_ptr )
{
    this->flushers_ptrs.erase(
        std::remove_if(
            this->flushers_ptrs.begin(),
            this->flushers_ptrs.end(), 
            [=] (Flusher* ptr) { return ptr == flusher_ptr; }
        )
    );
}

void
BulkExecutor::read_and_execute()
{
    std::string next;
    while ( true )
    {
        std::getline(in, next);
        bool success = true;
        if ( in.good() ) 
        {
            for ( auto& flusher_ptr: flushers_ptrs )
            {
                if ( not ( success = flusher_ptr->update(next) ) )
                {
                    std::cout << "ERROR: Incorrect place of end of sequence" << std::endl;
                    break;
                }
            }
        }
        else if ( in.eof() )
        {
            for ( auto& flusher_ptr: flushers_ptrs )
            {
                flusher_ptr->flush();
            }
            break;
        }
        else                // Unknown failure
        {
            std::cout << "Unknown ERROR" << std::endl;
            break;
        }
        if ( not success )
        {
            break;
        }
    }
}

/*
 * Definiton of Flusher methods
 */
void 
Flusher::flush ()
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
        output_sstream << command << ( commands_left-- > 1 ? ", " : "\n" );
    }

    auto output = output_sstream.str();

    if ( output.compare(FLUSH_INIT) )
    {
        *out << output;
        std::ofstream logfile;
        logfile.open (filename);
        logfile << output; 
        logfile.close();
    }

    commands.clear();
}

void
Flusher::set_filename ()
{
    std::stringstream filename_sstream;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    duration += std::chrono::seconds(timezone_diff_seconds);

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);

    filename_sstream << "bulk" << seconds.count() << ".log";
    this->filename = filename_sstream.str();
}

/*
 * Definition of methods for DefaultFlusher
 */
bool
DefaultFlusher::update (std::string command)
{
    if ( command == NEW_BLOCK_INIT )
    {
        flush();
        block_separators.push(NEW_BLOCK_INIT);
    }
    else if ( command == NEW_BLOCK_CLOSE && not block_separators.empty() )
    {
        block_separators.pop();
    }
    else if ( command == NEW_BLOCK_CLOSE )
    {
        return false;
    }
    else if ( block_separators.empty() )
    {
        if ( commands.empty() )
        {
            set_filename();
        }
    
        commands.emplace_back(command);
    
        if ( commands.size() == buffer_size )
        {
            flush();
        }
    }
    return true;
} 

    
/*
 * Definition of methods for BlockFlusher
 */
bool
BlockFlusher::update ( std::string command )
{
    if ( command == NEW_BLOCK_INIT )
    {
        block_separators.push(NEW_BLOCK_INIT);
    }
    else if ( command == NEW_BLOCK_CLOSE && not block_separators.empty() )
    {
        block_separators.pop();
        if ( block_separators.empty() )
        {
            flush();
        }
    }
    else if ( command == NEW_BLOCK_CLOSE )
    {
        return false;
    }
    else if ( not block_separators.empty() )
    {
        if ( commands.empty() )
        {
            set_filename();
        }
        commands.emplace_back(command);
    }
    return true;
}

