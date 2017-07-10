/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 10 Jul 2017 09:40:37 AM CST
 Description: 
 ************************************************************************/

#ifndef PRODUCERCONSUMERFACTORY_H
#define PRODUCERCONSUMERFACTORY_H

#include "ProducerConsumer.h"
#include "RingBuffer.h"
#include <condition_variable>
#include <mutex>

#define QUEUE_LENGTH 2

template <class T>
class ProducerConsumerFactory
{
    public:

        static Producer<T> *NewProducer();
        static Consumer<T> *NewConsumer();

    private:

        static RingBuffer<T> queue_;
        static std::condition_variable cv_producer_, cv_consumer_;
        static std::mutex mutex_;
};

#endif
