#include <gtest/gtest.h>
#include <string>
#include "../src/matrix.hpp"

TEST(MATRIX, emptiness)
{
    Matrix<char, 'u', 5> alpha;
    size_t size = alpha.size();
    ASSERT_EQ ( size, 0 );
    
    auto _ = alpha[0][1][2][4][5];
    size = alpha.size();
    ASSERT_EQ ( size, 0 );
}

TEST(MATRIX, assignment)
{
    Matrix<int, 0, 2> numeric;
    size_t size = numeric.size();
    ASSERT_EQ ( size, 0 );
    ASSERT_EQ ( numeric[100][1], 0 );
    
    numeric[10][9] = 1;
    size = numeric.size();
    ASSERT_EQ ( size, 1 );
    ASSERT_EQ ( numeric[10][9], 1 );
}

TEST(MATRIX, iterator) 
{
    Matrix<int, 1, 4> numeric;
    numeric[0][2][100][99]    = 100;
    numeric[10][3][12][98]    = 200;
    numeric[0][0][0][1000]    = 300;

    auto iter   = numeric.begin();
    auto end    = numeric.end();

    ASSERT_EQ ( *iter, std::make_tuple(0, 0, 0, 1000, 300) );
    ++iter;
    ASSERT_EQ ( *iter, std::make_tuple(0, 2, 100, 99, 100) );
    ++iter;
    ASSERT_EQ ( *iter, std::make_tuple(10, 3, 12, 98, 200) );
    ++iter;
    ASSERT_EQ ( iter, end );
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
