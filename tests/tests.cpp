#include <gtest/gtest.h>
#include <dirent.h>
#include "../src/map_reduce.h"

class TestMapReduce: public ::testing::Test
{
protected:
    void
    TearDown ( )
    {
        DIR* test_dir = opendir(".");
        struct dirent* file_;
        std::string LOG_END_PATTERN     = "xx.txt";

        while ( (file_ = readdir (test_dir)) != NULL )
        {
            std::string filename = file_->d_name;
            if ( filename.size ( ) > 6 and
                    LOG_END_PATTERN.compare ( filename.substr( filename.size ( ) - 6 ) ) == 0 )
            {
                std::cout << "rm " << filename << std::endl;
                std::remove ( filename.c_str ( ) );
            }
        }

        closedir(test_dir);
    }
};

TEST_F ( TestMapReduce, compute_index_1 )
{
    map::ReducedWord word_1 { "hello", 0, 1 };
    map::ReducedWord word_2 { "hi", 0, 1 };

    map::compute_index ( word_1, word_2 );

    ASSERT_EQ ( word_1.prefix_length, 2 );
    ASSERT_EQ ( word_2.prefix_length, 2 );
}

TEST_F ( TestMapReduce, compute_index_2 )
{
    map::ReducedWord word_1 { "yes", 0, 1 };
    map::ReducedWord word_2 { "no", 0, 1 };

    map::compute_index ( word_1, word_2 );

    ASSERT_EQ ( word_1.prefix_length, 1 );
    ASSERT_EQ ( word_2.prefix_length, 1 );
}

TEST_F ( TestMapReduce, compute_index_3 )
{
    map::ReducedWord word_1 { "yesyesno", 0, 1 };
    map::ReducedWord word_2 { "yesyesyes", 0, 1 };

    map::compute_index ( word_1, word_2 );

    ASSERT_EQ ( word_1.prefix_length, 7 );
    ASSERT_EQ ( word_2.prefix_length, 7 );
}

TEST_F ( TestMapReduce, reduce )
{
    std::queue<std::string> not_reduced_words;
    not_reduced_words.emplace ( "max_xx" );
    not_reduced_words.emplace ( "min_xx" );
    not_reduced_words.emplace ( "sort_xx" );
    not_reduced_words.emplace ( "sort_xx" );
    not_reduced_words.emplace ( "tail_xx" );
    not_reduced_words.emplace ( "take_xx" );

    map::reduce ( not_reduced_words );

    std::ifstream in ( "max_xx.txt" );
    std::string respond;
    in >> respond;

    ASSERT_EQ ( respond,
        "3"
    );
}


int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
