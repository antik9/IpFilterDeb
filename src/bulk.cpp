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
BulkExecutor::attach ( DefaultFlusher* flusher )
{
    default_flusher = static_cast<Flusher*>(flusher);
}

void
BulkExecutor::attach ( BlockFlusher* flusher )
{
    block_flusher = static_cast<Flusher*>(flusher);
}

void
BulkExecutor::read_and_execute()
{
    std::string next;
    while ( true )
    {
        std::getline(in, next);
        if ( in.good() && separators.empty() )
        {
            if ( next == NEW_BLOCK_INIT )
            {
                default_flusher->update();
                separators.push(next);
            }
            else if ( next == NEW_BLOCK_CLOSE )
            {
                std::cout << "ERROR: Incorrect place of end of sequence" << std::endl;
                break;
            }
            else 
            {
                default_flusher->update(next);
            }
        }
        else if ( in.good() && !separators.empty() )
        {
            if ( next == NEW_BLOCK_INIT )
            {
                separators.push(next);
            }
            else if ( next == NEW_BLOCK_CLOSE )
            {
                separators.pop();
                if ( separators.empty() )
                {
                    block_flusher->update();
                }
            }
            else
            {
                block_flusher->update(next);
            }
        }
        else
        {   
            default_flusher->update();
            break;
        }
    }
}

/*
 * Definiton of Flusher methods
 */
void
Flusher::update ( std::string command )
{
    if ( commands.empty() )
    {
        set_filename();
    }
    commands.emplace_back(command);
}

void
Flusher::update ()
{
    flush();
}

void 
Flusher::flush ()
{
    std::stringstream output_sstream;
    int commands_left = commands.size();
    
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
	
	using Days = std::chrono::duration<
	                int, std::ratio_multiply<std::chrono::hours::period, std::ratio<24>
	                >::type>;

	Days days = std::chrono::duration_cast<Days>(duration);
    duration -= days;
	
	auto hours = std::chrono::duration_cast<std::chrono::hours>(duration);
    duration -= hours;
	
	auto minutes = std::chrono::duration_cast<std::chrono::minutes>(duration);
    duration -= minutes;
	
	auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;
	
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;
	
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    duration -= microseconds;

    filename_sstream << std::setfill('0') << std::setw(2) << hours.count();
    filename_sstream << std::setfill('0') << std::setw(2) << minutes.count();
    filename_sstream << std::setfill('0') << std::setw(2) << seconds.count();
    filename_sstream << std::setfill('0') << std::setw(3) 
        << milliseconds.count() << microseconds.count() / 100 << ".log";
    
    this->filename = filename_sstream.str();
}

/*
 * Definition of methods for DefaultFlusher
 */
void
DefaultFlusher::update (std::string command)
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
