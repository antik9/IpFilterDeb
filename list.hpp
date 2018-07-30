#pragma once
#include <cstddef>
#include <iterator>

/* Node of List class */
template<class T>
struct Node
{
public: 
    using value_type    = T;
    Node()              = default;
    value_type          value;
    Node*               next;
};


/* List class */
template<class T, class _Alloc>
class List
{
public:
    using value_type    = T;
    using size_type     = size_t;
    using node          = Node<value_type>;
    using pointer       = node*;
    using Alloc         = typename _Alloc::template rebind<node>::other; 

    List() : head(nullptr), tail(nullptr), allocator(Alloc()), post_tail(node()) {}
    
    ~List() {
        auto current    = head;
        while ( current && current != &post_tail ) {
            auto next   = current->next;
            allocator.deallocate(current, sizeof(node));
            current->~node();
            current     = next;
        }
    }

	void push_back(value_type value) {
	    auto ptr        = allocator.allocate(1);
	    ptr->value      = value;
	    if ( head == nullptr ) {
	        head        = ptr;
	        head->next  = &post_tail;
        }
        else {
            tail->next  = ptr;
        }
	    tail            = ptr;
        tail->next      = &post_tail;
	}

    struct iterator : public std::iterator<std::forward_iterator_tag, value_type> {
    public:
        pointer     it;
        iterator()                      : it()          {}
        iterator(const iterator& iter)  : it(iter.it)   {}
        iterator(pointer to)            : it(to)        {}

        bool operator!= (iterator iter)     const   { return it != iter.it; }
        bool operator== (iterator iter)     const   { return it == iter.it; }

        iterator& operator++ () {
            it      = it->next;
            return  *this;
        }

        const value_type&   operator*()     const   { return (const value_type&) *(this->it); }
        const pointer       operator->()    const   { return (const value_type&) &*(this->it); }
    };

    // определите методы begin / end
    iterator begin()    const   { return iterator(head) ; }
    iterator end()      const   { return iterator((pointer) &post_tail) ; }

private:
    pointer             head;
    pointer             tail;
    node                post_tail;
    Alloc               allocator;
};

