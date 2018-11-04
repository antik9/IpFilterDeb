#include <iostream>

#include "advisor.h"

int
main ( int argc, char **argv )
{
    if ( argc != 3 )
    {
        std::cout << "Provide number of clusters and filename: ./rclst <num> <filename>\n";
        exit ( 1 );
    }

    unsigned short number_of_clusters = std::atoi ( argv[1] );
    if ( number_of_clusters == 0 )
    {
        std::cout << "Number of clusters should be a positive number\n";
    }
    std::string filename = argv[2];

    realty::Advisor advisor ( filename );
    advisor.read_data       ( );
    advisor.train           ( number_of_clusters );
    advisor.save            ( );

    return 0;
}
