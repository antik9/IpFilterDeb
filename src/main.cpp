#include <iostream>
#include "bulk.h"

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
        bulk_size = atoi(argv[1]);
        if ( bulk_size == 0 )
        {
            std::cout << "Provide a positive number <bulk_size>" << std::endl;
            exit(1);
        }
    }

    BulkExecutor executor = BulkExecutor ( std::cin );
    DefaultFlusher default_flusher ( bulk_size, std::cout );
    BlockFlusher block_flusher ( std::cout );
    executor.attach ( &default_flusher );
    executor.attach ( &block_flusher );

    executor.read_and_execute();
    return 0;
}
