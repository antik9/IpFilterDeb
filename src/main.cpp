#include <iostream>
#include <vector>

#include <dlib/clustering.h>
#include <dlib/rand.h>

using namespace std;
using namespace dlib;

int
main ( int argc, char **argv )
{
    if ( argc != 2 )
    {
        std::cout << "Provide number of clusters: ./kkmeans <num>\n";
        exit ( 1 );
    }

    unsigned short number_of_clusters = std::atoi ( argv[1] );
    if ( number_of_clusters == 0 )
    {
        std::cout << "Number of clusters should be a positive number\n";
    }

    typedef matrix<double,2,1> sample_type;
    typedef radial_basis_kernel<sample_type> kernel_type;

    kcentroid<kernel_type> kc(kernel_type(0.1),0.01, 8);
    kkmeans<kernel_type> test(kc);

    std::vector<sample_type> samples;
    std::vector<sample_type> initial_centers;

    sample_type m;
    dlib::rand rnd;

    while ( true )
    {
        double x, y;
        char separator;
        cin >> x >> separator >> y;
        if ( not cin.good ( ) )
        {
            break;
        }
        m(0) = x;
        m(1) = y;
        samples.push_back ( m );
    }

    test.set_number_of_centers ( number_of_clusters );
    pick_initial_centers ( number_of_clusters, initial_centers, samples, test.get_kernel ( ) );
    test.train(samples,initial_centers);

    for (unsigned long i = 0; i < samples.size(); ++i)
    {
        auto x = samples[i];
        cout << *x.begin ( ) << ";" << *( x.begin ( ) + 1 )
            << ";" << test ( samples[i] ) + 1 << std::endl;
    }

    return 0;
}
