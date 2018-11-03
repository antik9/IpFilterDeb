#include <iostream>
#include <string>
#include <vector>

#include <dlib/clustering.h>
#include <dlib/rand.h>

using namespace std;
using namespace dlib;

void
unify ( std::vector<std::vector<double>>& samples, int idx )
{
    double min = samples[0][idx], max = samples[0][idx];
    for ( auto& sample: samples )
    {
        min = min < sample[idx] ? min : sample[idx];
        max = max > sample[idx] ? max : sample[idx];
    }
    double delta = max - min;

    for ( auto& sample: samples )
    {
        sample[idx] = ( sample[idx] - min ) / delta;
    }
}


bool
read_double ( std::string& buffer, double& value )
{
    int i = 0;
    bool has_digits = false, end_of_file = false;
    for ( ; i < buffer.size ( ); ++i )
    {
        if ( buffer[i] == ';' or buffer[i] == '\n' )
        {
            break;
        }
        else if ( buffer[i] == EOF )
        {
            has_digits  = false;
            end_of_file = true;
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
    return end_of_file;
}

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

    typedef matrix<double,7,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;

    kcentroid<kernel_type> kc ( kernel_type ( .1 ), .01, 1024 );
    kkmeans<kernel_type> test ( kc );

    std::vector<sample_type> samples;
    std::vector<sample_type> initial_centers;
    std::vector<std::vector<double>>  __samples;

    sample_type m;
    dlib::rand rnd;

    std::vector<double> attrs ( 7, 0.0 );
    double floor, last_floor;

    while ( true )
    {
        bool end_of_file = false;
        std::string buffer;
        std::getline ( std::cin, buffer );
        if ( std::cin.eof ( ) )
            break;
        fflush(stdin);
        for ( int i = 0; i < 6; ++i )
        {
            end_of_file = read_double ( buffer, attrs[i] );
        }
        end_of_file = read_double ( buffer, floor );
        end_of_file = read_double ( buffer, last_floor );

        attrs[6] = ( floor == last_floor ) ? 0.0 : 1.0;

        // std::cout << attrs[0] << " " << attrs[1] << " " << " " << attrs[2] << " "
        //     << attrs[3] <<  " " << attrs[4] << " " << attrs[5] << " " << attrs[6] << std::endl;
        // samples.push_back ( m );
        __samples.push_back ( attrs );
    }

    std::cout << "OK\n";
    for ( int i = 0; i < 7; ++i )
    {
        unify ( __samples, i );
    }
    std::cout << "OK\n";
    std::cout << "END\n";

    samples.reserve ( __samples.size ( ) );
    for ( auto& sample: __samples )
    {
        for ( int i = 0; i < 7; ++i )
        {
            m ( i ) = sample[i];
        }
        samples.push_back ( m );
    }

    test.set_number_of_centers ( number_of_clusters );
    pick_initial_centers ( number_of_clusters, initial_centers, samples, test.get_kernel ( ) );
    test.train ( samples, initial_centers );

    std::cout << "Sample size " << samples.size ( ) << std::endl;
    for (unsigned long i = 0; i < samples.size(); ++i)
    {
        auto x = samples[i];
        cout << test ( samples[i] ) + 1 << std::endl;
    }

    // for ( auto& attrs: __samples )
    //     std::cout << attrs[0] << " " << attrs[1] << " " << " " << attrs[2] << " "
    //         << attrs[3] <<  " " << attrs[4] << " " << attrs[5] << " " << attrs[6] << std::endl;
    return 0;
}
