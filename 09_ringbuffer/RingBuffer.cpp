/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:31:23 AM CST
 Description: 
 ************************************************************************/

#include "RingBuffer.h"

template<class T>
RingBuffer<T>::RingBuffer(int count):
    read_idx_(0),
    write_idx_(0),
    free_space_(count)
{
    buf_.resize(count);
}

template <class T>
bool RingBuffer<T>::write(int count, T *inputs)
{
    /* check if enough free space to hold inputs */
    if (count > free_space_)
        return false;

    /* write each input */
    for (int i = 0; i < count; ++i)
    {
        read_idx_ = incIdx(read_idx_, 1);
        buf_[read_idx_] = inputs[i];
    }

    /* update free space */
    free_space_ -= count;

    return true;
}

template <class T>
bool RingBuffer<T>::read(int count, T *outputs)
{
    /* check if enough data to be read */
    if (count > (buf_.size() - free_space_))
        return false;

    /* read data */
    for (int i = 0; i < count; ++i)
    {
        write_idx_ = incIdx(write_idx_, 1);
        outputs[i] = buf_[write_idx_];
    }

    /* update free space */

    free_space_ += count;

    return true;
}

template <class T>
int RingBuffer<T>::incIdx(int index, int stride)
{
    return (index+stride)%buf_.size();
}

/******************************************
 * Define all template class to use here
 ******************************************/
template class RingBuffer<int>;
