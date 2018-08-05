/*! 
 * \file 
 * */
#include "print_ip.hpp"

/*!
 * \brief Main function to show polymorphism of print_ip function.
 * 
 * Calls with char, short, int, long, std::vector<int>, std::list<int>, std::string
 * and std::tuple<long, long... long>.
 */
int
main(int args, char *argv[])
{
    print_ip(char(-1));
    print_ip(short(0));
    print_ip(int(2130706433));
    print_ip(long(8875824491850138409));
    print_ip("251.1.123.12");
    print_ip(std::vector<int> {101, 154, 34, 56, 11});
    print_ip(std::list<int> {12, 11, 14, 0, 12321, 242, 121});
    print_ip(std::tuple<long, long, long, long, long>{10, 43, 13, 13, 1});
    return 0;
}
