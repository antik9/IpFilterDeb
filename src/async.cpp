#include <memory>
#include "async.h"

namespace async
{
    using ConnectorPtr = std::shared_ptr<Connector>;

    static std::unordered_map<int, ConnectorPtr> __connectors;

    handle_t
    connect ( std::size_t bulk )
    {
        for ( int i = 0; i < MAX_CONNECTIONS; ++i )
        {
            if ( __connectors.count ( i ) == 0 )
            {
                auto connector_ptr = std::make_shared<Connector> ( bulk );
                connector_ptr->connect ( );
                __connectors.insert ( std::pair<int, ConnectorPtr> ( i, connector_ptr ) );
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
            __connectors.erase ( handle_connection );
        }
    }

}
