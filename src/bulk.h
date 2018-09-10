#pragma once
#include <iostream>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <vector>

const std::string NEW_BLOCK_INIT    = "{";
const std::string NEW_BLOCK_CLOSE   = "}";
const std::string FLUSH_INIT        = "bulk: ";

struct
StatsAccumulator
{
    StatsAccumulator();
    void
    incr_blocks();
    void
    incr_commands();
	void
	print_stats();

private:
    size_t blocks;
    size_t commands;
};

struct
ConditionFlusher
{
    using stack         = std::stack<std::string>;
    using container     = std::vector<std::string>;
    ConditionFlusher    (int buffer_size, bool is_block);
    
    void
    flush ();

    void
    print_stats ();

    void
    receive (std::string message);

    void
    set_ostream (std::ostream*);
    
    void 
    set_filename_setter ( std::string (*__set_filename)() );

private:
    int                 buffer_size;
    bool                is_block;
    container           commands;
    stack               block_separators;
    std::ostream*       out;
    std::string         (*__set_filename) ();
	std::string			filename;
    StatsAccumulator    accumulator;
};

void
read_and_flush(ConditionFlusher& flusher, std::queue<std::string>& messages, 
                std::mutex& rw_mutex, std::mutex& off);

std::string
set_filename();
