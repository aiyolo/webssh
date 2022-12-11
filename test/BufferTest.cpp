#include "../webssh/Buffer.h"
#include <gtest/gtest.h>

using namespace std;

TEST(BufferTest, testBuffer)
{
    Buffer buffer(1024);
    ASSERT_EQ(buffer.writableBytes(), 1024-8);
    ASSERT_EQ(buffer.readableBytes(), 0);
    ASSERT_EQ(buffer.size(), 1024);
    string str(100, 'a');
    buffer.append(str);
    ASSERT_EQ(buffer.readableBytes(), 100);
    ASSERT_EQ(buffer.writableBytes(), 1024-8-100);
    buffer.retrieve(20);
    ASSERT_EQ(buffer.readableBytes(), 100-20);
    ASSERT_EQ(buffer.writableBytes(), 1024-8-100);
    buffer.ensureWritableBytes(2048);
    ASSERT_EQ(buffer.writableBytes(), 2048);
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}