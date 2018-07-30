#include <iostream>
#include <map>
#include "list.hpp"
#include "allocator.hpp"

int factorial (int n) { 
    return n == 0 ? 1 : n * factorial(n - 1); 
}

int main()
{
    using int_pair = std::pair<int,int>;
    
    // DEFAULT MAP
    std::map<int, int>  default_map;
    for (int i = 0; i < 10; ++i)
        default_map.insert(int_pair{i, factorial(i)});
    
    // MAP WITH CUSTOM ALLOCATOR
    std::map<int, int, std::less<int>, Allocator<int, 10>>  custom_map;
    for (int i = 0; i < 10; ++i)
        custom_map.insert(int_pair{i, factorial(i)});

    for (auto it = custom_map.begin(); it != custom_map.end(); ++it)
        std::cout << it->first << " " << it->second << std::endl;
   
    // DEFAULT ALLOCATOR WITH CUSTOM LIST
    List<int, std::allocator<int>> list_with_default_allocator;
    for (int i = 0; i < 10; ++i)
        list_with_default_allocator.push_back(i);
    
    // CUSTOM ALLOCATOR WITH CUSTOM LIST
    List<int, Allocator<int, 10>> list_with_custom_allocator;
    for (int i = 0; i < 10; ++i)
        list_with_custom_allocator.push_back(i);

    for (auto it = list_with_custom_allocator.begin(); it != list_with_custom_allocator.end(); ++it)
        std::cout << *it << std::endl;

    return 0;
}
