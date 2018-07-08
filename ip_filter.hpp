#pragma once

#include <vector>
#include <string>

using string_vector = std::vector<std::string>;
using string_size_t = std::string::size_type;

string_vector split(const std::string &str, char delim);

void __print_ip_pool_by_filter(std::vector<string_vector> ip_pool, 
        const string_vector &conditions, bool anyof = false); 

template<typename ...T>
void print_ip_pool_by_filter(std::vector<string_vector> ip_pool, 
        T... filters) {
    const string_vector conditions {filters...};
    __print_ip_pool_by_filter(ip_pool, conditions); 
}

template<typename ...T>
void print_ip_pool_by_filter_any_of(std::vector<string_vector> ip_pool, 
        T... filters) {
    const string_vector conditions {filters...};
    __print_ip_pool_by_filter(ip_pool, conditions, true); 
}

