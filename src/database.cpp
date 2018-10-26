#include <sstream>

#include "database.h"

namespace join
{
    std::string
    Database::insert    ( const std::string& command )
    {
        int separator_idx = 0, start_idx = 0;

        if ( next_pos ( start_idx, separator_idx, command ) == INCORRECT_SYNTAX )
        {
            return std::string ( CHECK_SYNTAX );
        }

        std::string table_name = command.substr ( 0, separator_idx );

        if ( separator_idx != 1 or ( table_name != TABLE_A_NAME and table_name != TABLE_B_NAME ) )
        {
            return std::string ( INCORRECT_TABLE_NAME + " " + table_name + "\n" );
        }

        if ( next_pos ( start_idx, separator_idx, command ) == INCORRECT_SYNTAX )
        {
            return std::string ( CHECK_SYNTAX );
        }

        unsigned int id = std::atoi (
                            command.substr ( start_idx, separator_idx - start_idx ).c_str ( ) );

        start_idx = separator_idx + 1;
        if ( ( separator_idx = command.find ( SPACE_SEPARATOR, start_idx ) ) != std::string::npos )
        {
            return std::string ( CHECK_SYNTAX );
        }

        std::string name            = command.substr ( start_idx );
        if ( name[name.size ( ) - 1] == '\n' )
        {
            name = name.substr ( 0, name.size ( ) - 1 );
        }

        Table& update_table         = table_name == TABLE_A_NAME ? A : B;
        Lock&  update_table_lock    = table_name == TABLE_A_NAME ? lock_A : lock_B;

        {
            WriteLock w_lock ( update_table_lock );
            if ( update_table.find ( id ) != update_table.end ( ) )
            {
                return std::string ( ERR_DUPLICATE + " " + std::to_string ( id ) + "\n" );
            }
            update_table.insert ( std::make_pair ( id, name ) );
        }

        return SUCCESS;
    }

    std::string
    Database::inner_join  ( )
    {
        ReadLock w_lock_A ( lock_A );
        ReadLock w_lock_B ( lock_B );
        std::stringstream intersection;

        for ( auto& it: A )
        {
            auto it_B = B.find ( it.first );
            if ( it_B != B.end ( ) )
            {
                intersection << it.first << ',' << it.second << ',' << it_B->second << std::endl;
            }
        }

        intersection << SUCCESS;
        return intersection.str ( );
    }

    std::string
    Database::anti_join   ( )
    {
        ReadLock w_lock_A ( lock_A );
        ReadLock w_lock_B ( lock_B );
        std::stringstream difference;

        auto it_A = A.begin ( );
        auto it_B = B.begin ( );

        while ( it_A != A.end ( ) or it_B != B.end ( ) )
        {
            if ( it_B == B.end ( ) or ( it_A != A.end ( ) and it_A->first < it_B->first ) )
            {
                difference << it_A->first << ',' << it_A->second << ',' << std::endl;
                ++it_A;
            }
            else if ( it_A == A.end ( ) or it_B->first < it_A->first )
            {
                difference << it_B->first << ",," << it_B->second << std::endl;
                ++it_B;
            }
            else
            {
                ++it_A; ++it_B;
            }
        }

        difference << SUCCESS;
        return difference.str ( );
    }

    std::string
    Database::truncate  ( const std::string& command )
    {
        auto& table_name = command;

        if ( ( table_name != TABLE_A_NAME and table_name != TABLE_B_NAME ) )
        {
            return std::string ( INCORRECT_TABLE_NAME + "\n" );
        }

        Table& update_table         = table_name == TABLE_A_NAME ? A : B;
        Lock&  update_table_lock    = table_name == TABLE_A_NAME ? lock_A : lock_B;

        {
            WriteLock w_lock ( update_table_lock );
            update_table.clear ( );
        }

        return SUCCESS;
    }

    int
    next_pos ( int& start_idx, int& separator_idx, const std::string& command )
    {
        while ( separator_idx < command.size ( ) and command[separator_idx] == SPACE_SEPARATOR )
        {
            ++separator_idx;
        }

        start_idx = separator_idx;

        if ( ( separator_idx = command.find ( SPACE_SEPARATOR, start_idx ) ) == std::string::npos )
        {
            return join::INCORRECT_SYNTAX;
        }

        return join::OK;
    }
}
