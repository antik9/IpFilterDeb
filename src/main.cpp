#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "map_reduce.h"

using namespace std::experimental::filesystem;

int
main ( int argc, char **argv )
{
    if ( argc != 4 )
    {
        std::cout << "Usage: yamr <src> <mnum> <rnum>\n";
        return 1;
    }

    unsigned short n_mappers    = std::atoi ( argv[2] );
    unsigned short n_reducers   = std::atoi ( argv[3] );

    if ( n_mappers == 0 or n_reducers == 0 )
    {
        std::cout << "Number of mappers or reducers cannot be `0`\n";
        return 2;
    }

    if ( not exists ( argv[1] ) )
    {
        std::cout << "File " << argv[1] << " does not exist\n";
    }

    std::vector<std::thread> mappers;
    mappers.reserve ( n_mappers );
    std::vector<map::Container> mapper_containers ( n_mappers );

    std::string src = argv[1];
    std::ifstream in ( src, std::ifstream::ate | std::ifstream::binary );
    std::ifstream::pos_type src_size = in.tellg ( );
    std::ifstream::pos_type current = 0, next = 0, partition = src_size / n_mappers;

    for ( int i = 0; i < n_mappers; ++i )
    {
        char next_chr;
        current = next;
        next = current + partition > src_size ? src_size : current + partition;
        in.seekg ( next );

        while ( next != EOF )
        {
            in.get ( next_chr );
            next = in.tellg ( );
            if ( next_chr == '\n' )
            {
                break;
            }
        }

        mappers.emplace_back ( std::thread ( map::map,
                                            std::ref ( mapper_containers[i] ),
                                            std::ref ( src ), current, next ) );
    }

    in.close ( );

    for ( int i = 0; i < n_mappers; ++i )
    {
        mappers[i].join ( );
    }


    std::vector<std::thread> reducers;
    reducers.reserve ( n_reducers );
    std::vector<std::queue<std::string>> reducer_containers ( n_reducers );

    map::shuffle ( mapper_containers, reducer_containers );

    for ( int i = 0; i < n_reducers; ++i )
    {
        reducers.emplace_back ( std::thread ( map::reduce,
                                            std::ref ( reducer_containers[i] ), i + 1 ) );
    }

    for ( int i = 0; i < n_reducers; ++i )
    {
        reducers[i].join ( );
    }

    return 0;
}
