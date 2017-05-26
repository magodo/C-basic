/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:28:45 AM CST
 Description: 
 ************************************************************************/

#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__ 

#include <mutex>
#include <vector>

template<class T>
class RingBuffer
{
    public:
        RingBuffer(int size);
        bool read(T *output, int n);
        bool read_nopop(T *output, int n);
        bool write(T *input, int n);
        int getFreeSpace();
        int getBusySpace();

    private:
        int incIdx(int index, int n);
    private:
        std::vector<T> buf_;
        int read_idx_;      // where to read then(starts from 0)
        int write_idx_;     // where to write then(starts from 0)
        int free_space_;    // without this, can't track free space
        std::mutex lock_;   // protect simultaneous access
};

/************************
 * define template classes
 ***********************/

template class RingBuffer<int>;

#endif
