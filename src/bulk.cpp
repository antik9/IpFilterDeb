#include <algorithm>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <thread>

#include "bulk.hpp"

/*
 * Definition of StatsAccumulator methods
 */

void
StatsAccumulator::accumulate ( const std::string& key, size_t value )
{
    if ( __stats.find(key) == __stats.end() )
    {
        __stats[key] = 0;
    }
    __stats[key] = __stats[key] + value;
}

size_t
StatsAccumulator::get ( const std::string& key )
{
    if ( __stats.find ( key ) != __stats.end ( ) )
    {
        return __stats[key];
    }
    return 0;
}

std::vector<std::string>
StatsAccumulator::get_keys ( )
{
    std::vector<std::string> keys;
    keys.reserve ( __stats.size ( ) );
    for ( auto& key_value: __stats )
    {
        keys.push_back ( key_value.first );
    }
    return keys;
}

void
StatsAccumulator::print_stats ( )
{
    if ( not prefix.empty() )
    {
        std::cout << prefix << ": ";
    }
    for ( auto& key_value: __stats )
    {
        std::cout << key_value.second << " " << key_value.first << " ";
    }
    std::cout << std::endl;
}

void
StatsAccumulator::set_prefix ( const std::string& prefix ) { this->prefix = prefix; }

/*
 * Definition of StatsAccumulatorWithContent methods
 */
Content& StatsAccumulatorWithContent::get_content ( ) { return content; }
void     StatsAccumulatorWithContent::set_content ( const Content& content ) { this->content = content; }

/*
 * Definition of Sorter methods
 */
Sorter::Sorter ( int buffer_size ) : __has_output ( false ), buffer_size ( buffer_size ) { }

bool Sorter::has_output ( ) { return __has_output; }

StatsAccumulatorWithContent
Sorter::get_output ( )
{
    if ( __has_output )
    {
        __has_output = false;
    }
    else
    {
        set_output ( );
    }
    return output;
}

void
Sorter::receive ( const std::string& message )
{
    if ( message == NEW_BLOCK_INIT )
    {
        if ( block_separators.empty ( ) )
        {
            set_output ( );
            filename = bulkmt::set_filename ( );
        }

        block_separators.push(NEW_BLOCK_INIT);
    }
    else if ( message == NEW_BLOCK_CLOSE )
    {
        if ( block_separators.empty () )
        {
            throw( std::logic_error( "ERROR: Incorrect place of end of sequence" ) );
        }

        block_separators.pop ( );

        if ( block_separators.empty ( ) )
        {
            set_output ( );
        }
    }
    else if ( block_separators.empty() )
    {
        if ( commands.empty ( ) )
        {
            filename = bulkmt::set_filename();
        }

        commands.emplace_back ( message );

        if ( commands.size ( ) == buffer_size )
        {
            set_output ( );
        }
    }
    else
    {
        commands.emplace_back ( message );
    }
}

void
Sorter::set_output ( )
{
    if ( commands.empty ( ) )
    {
        __has_output = false;
    }
    else
    {
        std::stringstream   output_sstream;
        Content             content;
        output              = StatsAccumulatorWithContent {};
        int commands_left   = commands.size();

        /* Flush if only EOF is correct regarding to block_separators */
        if ( not block_separators.empty ( ) )
        {
            __has_output = false;
            return;
        }

        output_sstream << FLUSH_INIT;
        output.accumulate ( "commands", commands_left );

        for ( auto command: commands )
        {
            output_sstream << command << ( commands_left-- > 1 ? ", " : "\n" );
        }

        content.filename    = filename;
        content.message     = output_sstream.str ( );
        output.set_content ( content );

        if ( content.message.compare ( FLUSH_INIT ) )
        {
            output.accumulate ( "blocks", 1 );
            __has_output = true;
        }
    }

    commands.clear ( );
    filename.clear ( );
}

/*
 * Definition of TeePipe methods
 */
TeePipe::TeePipe ( ) : sorter_ptr ( nullptr ) { }

void TeePipe::add_handler ( Sorter* sorter_ptr ) { this->sorter_ptr = sorter_ptr; }

void TeePipe::add_output ( TeePipe::ContentQueue* messages ) { outputs.emplace_back ( messages ); }

void
TeePipe::flush ( )
{
    auto content = sorter_ptr->get_output ( );
    if ( not content.get_content ( ).message.empty ( ) )
    {
        for ( auto& output_queue: outputs )
        {
            output_queue->push ( content );
        }
    }
}

void
TeePipe::in ( const std::string& message )
{
    if ( sorter_ptr != nullptr )
    {
        if ( sorter_ptr->has_output ( ) )
        {
            flush ( );
        }
        sorter_ptr->receive ( message );
    }
}

/*
 * Definition of WriterWithAccumulator methods
 */
WriterWithAccumulator::WriterWithAccumulator ( ) : accumulator ( nullptr ) { }

WriterWithAccumulator::~WriterWithAccumulator ( ) { print_stats(); }

void
WriterWithAccumulator::print_stats ( )
{
    if ( accumulator != nullptr )
    {
        accumulator->print_stats();
    }
}

void
WriterWithAccumulator::set_stats_accumulator ( StatsAccumulator* accumulator )
{
    this->accumulator = accumulator;
}

/*
 * Definition of FileWriter methods
 */
