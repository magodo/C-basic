/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:48:45 AM CST
 Description: 
 ************************************************************************/

#include "gtest/gtest.h"
#include "RingBuffer.h"
#include <iostream>
using namespace std;

TEST(RingBuffer, NoFreeSpaceToWrite)
{
    RingBuffer<int> ring_buffer(5);

    // write in something
    int input[3] = {1,2,3};
    EXPECT_TRUE(ring_buffer.write(3, input));

    // read something
    int output[3];
    EXPECT_TRUE(ring_buffer.read(3, output));
    for (int i = 0; i < 3; i++)
        EXPECT_TRUE(input[i] == output[i]);
    
    // write in something
    EXPECT_TRUE(ring_buffer.write(3, input));
    
    // write something again, exceed limit
    EXPECT_FALSE(ring_buffer.write(3, input));

}

TEST(RingBuffer, NothingToRead)
{
    RingBuffer<int> ring_buffer(5);
    
    // there is nothing at first
    int output[3];
    EXPECT_FALSE(ring_buffer.read(3, output));
    
    // write in something
    int input[2];
    EXPECT_TRUE(ring_buffer.write(2, input));
    
    // read more
    EXPECT_FALSE(ring_buffer.read(3, output));

}

TEST(RingBuffer, PutGetCorrect)
{
    RingBuffer<int> ring_buffer(6);

    int input = 0;
    int output = -1;

    ring_buffer.write(1, &input);
    ring_buffer.read_nopop(0, 1, &output);
    EXPECT_TRUE(0 == output);
    ring_buffer.read(1, &output);
    EXPECT_TRUE(0 == output);

    input = 1;

    ring_buffer.write(1, &input);
    ring_buffer.read_nopop(0, 1, &output);
    EXPECT_TRUE(1 == output);
    ring_buffer.read(1, &output);
    EXPECT_TRUE(1 == output);

}
