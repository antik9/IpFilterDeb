#include <algorithm>
#include <iostream>
#include <type_traits>
#include <utility>
#include <vector>

#include "concepts.hpp"


struct SomeStruct {
    explicit SomeStruct(int a) {}
    SomeStruct() = delete;

    bool operator<(const SomeStruct& ) {
        return true;
    }
};

template<typename T>
void customSort(typename std::vector<T>& values) {
    BOOST_CONCEPT_ASSERT((concepts::LessThanComparable<T>));
    std::sort(std::begin(values), std::end(values));
}

int main(int argc, char * argv[]) {
    std::vector<SomeStruct> values;

    // 1. The LessThenComparable doesn't check default constructible
    std::cout 
        << "SomeStruct is " 
        << ( std::is_default_constructible<SomeStruct>::value ? "" : "not " )
        << "default constructible"
        << std::endl;
    customSort(values);

    // 2. RandomAccessIterator check for vector
    BOOST_CONCEPT_ASSERT((concepts::RandomAccessIterator<decltype(values.begin())>));

    return 0;
}
