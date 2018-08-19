#include <cassert>
#include <iostream>
#include "matrix.hpp"


int
main(int args, char *argv[])
{
    Matrix<int, 0> matrix_nines;
    
    for ( int i = 0; i < 10; ++i )
    {
        matrix_nines[i][i] = i;
        matrix_nines[9 - i][i] = i;
    }
    
    assert(matrix_nines.size() == 18);
    
    for ( int i = 1; i <= 8; ++i )
    {
        for ( int j = 1; j <= 8; ++j )
        {
            std::cout << matrix_nines[i][j] << " ";
        }
        std::cout << std::endl;
    }

    assert(matrix_nines.size() == 18);
   
    std::cout << matrix_nines.size() << std::endl;

    for ( auto tuple_v: matrix_nines )
    {
        int x, y;
        int value;
        std::tie(x, y, value) = tuple_v;
        std::cout << x << y << value << std::endl;
    }

    assert(matrix_nines.size() == 18);
   
    // Uncomment to try
    /*
    Matrix<long, -1, 3> volume;
    volume[1][100][1000] = 12;
    volume[999][12][143] = 21;

    for ( auto tuple_v: volume )
    {
        int x, y, z;
        long value;
        std::tie(x, y, z, value) = tuple_v;
        std::cout << x << y << z << value << std::endl;
    }
    
    Matrix<char, 'a', 5> alpha;
    alpha[1][10][32][19990][123123] = 'b';
    alpha[9123][1453][2131][4][989] = 'z';
    alpha[9123][1453][2131][4][989] = 'w';
    
    for ( auto tuple_v: alpha )
    {
        int x, y, z, x_, y_;
        char value;
        std::tie(x, y, z, x_, y_, value) = tuple_v;
        std::cout << x << y << z << x_ << y_ << value << std::endl;
    }
    */

    return 0;
}
