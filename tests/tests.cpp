#include <gtest/gtest.h>
#include "../src/database.h"

class TestDatabase: public ::testing::Test
{
protected:
    join::Database database;
};

TEST_F ( TestDatabase, insert_check )
{
    ASSERT_EQ( database.insert ( "A 10 xxx\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 10 yyy\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "A 10 zzz\n" ),
            "ERR duplicate 10\n"
    );

    ASSERT_EQ( database.insert ( "A 1\n" ),
            "Check Syntax\n"
    );

    ASSERT_EQ( database.insert ( "A 1 uuu ppp\n" ),
            "Check Syntax\n"
    );

    ASSERT_EQ( database.insert ( "CXX 10 ccc\n" ),
            "Incorrect table name CXX\n"
    );
}

TEST_F ( TestDatabase, intersection )
{
    ASSERT_EQ( database.insert ( "A 10 xxx\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 10 yyy\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.inner_join ( ),
            "10,xxx,yyy\n"
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "A 1 uuu\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 2 ccc\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "A 2 ggg\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.inner_join ( ),
            "2,ggg,ccc\n"
            "10,xxx,yyy\n"
            "OK\n"
    );
}

TEST_F ( TestDatabase, symmetric_difference )
{
    ASSERT_EQ( database.insert ( "A 10 xxx\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 10 yyy\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.anti_join ( ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "A 1 uuu\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 2 ccc\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.anti_join ( ),
            "1,uuu,\n"
            "2,,ccc\n"
            "OK\n"
    );
}

TEST_F ( TestDatabase, truncate )
{
    ASSERT_EQ( database.insert ( "A 10 xxx\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 10 yyy\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.inner_join ( ),
            "10,xxx,yyy\n"
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "A 1 uuu\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.insert ( "B 2 ccc\n" ),
            "OK\n"
    );

    ASSERT_EQ( database.anti_join ( ),
            "1,uuu,\n"
            "2,,ccc\n"
            "OK\n"
    );

    ASSERT_EQ( database.truncate ( "A" ),
            "OK\n"
    );

    ASSERT_EQ( database.inner_join ( ),
            "OK\n"
    );

    ASSERT_EQ( database.anti_join ( ),
            "2,,ccc\n"
            "10,,yyy\n"
            "OK\n"
    );

    ASSERT_EQ( database.truncate ( "B" ),
            "OK\n"
    );

    ASSERT_EQ( database.anti_join ( ),
            "OK\n"
    );
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
