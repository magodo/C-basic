/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:28:45 AM CST
 Description: 
 ************************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__ 

#include <vector>

template <class T>
class RingBuffer
{
    public:
        RingBuffer();
        RingBuffer(int count);
        bool write(int count, T *inputs);
        bool read(int count, T *outputs);
        bool read_nopop(int offset, int count, T *outputs);
        inline int freeSpace() {return free_space_;}
        inline int busySpace() {return (buf_.size() - free_space_);}

    private:
        int incIdx(int index, int stride);

    private:
        std::vector<T> buf_;
        int read_idx_;  // starting from 0
        int write_idx_; // starting from 0
        int free_space_;
};

#endif