FileWriter::FileWriter ( ) : WriterWithAccumulator ( ) { }

void FileWriter::set_filename ( const std::string& filename ) { this->filename = filename; }

void
FileWriter::write ( StatsAccumulatorWithContent&& accumulator )
{
    if ( this->accumulator )
    {
        for ( auto key: this->accumulator->get_keys ( ) )
        {
            this->accumulator->accumulate ( key, accumulator.get ( key ) );
        }
    }

    auto content = accumulator.get_content ( );
    if ( not content.filename.empty() )
    {
        set_filename    ( content.filename );
        std::ofstream   logfile;
        logfile.open    ( filename );
        logfile         << content.message;
        logfile.close();
        filename.clear();
    }
}

/*
 * Definition of StreamWriter methods
 */
StreamWriter::StreamWriter ( ) : WriterWithAccumulator ( ), out ( nullptr ) { }

void
StreamWriter::set_ostream ( std::ostream* out ) { this->out = out; }

void
StreamWriter::write ( StatsAccumulatorWithContent&& accumulator )
{
    if ( this->accumulator )
    {
        for ( auto key: this->accumulator->get_keys ( ) )
        {
            this->accumulator->accumulate ( key, accumulator.get ( key ) );
        }
    }

    auto content = accumulator.get_content();
    if ( out != nullptr )
    {
        std::cout << content.message;
    }
}

/*
 * Connector methods
 */
Connector::Connector        ( size_t bulk_size ) : sorter ( Sorter ( bulk_size ) )
{
    stream_writer.set_ostream   ( &std::cout );
    tee.add_output              ( &cout_queue );
    tee.add_output              ( &fwrite_queue );
    tee.add_handler             ( &sorter );
}

void
Connector::connect          ( )
{
    pipe_off.lock();
    write_off.lock();

    pipe_in     = std::thread ( bulkmt::read_to_pipe,
                                std::ref ( tee ), std::ref ( pipe_queue ),
                                std::ref ( pipe_off ) );
    cout_log    = std::thread ( bulkmt::write,
                                std::ref ( stream_writer ), std::ref( cout_queue ),
                                std::ref ( write_off ) );
    file_1_log  = std::thread ( bulkmt::write,
                                std::ref ( file_writer_1 ), std::ref( fwrite_queue ),
                                std::ref ( write_off ) );
    file_2_log  = std::thread ( bulkmt::write,
                                std::ref ( file_writer_2 ), std::ref( fwrite_queue ),
                                std::ref ( write_off ) );
}

void
Connector::disconnect   ( )
{
    fraction.clear      ( );
    pipe_off.unlock     ( );
    pipe_in.join        ( );
    write_off.unlock    ( );
    cout_log.join       ( );
    file_1_log.join     ( );
    file_2_log.join     ( );
}


void
Connector::receive      ( const std::string& message )
{
    ssize_t idx = -1, next_idx;

    if ( not fraction.empty ( ) )
    {
        idx = message.find ( '\n' );
        if ( idx == std::string::npos )
        {
            fraction += message;
            return;
        }
        else
        {
            fraction += message.substr ( 0, idx );
            pipe_queue.push ( fraction );
            fraction.clear ( );
        }
    }

    while ( idx + 1 < message.size ( )
            and ( next_idx = message.find ( '\n', idx + 1 ) ) != std::string::npos )
    {
        pipe_queue.push     ( message.substr ( idx + 1, next_idx - idx - 1 ) );
        idx = next_idx;
    }

    if ( idx + 1 != message.size ( ) )
    {
        fraction = message.substr ( idx + 1 );
    }
}

/*
 * Bulkmt module functions
 */

namespace bulkmt
{
    void
    read_to_pipe ( TeePipe& tee, LockingQueue<std::string>& messages, std::mutex& off )
    {
        while ( true )
        {
            if ( not messages.empty ( ) )
            {
                try
                {
                    messages.lock ( );
                    if ( not messages.empty ( ) )
                    {
                        tee.in ( messages.pop ( ) );
                    }
                    messages.unlock ( );
                }
                catch ( std::logic_error err )
                {
                    std::cerr << err.what ( ) << std::endl;
                    return;
                }
            }
            else
            {
                std::this_thread::sleep_for( std::chrono::milliseconds ( 1 ) );
            }

            /* If `off` mutex is unlocked the thread should exit */
            if ( messages.empty ( ) and off.try_lock ( ) )
            {
                off.unlock ( );
                tee.flush ( );
                return;
            }
        }
    }


    void
    write ( WriterWithAccumulator& writer,
            LockingQueue<StatsAccumulatorWithContent>& messages, std::mutex& off )
    {
        while ( true )
        {
            if ( not messages.empty ( ) )
            {
                messages.lock ( );
                if ( not messages.empty ( ) )
                {
                    writer.write ( messages.pop ( ) );
                }
                messages.unlock ( );
            }
            else
            {
                std::this_thread::sleep_for( std::chrono::milliseconds ( 1 ) );
            }

            /* If `off` mutex is unlocked the thread should exit */
            if ( messages.empty ( ) and off.try_lock ( ) )
            {
                off.unlock ( );
                if ( messages.empty ( ) )
                {
                    return;
                }
            }
        }
    }

    std::string
    set_filename ( )
    {
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();

        auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
        return "bulk" + std::to_string ( microseconds.count ( ) ) + ".log";
    }
}
