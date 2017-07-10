/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 10 Jul 2017 10:09:57 AM CST
 Description: 
 ************************************************************************/

#include "ProducerConsumerFactory.h"

/* Factory */

template <class T>
RingBuffer<T> ProducerConsumerFactory<T>::queue_(QUEUE_LENGTH);

template <class T>
std::condition_variable ProducerConsumerFactory<T>::cv_producer_;

template <class T>
std::condition_variable ProducerConsumerFactory<T>::cv_consumer_;

template <class T>
std::mutex ProducerConsumerFactory<T>::mutex_;

template <class T>
Producer<T> *ProducerConsumerFactory<T>::NewProducer()
{
    return new Producer<T>(&queue_, &cv_producer_, &cv_consumer_, &mutex_);
}

template <class T>
Consumer<T> *ProducerConsumerFactory<T>::NewConsumer()
{
    return new Consumer<T>(&queue_, &cv_producer_, &cv_consumer_, &mutex_);
}
