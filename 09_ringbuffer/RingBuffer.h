/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:28:45 AM CST
 Description: 
 ************************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__ 

class RingBuffer
{
    public:
        RingBuffer(int size);
        bool updateReadIdx(int stride);
        bool updateWriteIdx(int stride);

    private:
        int incIdx(int index, int stride);
        const int buf_size_;
        int read_idx_;  // starting from 0
        int write_idx_; // starting from 0
        int free_space_;
};

#endif
