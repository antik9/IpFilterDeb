#include <iostream>
#include "bulk.h"

size_t
custom_atoi ( char* param )
{
    size_t value = 0;
    while ( *param != '\0' )
    {
        if ( *param < '0' || *param > '9' )
        {
            value = 0;
            break;
        }
        else 
        {
            value *= 10;
            value += (int) *param - '0';
        }

        ++param;
    }

    return value;
}

int
main(int args, char *argv[])
{
    size_t bulk_size;
    if ( args != 2 )
    {
        std::cout << "Run command as `bulk <bulk_size>`" << std::endl;
        exit(1);
    }
    else
    {
        bulk_size = custom_atoi(argv[1]);
        if ( bulk_size == 0 )
        {
            std::cout << "Provide a positive number <bulk_size>" << std::endl;
            exit(1);
        }
    }

    BulkExecutor executor = BulkExecutor(std::cin);
    DefaultFlusher default_flusher(executor, bulk_size, std::cout);
    BlockFlusher block_flusher(executor, std::cout);

    executor.read_and_execute();
    return 0;
}
