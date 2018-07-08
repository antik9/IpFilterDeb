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


void __print_ip_pool_by_filter(std::vector<string_vector> ip_pool, 
        const string_vector &conditions, bool any_of = false) 
{
    /* 
     * Lambda function to check if ip_address satisfy to condtion.
     * If any_of flag is true, then we should check that octet of
     * ip_address is equal to any of condition strings.
     */
    auto is_satisfy = [](const string_vector& ip_address,
            const string_vector &conditions, bool any_of) {
        if ( any_of ) {
            for ( auto ip_it = ip_address.begin(); ip_it < ip_address.end(); ++ip_it)
                for ( auto filter_it = conditions.begin();
                        filter_it < conditions.end(); ++ filter_it)
                    if ( *filter_it == *ip_it )
                        return true;
            return false;
        }
        else {
            for (size_t i = 0; i < conditions.size(); ++i)
                if ( ip_address[i] != conditions[i] )
                    return false;
            return true;
        }
    };

    for(auto ip = ip_pool.cbegin(); ip != ip_pool.cend(); ++ip)
    {
        if (is_satisfy(*ip, conditions, any_of)) { 
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

