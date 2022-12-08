#include <iostream>
// #include "src/TcpServer.h"
#include "../webssh/InetAddress.h"
#include "../webssh/Socket.h"
#include "../webssh/util.h"
#include "../webssh/TcpServer.h"
#include <gtest/gtest.h>

using namespace std;

TEST(InetAddressTest, testToPort)
{
    InetAddress addr(12345);
    // Test a port number in the valid range (1-65535)
    {  
        std::string expected = "12345";
        std::string actual = addr.toPort();
        ASSERT_EQ(expected, actual);
    }
    
    {
        ASSERT_EQ(addr.toIpPort(), "[0.0.0.0]:12345");
    }
}

// TEST(InetAdressTest, toIp)
// {
//     {
//         InetAddress addr(12345);
//         std::string expected = "0.0.0.0";
//         std::string actual = addr.toIp();
//         std::cerr << actual << endl;
//         ASSERT_EQ(expected, actual);
//     }
// }


int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}