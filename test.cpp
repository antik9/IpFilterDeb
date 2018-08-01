#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "allocator.hpp"
#include "list.hpp"

TEST(ListTest, iteratorTest) {
    double reference_array[] { 5.3, 4.6, 33.4 };
    List<double, std::allocator<double>> list_container;
    for (int i = 0; i < 3; ++i) {
        list_container.push_back(reference_array[i]);
    }
    int index = 0;
    for (auto it = list_container.begin(); it != list_container.end(); ++it) {
        ASSERT_EQ(*it, reference_array[index++]);
    }
    ASSERT_EQ(index, 3);
}

TEST(AllocatorTest, constructAndDestroyTest) {
    std::string TEST_VALUE = std::string("PASS");
    std::string *ptr = new std::string;
    Allocator<std::string, 2> allocator;
    
    /* Check construction */
    allocator.construct(ptr, TEST_VALUE); 
    ASSERT_EQ(*ptr, TEST_VALUE);
    
    allocator.destroy(ptr);
    delete ptr;
}

TEST(AllocatorTest, rebindTest) {
    int TEST_VALUE = 911;
    using RebindAlloc = typename Allocator<std::string, 2>::template rebind<int>::other;
    RebindAlloc allocator;
    int *ptr = new int;
    
    /* Check correct rebind */
    allocator.construct(ptr, TEST_VALUE);
    ASSERT_EQ(*ptr, TEST_VALUE);
    
    allocator.destroy(ptr);
    delete ptr;
}

TEST(AllocatorTest, allocationTest) {
    int TEST_LOOP = 5000;
    std::vector<std::string, Allocator<std::string, 10>> vec_string;
    for ( int i = 0; i < TEST_LOOP; ++i ) {
        if ( i % 2 )
            vec_string.push_back("foo");
        else
            vec_string.push_back("baz");
    }
    
    ASSERT_EQ(vec_string.size(), TEST_LOOP);

    for ( int i = 0; i < TEST_LOOP; ++i ) {
        if ( i % 2 )
            ASSERT_EQ(vec_string[i], "foo");
        else
            ASSERT_EQ(vec_string[i], "baz");
    }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

