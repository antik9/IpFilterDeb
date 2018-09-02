#include <boost/concept_check.hpp>

namespace concepts
{
    /* Equality Comparable*/
    template <typename Iter>
    struct
    EqualityComparable
    {
        BOOST_CONCEPT_USAGE(EqualityComparable)
        {
            bool result = ( it == other );
            (void) result;
        }

    private:
        Iter it, other;
    };

    /* Less Then Comparable */
    template <typename T>
    struct 
    LessThanComparable
    {
        BOOST_CONCEPT_USAGE(LessThanComparable)
        {
            bool result = left < right;  // required boolean < operator
            (void) result;
        }
    
    private:
        T left, right;
    };

    /* Move Constructible */
    template <typename Iter>
    struct
    MoveConstructible 
    {
        BOOST_CONCEPT_USAGE(MoveConstructible)
        {
            using dereferenced  = typename std::remove_reference<Iter>::type;
            using rvalue_t      = typename std::add_rvalue_reference<dereferenced>::type;
            Iter it             = reinterpret_cast<rvalue_t>(other);
        }

    private:
        Iter other;
    };

    /* Move Assignable */
    template <typename Iter>
    struct
    MoveAssignable 
    {
        BOOST_CONCEPT_USAGE(MoveAssignable)
        {
            using dereferenced  = typename std::remove_reference<Iter>::type;
            using rvalue_t      = typename std::add_rvalue_reference<dereferenced>::type;
            it                  = reinterpret_cast<rvalue_t>(other);
        }

    private:
        Iter it, other;
    };

    /* Copy Constructible */
    template <typename Iter>
    struct
    CopyConstructible 
    {
        BOOST_CONCEPT_ASSERT((concepts::MoveConstructible<Iter>));
        BOOST_CONCEPT_USAGE(CopyConstructible)
        {
            Iter it_simple      = other;
            Iter it_const       = other_const;
            using dereferenced  = typename std::remove_reference<Iter>::type;
            using rvalue_t      = typename std::add_rvalue_reference<dereferenced>::type;
            using crvalue_t     = typename std::add_const<rvalue_t>::type;
            Iter it_crvalue     = reinterpret_cast<crvalue_t>(other);
        }

    private:
        Iter other;
        const Iter other_const;
    };

    /* Copy Assignable */
    template <typename Iter>
    struct
    CopyAssignable 
    {
        BOOST_CONCEPT_ASSERT((concepts::MoveAssignable<Iter>));
        BOOST_CONCEPT_USAGE(CopyAssignable)
        {
            it = other;
            it = const_other;
            using dereferenced  = typename std::remove_reference<Iter>::type;
            using rvalue_t      = typename std::add_rvalue_reference<dereferenced>::type;
            using crvalue_t     = typename std::add_const<rvalue_t>::type;
            it                  = reinterpret_cast<crvalue_t>(other);

        }
    private:
        Iter it, other;
        const Iter const_other;
    };

    /* Destructible */
    template <typename Iter>
    struct
    Destructible 
    {
        BOOST_CONCEPT_USAGE(Destructible)
        {
            it.~Iter();
        }

    private:
        Iter it;
    };

    /* Swappable */
    template <typename Iter>
    struct
    Swappable 
    {
        BOOST_CONCEPT_USAGE(Swappable)
        {
            std::swap(it, other);
            std::swap(other, it);
        }

    private:
        Iter it, other;
    };

    /* Iterator */
    template <typename Iter>
    struct
    Iterator 
    {
        BOOST_CONCEPT_ASSERT((concepts::CopyConstructible<Iter>));
        BOOST_CONCEPT_ASSERT((concepts::CopyAssignable<Iter>));
        BOOST_CONCEPT_ASSERT((concepts::Destructible<Iter>));
        BOOST_CONCEPT_ASSERT((
                    concepts::Swappable<
                        typename std::add_lvalue_reference<
                            typename std::remove_reference<Iter>::type
                        >::type
                    >));

        BOOST_CONCEPT_USAGE(Iterator)
        {
            *it;
            ++it;
        }

    private:
        using reference         = typename std::iterator_traits<Iter>::reference;
        using value_type        = typename std::iterator_traits<Iter>::value_type;
        using difference_type   = typename std::iterator_traits<Iter>::difference_type;
        using pointer           = typename std::iterator_traits<Iter>::pointer;
        using iterator_category = typename std::iterator_traits<Iter>::iterator_category;
        Iter it;
    };

    /* Input Iterator */
    template <typename Iter>
    struct
    InputIterator 
    {
        BOOST_CONCEPT_ASSERT((concepts::EqualityComparable<Iter>));
        BOOST_CONCEPT_ASSERT((concepts::Iterator<Iter>));

        BOOST_CONCEPT_USAGE(InputIterator)
        {
            bool result = it != other;
            (void) result;
            *it;
            ++it;
            (void) it++;
            *it++;
        }

    private:
        using reference     = typename std::iterator_traits<Iter>::reference;
        using value_type    = typename std::iterator_traits<Iter>::value_type;
        Iter it, other;
    };

    /* Default Constructible */
    template <typename Iter>
    struct
    DefaultConstructible 
    {
        BOOST_CONCEPT_USAGE(DefaultConstructible)
        {
            Iter it_simple;
            Iter it_list_init {};
            
            try { Iter (); }
            catch(...) { Iter {}; }
        }
    };

    /* Forward Iterator */
    template <typename Iter>
    struct
    ForwardIterator 
    {
        BOOST_CONCEPT_ASSERT((concepts::DefaultConstructible<Iter>));
        BOOST_CONCEPT_ASSERT((concepts::InputIterator<Iter>));
        BOOST_CONCEPT_USAGE(ForwardIterator)
        {
            it++;
            *it++;
        }
    private:
        using reference = typename std::iterator_traits<Iter>::reference;
        Iter it;
    };

    /* Bidirectional Iterator */
    template <typename Iter>
    struct
    BidirectionalIterator
    {
        BOOST_CONCEPT_ASSERT((concepts::ForwardIterator<Iter>));
        BOOST_CONCEPT_USAGE(BidirectionalIterator)
        {
            --it;
            it--;
            *--it;
        }

    private:
        using reference = typename std::iterator_traits<Iter>::reference;
        Iter it;
    };

    /* Random Access Iterator */
    template <typename Iter>
    struct
    RandomAccessIterator 
    {
        BOOST_CONCEPT_ASSERT((concepts::LessThanComparable<Iter>));
        BOOST_CONCEPT_ASSERT((concepts::BidirectionalIterator<Iter>));
    private:
    };
};
