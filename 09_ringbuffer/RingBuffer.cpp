/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 12:31:23 AM CST
 Description: 
 ************************************************************************/

#include "RingBuffer.h"

template<class T>
RingBuffer<T>::RingBuffer(int size):
    read_idx_(0),
    write_idx_(0),
    free_space_(size)
{
    buf_.resize(size);
}

template<class T>
bool RingBuffer<T>::read(T *output, int n)
{
    std::lock_guard<std::mutex> locker(lock_);

    /* check if enough data to read */
    if (n > getBusySpace())
        return false;

    /* read something */
    for (int i = 0; i < n; ++i)
    {
        output[i] = buf_[read_idx_];
        read_idx_ = incIdx(read_idx_, 1);
    }

    /* track free space */
    free_space_ += n;

    return true;
}

template<class T>
bool RingBuffer<T>::read_nopop(T *output, int n)
{
    std::lock_guard<std::mutex> locker(lock_);

    /* check if enough data to read */
    if (n > getBusySpace())
        return false;

    /* read something without update read index */
    for (int i = 0; i < n; ++i)
    {
        output[i] = buf_[incIdx(read_idx_, i)];
    }

    return true;
}

template<class T>
bool RingBuffer<T>::write(T *input, int n)
{
    std::lock_guard<std::mutex> locker(lock_);

    /* check if enough space to write */
    if (n > free_space_)
        return false;
    /* write something and update write index */
    for (int i = 0; i < n; ++i)
    {
        buf_[write_idx_] = input[i];
        write_idx_ = incIdx(write_idx_, 1);
    }

    /* track free space */
    free_space_ -= n;

    return true;
}


template<class T>
int RingBuffer<T>::incIdx(int index, int n)
{
    std::lock_guard<std::mutex> locker(lock_);

    return (index+n)%buf_.size();
}

template<class T>
int RingBuffer<T>::getFreeSpace()
{
    //std::lock_guard<std::mutex> locker(lock_);
    return free_space_;
}

template<class T>
int RingBuffer<T>::getBusySpace()
{
    //std::lock_guard<std::mutex> locker(lock_);
    return (buf_.size() - free_space_);
}

