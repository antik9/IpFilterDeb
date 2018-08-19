#include <iostream>
#include <list>
#include <map>
#include <tuple>
#include <utility>

template <typename T>
const int
__stub_value(T)
{
    return -1;
}

/*
 * Struct which wrap std::map for all operations. Neeeded to obtain default value operations.
 */
template <typename Layer, typename T, T default_v>
struct MapWrapper
{
public:

    Layer& operator[] (const int& key)
    {
        return __values[key];
    }
    
    using iterator = decltype(std::map<int, Layer>().cbegin());
    iterator begin() const { return this->__values.cbegin(); }
    iterator end() const { return this->__values.cend(); }

private:
    std::map<int, Layer> __values;
};

template <typename T, T default_v>
struct MapWrapper<T, T, default_v>
{
public:

    T&
    operator[] (const int& key)
    {
        if ( __values.find(key) == __values.end() )
        {
            __values[key] = default_v;
        }
        return __values[key];
    }
    
    using iterator = decltype(std::map<int, T>().cbegin());
    iterator begin() const { return this->__values.cbegin(); }
    iterator end() const { return this->__values.cend(); }

private:
    std::map<int, T> __values;
};


/*
 * Helper struct for instantiation of inner map for matrix in time of compilation
 */
template <typename T, T default_v, int dimensions>
struct MatrixMapTypeWrapper
{
    using type  = MapWrapper<
        typename MatrixMapTypeWrapper<T, default_v, dimensions - 1>::type, T, default_v>;
};

template <typename T, T default_v>
struct MatrixMapTypeWrapper<T, default_v, 0>
{
    using type  = MapWrapper<T, T, default_v>;
};

/*
 * Helper struct for obtainint iterator type
 */
template <typename Tuple, typename T, int dimensions>
struct IterTypeWrapper
{
	using __subtype = decltype(std::tuple_cat(Tuple(), std::tuple<const int>()));
	using type      = typename IterTypeWrapper<__subtype, T, dimensions - 1>::type;
};

template <typename Tuple, typename T>
struct IterTypeWrapper<Tuple, T, 0>
{
    using type      = decltype(std::tuple_cat(Tuple(), std::tuple<T>()));
};


/*
 * Matrix class. Default dimension is common value of 2.
 */
template <typename T, T default_v, int dimensions = 2>
class Matrix
{
public:
    using value_type    = T;
    using pointer       = T*;
    using MatrixMap     = typename MatrixMapTypeWrapper<T, default_v, dimensions - 1>::type;
    using Layer         = decltype(MatrixMap()[0]);
	using IterValue		= typename IterTypeWrapper<decltype(std::tuple<>()), T, dimensions>::type;
	using IterPtr       = decltype(std::list<IterValue>().begin());

    Matrix()            = default;
    ~Matrix()           = default;
    
    Layer operator[] (const int& key) { return layers[key]; }

    struct iterator : public std::iterator<std::forward_iterator_tag, IterPtr> {
    public:
        IterPtr     it;
        
        iterator(const MatrixMap* layers)
        {
           for ( auto key_layer_pair: *layers )
           {
               __insert_values_to_iterator(std::tuple<>(), key_layer_pair);
           }
           this->__values.push_back(__stub);
           __move_to_begin();
        }
       
        template<typename Tuple>
        void
        __insert_values_to_iterator (Tuple tuple, std::pair<const int, value_type> key_layer_pair)
        {
            this->__values.push_back(std::tuple_cat(tuple, key_layer_pair));
        }
        
        template <typename Tuple, typename Pair>
        void
        __insert_values_to_iterator(Tuple tuple, Pair key_layer_pair)
        {
            for ( auto __key_layer_pair: key_layer_pair.second )
            {
                __insert_values_to_iterator(
                        std::tuple_cat(
                            tuple, std::tuple<const int>(key_layer_pair.first)), __key_layer_pair);
            }
        }
       
        template <size_t ...Ns>
        auto
        __compile_tuple( std::index_sequence<Ns...>, value_type __default_v )
        {
            return std::tuple_cat(
                    std::make_tuple((__stub_value(Ns))...), std::tuple<value_type>(__default_v));
        }

        IterValue
        __make_stub()
        {
            return __compile_tuple( std::make_index_sequence<dimensions>{}, default_v );
        }
        
        void
        __move_to_begin()
        {
            this->it = this->__values.begin();
        }

        void 
        __move_to_end()
        {
            this->it = this->__values.end();
            --this->it;
        }

        iterator(const iterator& iter)  : it(iter.it)   {}
        iterator(IterPtr to)            : it(to)        {}

        bool operator!= (iterator iter)     const   { return *it != *iter.it; }
        bool operator== (iterator iter)     const   { return *it == *iter.it; }

        iterator& operator++ () {
            do 
            {
                ++this->it;
            }
            while ( *this->it != __stub && std::get<dimensions>(*this->it) == default_v );

            return  *this;
        }

        const IterValue&    operator*()     const   { return (const IterValue&) *(this->it); }
        const IterValue*    operator->()    const   { return (const IterValue&) &*(this->it); }
    
    private:
        std::list<IterValue>    __values;
        IterValue               __stub = __make_stub();
    };

    iterator begin()    const 
    {
        iterator iter(&this->layers);
        while ( std::get<dimensions>(*iter) == default_v && *iter != iter.__make_stub() )
        {
            ++iter;
        }
        return iter;
    }

    iterator
    end()      const 
    { 
        iterator iter(&this->layers);
        iter.__move_to_end();
        return iter;
    }

    size_t size() { 
        size_t __size = 0;
        for ( auto it = this->begin(); it != this->end(); ++it )
        {
            ++__size;
        }
        return __size;
    }
    
private:
    MatrixMap layers;
};
