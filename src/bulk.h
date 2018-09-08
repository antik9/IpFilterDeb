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

    BulkExecutor(std::istream& in) : 
        in(in) {};

    ~BulkExecutor()     = default;

    void
    attach ( Flusher* flusher_ptr);

    void
    detach ( Flusher* flusher_ptr);

    void
    read_and_execute (); 

    std::vector<Flusher*>   flushers_ptrs;
private:
    std::istream&           in;
};


class Flusher
{
public:
    using stack         = std::stack<std::string>;
    using container     = std::vector<std::string>;
    
    Flusher ( std::ostream* out_s ) :
        out(out_s)                  {};

    virtual
    ~Flusher()              = default;

    virtual bool 
    update (std::string)    = 0;
    
    virtual void 
    flush ();
    
    virtual void
    set_filename ();

protected:
    container       commands;
    stack           block_separators;
    std::string     filename;
    std::ostream*   out;
};


class DefaultFlusher : public Flusher
{
public:
    DefaultFlusher ( size_t buffer_size, std::ostream& out_s )  :
        Flusher(&out_s), buffer_size(buffer_size)               {};
 
    bool 
    update  (std::string);

private:
    size_t          buffer_size;
};


class BlockFlusher : public Flusher
{
public:
    BlockFlusher ( std::ostream& out_s )    :
        Flusher(&out_s)                     {};
    
    bool 
    update  (std::string);
};
