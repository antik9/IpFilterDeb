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
    double TEST_VALUE = 4.5; 
    double *ptr = new double;
    Allocator<double, 2> allocator;
    
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
    std::vector<double, Allocator<double, 10>> vec_double;
    for ( int i = 0; i < TEST_LOOP; ++i ) {
        if ( i % 2 )
            vec_double.push_back(14.5);
        else
            vec_double.push_back(15.4);
    }
    
    ASSERT_EQ(vec_double.size(), TEST_LOOP);

    for ( int i = 0; i < TEST_LOOP; ++i ) {
        if ( i % 2 )
            ASSERT_EQ(vec_double[i], 14.5);
        else
            ASSERT_EQ(vec_double[i], 15.4);
    }
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

