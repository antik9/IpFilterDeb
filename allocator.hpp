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
        this->pool_line = std::make_shared<std::vector<pointer>>(std::vector<pointer>{pool});
    }
    
    Allocator(const Allocator& other) : 
        allocated       (other.allocated), 
        limit           (other.limit),
        pool            (other.pool),
        pool_line       (other.pool_line)                             {}

    ~Allocator() {
        if ( this->pool_line ) {
            for (auto it: *(this->pool_line)) 
                if ( it ) {
                    free (it);
                    it = nullptr;
                }
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
    
    void deallocate(void *ptr, size_type n) { 
        for(int i = 0; i < n; ++i) {
            this->destroy((pointer) ptr);
        }
    }

    pointer                 address(reference value)        const   { return &value; }
    const_pointer           address(const_reference value)  const   { return &value; }
    Allocator<value_type, blocksize>&  
                            operator=(const Allocator&)             { return *this; }
    
    void construct(pointer ptr, const_reference value) { new(ptr) value_type(value); }

    void                    destroy(pointer ptr)                    { ptr->~value_type(); }
    size_type               max_size()                      const   { return size_t(-1); }

    template <class U>
    struct rebind { using other = Allocator<U, blocksize>; };
    
private:
    size_type               allocated;
    size_type               limit;
    pointer                 pool;
    std::shared_ptr<std::vector<pointer>> pool_line;
};

