#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include "version.h"
#include "ip_filter.h"

/* 
 * using string_vector = std::vector<std::string>;
 * using string_size_t = std::string::size_type;
 */


int main(int argc, char const *argv[])
{
    try
    {
        if (argc == 2 && std::string(argv[1]) == "-v" 
                || std::string(argv[1]) == "--version") {
            std::cout << "VERSION: " << version() << std::endl;
            return 0;
        }

        std::vector<string_vector> ip_pool;

        for(std::string line; std::getline(std::cin, line);)
        {
             string_vector incoming_information = split(line, '\t');
             ip_pool.push_back(split(incoming_information.at(0), '.'));
        }
        
        // Sort vector of ip-addresses in reverse lexicographical order
        // Compare each octet in order from left to right
        std::sort(ip_pool.begin(), ip_pool.end(), 
                [](const string_vector &first, const string_vector &second) {
                    for (int i = 0; i < 4; ++i) {
                        if (first[i].size() > second[i].size())
                            return true;
                        else if (first[i].size() < second[i].size())
                            return false;
                        else if (first[i].compare(second[i]))
                            return first[i].compare(second[i]) > 0 ? true : false;
                    }
                    return false;
                    });

        // Filtering
        // First condition to print all ip-addresses without any filter
        print_ip_pool_by_filter(ip_pool, [](const string_vector &ip_address) {
                    return true;
                });

        // Second condition to print all ip addresses satisfied by `1.*.*.*`
        print_ip_pool_by_filter(ip_pool, [](const string_vector &ip_address) {
                    return ip_address[0] == "1";
                });
        
        // Third condition to print all ip addresses satisfied by `46.70.*.*` 
        print_ip_pool_by_filter(ip_pool, [](const string_vector &ip_address) {
                    return ip_address[0] == "46" && ip_address[1] == "70";
                });
        
        // Fourth condition to print all ip addresses satisfied by having 46 in it 
        print_ip_pool_by_filter(ip_pool, [](const string_vector &ip_address) {
                    return ip_address[0] == "46" || ip_address[1] == "46" 
                        || ip_address[2] == "46" || ip_address[3] == "46";
                });
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
