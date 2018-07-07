#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "ip_filter.h"

string_vector split(const std::string &str, char delim);

TEST(SplitTest, splitsByDots) {
    string_vector result_test_1 = split("12.4.11.19", '.');
    ASSERT_EQ(result_test_1.size(), 4);
    ASSERT_THAT (
            result_test_1, 
            testing::ElementsAreArray({"12", "4", "11", "19"})); 
}


TEST(SplitTest, splitsByColons) {
    string_vector result_test_1 = split("2:123:12:5:56:2", ':');
    ASSERT_EQ(result_test_1.size(), 6);
    ASSERT_THAT (
            result_test_1, 
            testing::ElementsAreArray({"2", "123", "12", "5", "56", "2"})); 
}

class TestPrintIp : public ::testing::Test
{
public:
    std::vector<string_vector> test_ips {
            {"12", "15", "16", "90"},
            {"90", "154", "67", "255"},
            {"1", "45", "43", "46"},
            {"46", "70", "100", "100"},
            {"1", "45", "43", "47"}
    };
};

TEST_F(TestPrintIp, printAll) {
    testing::internal::CaptureStdout();
    print_ip_pool_by_filter(test_ips, 
            [](const string_vector &ip_address) {return true;});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, 
            "12.15.16.90\n"
            "90.154.67.255\n"
            "1.45.43.46\n"
            "46.70.100.100\n"
            "1.45.43.47\n");
}

TEST_F(TestPrintIp, printOnlyWith1AtFirstPosition) {
    testing::internal::CaptureStdout();
    print_ip_pool_by_filter(test_ips, 
            [](const string_vector &ip_address) {return ip_address[0] == "1";});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, 
            "1.45.43.46\n"
            "1.45.43.47\n");
}

TEST_F(TestPrintIp, print_46_70) {
    testing::internal::CaptureStdout();
    print_ip_pool_by_filter(test_ips, 
            [](const string_vector &ip_address) {
                return ip_address[0] == "46" && ip_address[1] == "70";});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, 
            "46.70.100.100\n" );
}

TEST_F(TestPrintIp, printWith46) {
    testing::internal::CaptureStdout();
    print_ip_pool_by_filter(test_ips, 
            [](const string_vector &ip_address) {
                return ip_address[0] == "46" || ip_address[1] == "46"
                || ip_address[2] == "46" || ip_address[3] == "46";});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ( output, 
            "1.45.43.46\n"
            "46.70.100.100\n");
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
