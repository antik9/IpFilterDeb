#include <fstream>
#include <queue>
#include <string>
#include <vector>

namespace map
{
    struct
    Compare
    {
        int
        operator() ( std::string& lhs, std::string& rhs )
        {
            return lhs > rhs;
        }
    };

    struct
    PairCompare
    {
        int
        operator() ( std::pair<std::string, int>& lhs, std::pair<std::string, int>& rhs )
        {
            return lhs.first > rhs.first;
        }
    };

    struct
    ReducedWord
    {
        std::string self;
        int prefix_length;
        int repeat;
    };

    using Container = std::priority_queue<std::string, std::vector<std::string>, Compare>;
    using PairContainer = std::priority_queue<
                                std::pair<std::string, int>,
                                std::vector<std::pair<std::string, int>>, PairCompare>;

    void
    map ( Container&, std::string&, std::ifstream::pos_type current, std::ifstream::pos_type next );

    void
    compute_index ( ReducedWord& previous, ReducedWord& next );

    void
    write_to_file ( int max_length, int reducer_idx );

    void
    shuffle ( std::vector<Container>&, std::vector<std::queue<std::string>>& );

    void reduce ( std::queue<std::string>&, int reducer_idx );
}
