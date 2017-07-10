/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 10 Jul 2017 10:27:59 AM CST
 Description: 
 ************************************************************************/
#ifndef PRODUCERCONSUMER_H
#define PRODUCERCONSUMER_H

#include "RingBuffer.h"
#include <condition_variable>
#include <mutex>

template <class T>
class Producer
{
    public:
        Producer<T>(RingBuffer<T> *queue, std::condition_variable *cv_producer,
                    std::condition_variable *cv_consumer, std::mutex *mutex);
        void Put(int count, T *inputs);
    private:
        RingBuffer<T> *queue_;
        std::condition_variable *cv_producer_;
        std::condition_variable *cv_consumer_;
        std::mutex *mutex_;
};

template <class T>
class Consumer
{
    public:
        Consumer<T>(RingBuffer<T> *queue, std::condition_variable *cv_producer,
                    std::condition_variable *cv_consumer, std::mutex *mutex);
        void Get(int count, T *outputs);
    private:
        RingBuffer<T> *queue_;
        std::condition_variable *cv_producer_;
        std::condition_variable *cv_consumer_;
        std::mutex *mutex_;
};

#endif
