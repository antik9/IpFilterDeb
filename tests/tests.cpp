#include <boost/concept_check.hpp>

#include <gtest/gtest.h>
#include <iterator>
#include <list>
#include <queue>
#include <string>
#include <vector>

#include "../src/concepts.hpp"


TEST(TestConcept, vector)
{
    BOOST_CONCEPT_ASSERT((
        concepts::RandomAccessIterator<decltype(std::vector<int>().begin())>
    ));
}

/* No match for operator< ... */

// TEST(TestConcept, vector_fiterator)
// {
//     BOOST_CONCEPT_ASSERT((
//         concepts::RandomAccessIterator<std::iterator<std::forward_iterator_tag, int>>
//     ));
// }

TEST(TestConcept, deque)
{
    BOOST_CONCEPT_ASSERT((
        concepts::RandomAccessIterator<decltype(std::deque<int>().begin())>
    ));
}

/* No match for operator< */

// TEST(TestConcept, list)
// {
//     BOOST_CONCEPT_ASSERT((
//         concepts::RandomAccessIterator<decltype(std::list<int>().begin())>
//     ));
// }

TEST(TestConcept, string)
{
    BOOST_CONCEPT_ASSERT((
        concepts::RandomAccessIterator<decltype(std::string().begin())>
    ));
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
