#include <iostream>

#include "advisor.h"

using namespace dlib;

int
main ( int argc, char **argv )
{
    if ( argc != 2 )
    {
        std::cout << "Provide filename: ./rclss <filename>\n";
        exit ( 1 );
    }

    std::string filename = argv[1];

    realty::Advisor advisor ( filename );
    advisor.restore ( );
    advisor.read_data ( );

    while ( advisor.has_advice ( ) )
    {
        advisor.give_advice ( );
    }

    return 0;
}
