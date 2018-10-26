#include <iostream>
#include <tuple>

#include "map_reduce.h"

namespace map
{
    void
    map ( Container& container, std::string& src, std::ifstream::pos_type current,
            std::ifstream::pos_type next )
    {
        std::string email;
        std::ifstream in ( src );
        in.seekg ( current );
        while ( true )
        {
            in >> email;
            current = in.tellg ( );
            if ( ( next != EOF and current < next and current != EOF )
                    or ( next == EOF and current != EOF ) )
            {
                container.emplace ( email );
            }
            else
            {
                break;
            }
        }
        in.close ( );
    }

    void
    shuffle ( std::vector<Container>& mapper_containers,
            std::vector<std::queue<std::string>>& reducer_containers )
    {
        auto base = reducer_containers.size ( );
        PairContainer stack_words;

        for ( auto i = 0; i < mapper_containers.size ( ); ++i )
        {
            if ( not mapper_containers[i].empty ( ) )
            {
                stack_words.emplace ( std::make_pair ( mapper_containers[i].top ( ), i ) );
                mapper_containers[i].pop ( );
            }
        }

        int counter1 = 0, counter2 = 0;

        while ( not stack_words.empty ( ) )
        {
            ++counter1;
            std::string next_word;
            int container_idx;
            std::tie ( next_word, container_idx ) = stack_words.top ( );
            stack_words.pop ( );

            if ( not next_word.empty ( ) )
            {
                ++counter2;
                auto idx = ( (int) next_word[0] ) % base;
                reducer_containers[idx].emplace ( next_word );
            }

            if ( not mapper_containers[container_idx].empty ( ) )
            {
                stack_words.emplace (
                    std::make_pair ( mapper_containers[container_idx].top ( ), container_idx ) );
                mapper_containers[container_idx].pop ( );
            }
        }
    }

    void
    compute_index ( ReducedWord& previous, ReducedWord& next )
    {
        size_t previous_size = previous.self.size ( ), next_size = next.self.size ( );
        size_t max_length = previous_size > next_size ? previous_size : next_size;
        size_t prefix_length = 0;
        for ( int i = 0; i < max_length; ++i )
        {
            if ( i == previous_size or i == next_size or previous.self[i] != next.self[i] )
            {
                prefix_length = i + 1;
                break;
            }
        }

        prefix_length = ( prefix_length == 0 ) ? max_length : prefix_length;

        previous.prefix_length = previous.prefix_length > prefix_length
                                    ? previous.prefix_length
                                    : prefix_length;

        previous.prefix_length = previous.prefix_length > previous_size
                                    ? previous_size
                                    : previous.prefix_length;

        next.prefix_length = next.prefix_length > prefix_length
                                    ? next.prefix_length
                                    : prefix_length;

        next.prefix_length = next.prefix_length > next_size
                                    ? next_size
                                    : next.prefix_length;
    }

    void
    write_to_file ( std::vector<ReducedWord>& reduced_words )
    {
        std::string filename = reduced_words[0].self + ".txt";
        std::ofstream out ( filename, std::ofstream::trunc );

        for ( auto& reduce_word: reduced_words )
        {
            for ( int i = 0; i < reduce_word.repeat; ++i )
            {
                out << reduce_word.prefix_length << std::endl;
            }
        }
        out.close ( );
    }

    void
    reduce ( std::queue<std::string>& reduce_container )
    {
        ReducedWord previous, next;
        std::vector<ReducedWord> reduced_words;
        std::string word;

        while ( not reduce_container.empty ( ) )
        {
            word = reduce_container.front ( );
            reduce_container.pop ( );
            if ( previous.self.empty ( ) )
            {
                previous = { word, 0, 1 };
            }
            else if ( word == previous.self )
            {
                ++previous.repeat;
            }
            else if ( next.self.empty ( ) )
            {
                next = { word, 0, 1 };
                compute_index ( previous, next );
                reduced_words.push_back ( previous );
            }
            else if ( next.self == word )
            {
                ++next.repeat;
            }
            else
            {
                previous = next;
                next = { word, 0, 1 };
                compute_index ( previous, next );
                reduced_words.push_back ( previous );
            }
        }

        if ( next.self.empty ( ) and not previous.self.empty ( ) )
        {
            reduced_words.push_back ( previous );
        }
        else if ( not next.self.empty ( ) )
        {
            reduced_words.push_back ( next );
        }

        if ( not reduced_words.empty ( ) )
        {
            write_to_file ( reduced_words );
        }
    }
}
