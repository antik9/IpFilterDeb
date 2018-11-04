#include <iostream>
#include "read.h"

namespace realty
{
    void
    read_double ( std::string& buffer, double& value )
    {
        int i = 0;
        bool has_digits = false;
        for ( ; i < buffer.size ( ); ++i )
        {
            if ( buffer[i] == ';' or buffer[i] == '\n' )
            {
                break;
            }
            has_digits  = true;
        }

        if ( has_digits )
        {
            value = std::stod ( buffer.substr ( 0, i ) );
        }

        if ( i + 1 < buffer.size ( ) )
        {
            buffer = buffer.substr ( i + 1 );
        }
    }

    std::vector<std::vector<double>>
    get_raw_data ( )
    {
        std::vector<std::vector<double>>  samples;
        std::vector<double> attrs ( 7, 0.0 );
        double floor, last_floor;

        while ( true )
        {
            std::string buffer;
            std::getline ( std::cin, buffer );
            if ( std::cin.eof ( ) )
                break;
            for ( int i = 0; i < 6; ++i )
            {
                read_double ( buffer, attrs[i] );
            }
            read_double ( buffer, floor );
            read_double ( buffer, last_floor );

            attrs[6] = ( floor == last_floor ) ? 0.0 : 1.0;

            samples.push_back ( attrs );
        }
        return samples;
    }
}
