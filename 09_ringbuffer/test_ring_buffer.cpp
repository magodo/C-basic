/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:48:45 AM CST
 Description: 
 ************************************************************************/

#include "gtest/gtest.h"
#include "RingBuffer.h"

TEST(RingBuffer, NoFreeSpaceToWrite)
{
    RingBuffer<int> ring_buffer(5);

    // write in something
    int input[3] = {1,2,3};
    EXPECT_TRUE(ring_buffer.write(input, 3));

    // read something
    int output[3];
    EXPECT_TRUE(ring_buffer.read(output, 3));
    for (int i = 0; i < 3; ++i)
        EXPECT_TRUE(input[i] == output[i]);
    
    // write in until exceed
    EXPECT_TRUE(ring_buffer.write(input, 3));
    EXPECT_FALSE(ring_buffer.write(input, 3));

}

TEST(RingBuffer, NothingToRead)
{
    RingBuffer<int> ring_buffer(5);
    
    // there is nothing at first
    int output;
    EXPECT_FALSE(ring_buffer.read(&output, 1));
    
    // write in something
    int input[3] = {1,2,3};
    EXPECT_TRUE(ring_buffer.write(input, 3));
    
    // read too much
    int outputs[4];
    EXPECT_FALSE(ring_buffer.read(outputs, 4));

}
