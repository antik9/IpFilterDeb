#include <map>
#include <queue>
#include <vector>
#include <string>
#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>


namespace join
{
    const char          SPACE_SEPARATOR     = ' ';
    const std::string   TABLE_A_NAME        = "A";
    const std::string   TABLE_B_NAME        = "B";
    const int           INCORRECT_SYNTAX    = 1;
    const int           OK                  = 0;

    using Lock      = boost::shared_mutex;
    using WriteLock = boost::unique_lock< Lock >;
    using ReadLock  = boost::shared_lock< Lock >;

    class Database
    {
    public:
        using Table     = std::map<int, std::string>;

        Database ( )    = default;

        std::string
        insert      ( const std::string& );

        std::string
        inner_join  ( );

        std::string
        anti_join   ( );

        std::string
        truncate    ( const std::string& );

    private:
        Table       A;
        Table       B;
        Lock        lock_A;
        Lock        lock_B;
    };

    int
    next_pos ( int& start_idx, int& separator_idx, const std::string& command );
}
