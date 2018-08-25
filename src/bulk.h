#pragma once
#include <iostream>
#include <stack>
#include <string>
#include <vector>

const std::string NEW_BLOCK_INIT    = "{";
const std::string NEW_BLOCK_CLOSE   = "}";
const std::string FLUSH_INIT        = "bulk: ";

class Flusher;
class BlockFlusher;
class DefaultFlusher;

class BulkExecutor
{
public:
    using stack         = std::stack<std::string>;

    BulkExecutor(std::istream& in) : 
        in(in) {};

    ~BulkExecutor()     = default;

    void
    attach ( BlockFlusher* flusher );

    void
    attach ( DefaultFlusher* flusher );
    
    void
    read_and_execute (); 

private:
    Flusher*            block_flusher;
    Flusher*            default_flusher;
    std::istream&       in;
    stack               separators;
};


class Flusher
{
public:
    using container     = std::vector<std::string>;
    
    Flusher ( std::ostream* out_s ) :
        out(out_s) {};

    virtual void 
    update (std::string);
    
    virtual void 
    update ();

    virtual void 
    flush ();
    
    virtual void
    set_filename ();

protected:
    container       commands;
    std::string     filename;
    std::ostream*   out;
};


class DefaultFlusher : public Flusher
{
public:
    DefaultFlusher ( BulkExecutor& executor, size_t buffer_size, std::ostream& out_s ) :
        buffer_size(buffer_size), Flusher(&out_s)
    {
        executor.attach(this);
    }

    void
    update (std::string);

private:
    size_t          buffer_size;
};


class BlockFlusher : public Flusher
{
public:
    BlockFlusher ( BulkExecutor& executor, std::ostream& out_s ) :
        Flusher(&out_s)
    {
        executor.attach(this);
    }
};
