#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "../src/print_ip.hpp"

template<typename T>
struct false_struct 
{
};

TEST(IS_CONTAINER, list)
{
    bool is_container   = __is_container<std::list<float>>::value; 
    ASSERT_TRUE( is_container );
}

TEST(IS_CONTAINER, false_struct) 
{
    bool is_container   = __is_container<false_struct<int>>::value; 
    ASSERT_TRUE( !is_container );
}

TEST(IS_UNIFIED_TUPLE, unified) 
{
    bool unified        = __is_unified_tuple<int, int, int, int>::value;
    ASSERT_TRUE( unified );
}

TEST(IS_UNIFIED_TUPLE, not_unified) 
{
    bool unified        = __is_unified_tuple<int, float, long, char>::value;
    ASSERT_TRUE( !unified );
}

TEST(PRINT_IP, print_char) 
{
    testing::internal::CaptureStdout();
    print_ip((char) 200);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "200\n" );
}

TEST(PRINT_IP, print_short) 
{
    testing::internal::CaptureStdout();
    short octet_1 = 15, octet_2 = 45;
    print_ip((short) ((octet_1 << OCTAL) + octet_2));
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "15.45\n" );
}

TEST(PRINT_IP, print_int) 
{
    testing::internal::CaptureStdout();
    int octet_1 = 15, octet_2 = 45, octet_3 = 1, octet_4 = 178;
    print_ip((int) ((((((octet_1 << OCTAL) + octet_2) << OCTAL) + octet_3) << OCTAL) + octet_4));
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "15.45.1.178\n" );
}

TEST(PRINT_IP, print_string) 
{
    testing::internal::CaptureStdout();
    std::string ip_v("178.12.14.90");
    print_ip(ip_v, false);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, ip_v );
}

TEST(PRINT_IP, print_list)
{
    testing::internal::CaptureStdout();
    std::list<int> list_ip_v { 89, 11, 190, 255, 254 };
    print_ip(list_ip_v);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "89.11.190.255.254\n" );
}

TEST(PRINT_IP, print_vector)
{
    testing::internal::CaptureStdout();
    std::vector<int> vector_ip_v { 89, 11, 190, 255, 254 };
    print_ip(vector_ip_v);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "89.11.190.255.254\n" );
}

TEST(PRINT_IP, print_tuple)
{
    testing::internal::CaptureStdout();
    std::tuple<short, short, short> tuple_ip_v { 56, 190, 43 };
    print_ip(tuple_ip_v);
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, "56.190.43\n" );
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
