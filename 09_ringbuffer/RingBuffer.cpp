/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:31:23 AM CST
 Description: 
 ************************************************************************/

#include "RingBuffer.h"

RingBuffer::RingBuffer(int size):
    buf_size_(size),
    read_idx_(0),
    write_idx_(0),
    free_space_(buf_size_)
{}

bool RingBuffer::updateReadIdx(int stride)
{
    if (stride > (buf_size_ - free_space_))
        return false;
    read_idx_ = incIdx(read_idx_, stride);
    free_space_ += stride;
    return true;
}

bool RingBuffer::updateWriteIdx(int stride)
{
    if (stride > free_space_)
        return false;
    write_idx_ = incIdx(write_idx_, stride);
    free_space_ -= stride;
    return true;
}

int RingBuffer::incIdx(int index, int stride)
{
    return (index+stride)%buf_size_;
}
