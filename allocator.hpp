#pragma once
#include <vector>
#include <memory>

template <class T, size_t blocksize>
class Allocator
{
public:
    using size_type         = size_t;
    using difference_type   = ptrdiff_t;
    using pointer           = T*;
    using const_pointer     = const T*;
    using reference         = T&;
    using const_reference   = const T&;
    using value_type        = T;

    Allocator() : 
        allocated       (0), 
        limit           (blocksize), 
        pool            ((pointer) malloc(blocksize * sizeof(value_type))) 
    {
        this->_pool_line        = std::vector<pointer>{pool};
        this->pool_line         = std::make_shared<std::vector<pointer>>(_pool_line);
    }
    
    Allocator(const Allocator& other) : 
        allocated       (other.get_allocated()), 
        limit           (other.get_limit()),
        pool            (other.get_pool()),
        pool_line       (other.get_pool_line())                             {}

    ~Allocator() {
        if ( this->pool_line )
            for (auto it = this->pool_line->begin(); it != this->pool_line->end(); ++it)
                if ( *it ) {
                    free (*it);
                    *it = nullptr;
                }
    }

    pointer allocate(size_type n, const void* = 0) {
        auto available_space    = this->limit - this->allocated;
        
        if ( available_space >= n ) {
            auto ptr            = this->pool + (difference_type) this->allocated;
            this->allocated     += n;
            return ptr;
        } else {
            if ( n == 1 ) {
                this->pool          = (pointer) malloc(blocksize * sizeof(value_type));
                this->allocated     = 1;
            } else {
                this->pool          = (pointer) malloc(n * sizeof(value_type));
                this->allocated     = n;
                this->limit         = n;
            }
            this->pool_line->push_back(this->pool);
            return this->pool;
        }
    }
    
    void deallocate(void *ptr, size_type n) { }

    pointer                 address(reference value)        const   { return &value; }
    const_pointer           address(const_reference value)  const   { return &value; }
    Allocator<value_type, blocksize>&  
                            operator=(const Allocator&)             { return *this; }
    
    void construct(pointer ptr, const_reference value) {
        new ((pointer) ptr) value_type(value);
    }

    void                    destroy(pointer ptr)                    { ptr->~value_type(); }
    size_type               max_size()                      const   { return size_t(-1); }

    template <class U>
    struct rebind { using other = Allocator<U, blocksize>; };
    
    size_type               get_allocated()                 const   { return allocated; }
    size_type               get_limit()                     const   { return limit; }
    pointer                 get_pool()                      const   { return pool; }
    auto                    get_pool_line()                 const   { return pool_line; }

private:
    size_type               allocated;
    size_type               limit;
    pointer                 pool;
    std::vector<pointer>    _pool_line;
    std::shared_ptr<std::vector<pointer>> pool_line;
};

