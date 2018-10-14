#include "async.h"

namespace async
{
    handle_t
    connect ( std::size_t bulk )
    {
        for ( int i = 0; i < MAX_CONNECTIONS; ++i )
        {
            if ( __connectors.count ( i ) == 0 )
            {
                Connector* connector_ptr = new Connector ( bulk );
                connector_ptr->connect ( );
                __connectors.insert ( std::make_pair ( i, connector_ptr ) );
                return i;
            }
        }
        std::cout << "All connections are busy. Try another time.\n";
        return -1;
    }

    void
    receive ( handle_t handle, const char *data, std::size_t size )
    {
        auto handle_connection = __connectors.find ( handle );
        if ( handle_connection != __connectors.end ( ) )
        {
            handle_connection->second->receive ( std::string ( data ) );
        }
    }

    void
    disconnect ( handle_t handle )
    {
        auto handle_connection = __connectors.find ( handle );
        if ( handle_connection != __connectors.end ( ) )
        {
            handle_connection->second->disconnect ( );
            delete handle_connection->second;
            __connectors.erase ( handle_connection );
        }
    }

}
