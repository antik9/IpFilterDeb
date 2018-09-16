#pragma once
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <vector>

const std::string NEW_BLOCK_INIT    = "{";
const std::string NEW_BLOCK_CLOSE   = "}";
const std::string FLUSH_INIT        = "bulk: ";

/* Content to pass information further */
struct
Content
{
    std::string filename;
    std::string message;
};

/* Stats Accumulators */
struct
StatsAccumulator
{
    template < typename ...Keys >
    StatsAccumulator ( Keys... keys )
    {
        this->__stats = std::map<std::string, size_t> { 
                            std::pair<std::string, size_t>(keys, 0)... };
    }

    void
    accumulate          ( std::string key, size_t value );
    size_t
    get                 ( std::string key );
    std::vector<std::string>
    get_keys            ( );
	virtual void
	print_stats         ( );
	void
	set_prefix          ( std::string prefix );

protected:
    std::string                     prefix;
    std::map<std::string, size_t>   __stats;
};


struct
StatsAccumulatorWithContent: StatsAccumulator
{
    template < typename ...Keys >
    StatsAccumulatorWithContent ( Keys... keys ) :
        StatsAccumulator ( keys... ) { };

    Content&
    get_content ( );
    void
    set_content ( Content& content );

private:
    Content     content;
};

/* Queue Wrapper */
template < typename T >
struct
LockingQueue
{
    LockingQueue ( std::mutex& rw_mutex ) : rw_mutex ( rw_mutex )  { };

    bool
    empty ( ) { return messages.empty(); }

    void
    lock ( ) { rw_mutex.lock(); } 

    T
    pop ( )
    {
        T front = messages.front();
        messages.pop();
        return front;
    }

    void
    push ( T message )
    {
        std::lock_guard<std::mutex> _lock ( rw_mutex );
        messages.push ( message );
    }

    void
    unlock ( ) { rw_mutex.unlock(); } 

private:
    std::queue<T>   messages;
    std::mutex&     rw_mutex;
};

/* Handler to differentiate buffered and block messages */
struct
Sorter
{
    using stack         = std::stack <std::string>;
    using container     = std::vector<std::string>;

    Sorter      ( int buffer_size );
    bool
    has_output  ( );
    StatsAccumulatorWithContent
    get_output  ( );
    void
    receive     ( std::string message );

private:
    bool                        __has_output;
    int                         buffer_size;
    StatsAccumulatorWithContent output;
    container                   commands;
    stack                       block_separators;
    std::string                 filename;

    void
    set_output  ( );
};

/* Pipe with multi output */
struct
TeePipe
{
    using ContentQueue   = LockingQueue<StatsAccumulatorWithContent>;

    TeePipe         ( );
    void
    add_handler     ( Sorter* sorter_ptr );
    void
    add_output      ( ContentQueue* messages );
    void
    flush           ( );
    void
    in              ( std::string message );
private:
    std::vector<ContentQueue*>  outputs;
    Sorter*                     sorter_ptr;
};

/* Writers */
struct
WriterWithAccumulator
{
    WriterWithAccumulator   ( );
    virtual
    ~WriterWithAccumulator  ( );

    virtual void
    print_stats             ( );
    virtual void
    set_stats_accumulator   ( StatsAccumulator* accumulator );
    virtual void
    write                   ( StatsAccumulatorWithContent&& accumulator )  = 0;
protected:
    StatsAccumulator*       accumulator;
};


struct
FileWriter : public WriterWithAccumulator
{
    FileWriter      ( );
    void
    set_filename    ( std::string filename );
    void
    write           ( StatsAccumulatorWithContent&& accumulator );
private:
    std::string     filename;
};

struct
StreamWriter : public WriterWithAccumulator
{
    StreamWriter    ( );
    void
    set_ostream     ( std::ostream* out );
    void
    write           ( StatsAccumulatorWithContent&& accumulator );
private:
    std::ostream*   out;
};

namespace bulkmt
{
    void
    read_to_pipe    ( TeePipe& tee, LockingQueue<std::string>& messages, std::mutex& off );

    void
    write           ( WriterWithAccumulator& writer, 
                        LockingQueue<StatsAccumulatorWithContent>& messages,
                        std::mutex& off );

    std::string
    set_filename    ( );
}
