#include <iostream>
#include <vector>

#include <dlib/clustering.h>
#include <dlib/serialize.h>

#include "read.h"
#include "unifier.h"

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

    typedef matrix<double,7,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;
    kcentroid<kernel_type> kc ( kernel_type ( .1 ), .01, 1024 );
    kkmeans<kernel_type> test ( kc );

    auto unifier = realty::SampleUnifier ( );

    std::vector<sample_type> samples;
    auto  __samples = realty::get_raw_data ( );

    unifier.restore_from_file ( "unifier.dat" );

    for ( int i = 0; i < 7; ++i )
    {
        unifier.unify ( __samples, i );
    }

    sample_type m;

    samples.reserve ( __samples.size ( ) );
    for ( auto& sample: __samples )
    {
        for ( int i = 0; i < 7; ++i )
        {
            m ( i ) = sample[i];
        }
        samples.push_back ( m );
    }

    deserialize ( filename + ".dat" ) >> test;
    for (unsigned long i = 0; i < samples.size(); ++i)
    {
        auto x = samples[i];
        std::cout << test ( samples[i] ) + 1 << std::endl;
    }

    return 0;
}
