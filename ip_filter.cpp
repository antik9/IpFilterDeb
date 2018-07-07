#include <iostream>
#include <vector>
#include <string>

using string_vector = std::vector<std::string>;
using string_size_t = std::string::size_type;

string_vector split(const std::string &str, char delim)
{
    string_vector ip_address;

    string_size_t start = 0;
    string_size_t stop  = str.find_first_of(delim);
    
    while(stop != std::string::npos)
    {
        ip_address.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(delim, start);
    }

    ip_address.push_back(str.substr(start));

    return ip_address;
}

void print_ip_pool_by_filter(std::vector<string_vector> ip_pool, 
        bool (*is_satisfy) (const string_vector &ip_address)) 
{
    for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        if (is_satisfy(*ip)) { 
            for(auto ip_part = ip->cbegin(); ip_part != ip->cend(); ++ip_part)
            {
                if (ip_part != ip->cbegin())
                    std::cout << ".";
                std::cout << *ip_part;
            }
            std::cout << std::endl;
        }
    }
}

