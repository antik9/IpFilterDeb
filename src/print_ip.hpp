/*!
 * \file
 * \brief module provides templates for printing ip from various sources coming into the function.
 */

#include <iostream>
#include <list>
#include <vector>
#include <tuple>
#include <type_traits>

const unsigned int OCTAL = 8;
const unsigned int OCTET = ((1 << OCTAL) - 1);

// Forward Declaration 
template<typename T>
void
__print_by_octets(T ip_v, int bytes, bool eofline=false); 

template<typename... T>
void 
__print_tuple_args(const std::tuple<T...>& ip_v); 
    
// Definition

/*!
 * \brief Helper struct to identify std::list and std::vector STL containers.
 */
template<class T>
struct __is_container
{
    /*!
     * Bool value indicator is passed type T is std::list or std::vector container
     */
    static const bool value = false;
};

/*!
 * \brief Helper struct to identify std::list and std::vector STL containers.
 */
template <typename ...T>
struct __is_container<std::list<T...>>
{
    /*!
     * Bool value indicator is passed type T is std::list or std::vector container
     */
    static const bool value = true;
};

/*!
 * \brief Helper struct to identify std::list and std::vector STL containers.
 */
template <typename ...T>
struct __is_container<std::vector<T...>>
{
    /*!
     * Bool value indicator is passed type T is std::list or std::vector container
     */
    static const bool value = true;
};

/*!
 * \brief Helper class for tuple type checking.
 */
template<typename T, typename U, typename ...Args>
struct 
__is_unified_pack 
{
    static const bool value = std::is_same<T, U>::value & __is_unified_pack<T, Args...>::value;
};

/*!
 * \brief Helper class for tuple type checking.
 */
template<typename T, typename U>
struct 
__is_unified_pack<T, U>
{
    static const bool value = std::is_same<T, U>::value;
};

/*!
 * \brief Struct for checking tuple for unificated values.
 *
 * In the time of compilation struct checks argument by argument for provided tuple
 * if all of them have the same type.
 */
template<typename T, typename ...Args>
struct 
__is_unified_tuple 
{
    /*! 
     * Value is static bool which __is_unified_tuple struct provides for type-checking.
     * \code
     * __is_unified_tuple::value
     * \endcode
     */
    static const bool value = __is_unified_pack<T, T, Args...>::value;
};

/*!
 * \brief Function print_ip for integral types.
 * \param[in] ip_v IP address representation as a char, short, int or long
 * \param[in] eofline bool value to indicate should be EOL be printed after ip
 */
template<class T>
typename std::enable_if<std::is_integral<T>::value, void>::type
print_ip(T ip_v, bool eofline=true)
{
    size_t bytes = sizeof(T);
    __print_by_octets(ip_v, bytes, eofline);
}

/*!
 * \brief Function print_ip for std::string type. Default behaviour: print string as it is.
 * \param[in] ip_v IP address representation as a std::string.
 * \param[in] eofline bool value to indicate should be EOL be printed after ip
 */
template<typename T=std::string>
void
print_ip(const std::string& ip_v, bool eofline=true)
{
    std::cout << ip_v;
    if ( eofline ) 
    {
        std::cout << std::endl;
    }
}

/*!
 * \brief Function print_ip for std::list and std::vector containers. 
 * \param[in] ip_v IP address representation as a container.
 * \param[in] eofline bool value to indicate should be EOL be printed after ip
 */
template<typename Container>
typename std::enable_if<__is_container<Container>::value, void>::type
print_ip(Container ip_v, bool eofline=true)
{
    unsigned int index_of_octet = 0;
    size_t last_element_index = ip_v.size() - 1;
    for ( auto octet: ip_v ) 
    {
        bool last_element = ( index_of_octet++ == last_element_index );
        std::cout << octet << ( last_element ? "" : "." );
    }

    if ( eofline ) 
    {
        std::cout << std::endl;
    }
}

/*!
 * \brief Detalisation of tuple template for empty tuple. Do nothing.
 */
void
print_ip(const std::tuple<>)
{
}

/*!
 * \brief Function print_ip for std::tuple. Tuple will be printed if only all of its
 * elements have the same type.
 * \param[in] ip_v IP address representation as a std::tuple<>
 * \param[in] eofline bool value to indicate should be EOL be printed after ip
 */
template<typename T, typename ...Ts>
typename std::enable_if<__is_unified_tuple<T, Ts...>::value, void>::type
print_ip(const std::tuple<T, Ts...>& ip_v, bool eofline=true)
{
    __print_tuple_args(ip_v);
    if ( eofline ) 
    {
        std::cout << std::endl;
    }
}

/*!
 * \brief Helper function to print integral types representation of ip.
 * \param[in] ip_v char, short, int or long integral type
 * \param[in] bytes number of bytes left to print
 * \param[in] eofline bool value to indicate shoule be EOL be printed adter ip
 */
template<typename T>
void
__print_by_octets(T ip_v, int bytes, bool eofline) 
{
    if ( not bytes )
    {
        return;
    }
    
    bool last_element = ( bytes == sizeof(T) );
    __print_by_octets(ip_v >> 8, bytes - 1, false);
    std::cout << ((int) ip_v & OCTET) << ( last_element ? "" : "." );
    
    if ( eofline ) 
    {
        std::cout << std::endl;
    }
}

/*!
 * \brief Helper struct to print tuple element by element
 */
template<size_t index, typename... T>
struct 
__print_tuple 
{
    /*!
     * \brief Call operator to print to stdout next tuple element.
     */
    void operator() (const std::tuple<T...>& ip_v) 
    {
        bool last_element = index == std::tuple_size<std::tuple<T...>>::value - 1;
        __print_tuple<index - 1, T...> {} (ip_v);
        std::cout << std::get<index>(ip_v) << ( last_element ? "" : "." );
    }
};

/*!
 * \brief Helper struct to print tuple element by element
 */
template<typename... T>
struct
__print_tuple<0, T...> 
{
    /*!
     * \brief Call operator to print to stdout next tuple element.
     */
    void operator() (const std::tuple<T...>& ip_v) 
    {
        bool last_element = ( 0 == std::tuple_size<std::tuple<T...>>::value - 1 );
        std::cout << std::get<0>(ip_v) << ( last_element ? "" : "." );
    }
};

/*!
 * \brief Function to print tuple element by element.
 * \param[in] ip_v tuple with multiple elements with.
 */
template<typename... T>
void 
__print_tuple_args(const std::tuple<T...>& ip_v) 
{
    const auto size = std::tuple_size<std::tuple<T...>>::value;
    __print_tuple<size - 1, T...> {} (ip_v);
}

