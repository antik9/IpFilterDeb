#pragma once

#include <vector>
#include <string>

using string_vector = std::vector<std::string>;
using string_size_t = std::string::size_type;

string_vector split(const std::string &str, char delim);

void print_ip_pool_by_filter(std::vector<string_vector> ip_bool,
        bool (*is_satisfy) (const string_vector &ip_address));
