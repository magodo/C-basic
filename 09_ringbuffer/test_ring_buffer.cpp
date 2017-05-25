/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:48:45 AM CST
 Description: 
 ************************************************************************/

#include "gtest/gtest.h"
#include "RingBuffer.h"

TEST(RingBuffer, NoFreeSpaceToWrite)
{
    RingBuffer ring_buffer(5);

    // write in something
    EXPECT_TRUE(ring_buffer.updateWriteIdx(3));

    // read something
    EXPECT_TRUE(ring_buffer.updateReadIdx(3));
    
    // write in until full
    EXPECT_TRUE(ring_buffer.updateWriteIdx(5));
    
    // write something
    EXPECT_FALSE(ring_buffer.updateWriteIdx(1));

}

TEST(RingBuffer, NothingToRead)
{
    RingBuffer ring_buffer(5);
    
    // there is nothing at first
    EXPECT_FALSE(ring_buffer.updateReadIdx(1));
    
    // write in something
    EXPECT_TRUE(ring_buffer.updateWriteIdx(3));
    
    // read something
    EXPECT_FALSE(ring_buffer.updateReadIdx(4));

}
