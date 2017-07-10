/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 10 Jul 2017 10:29:28 AM CST
 Description: 
 ************************************************************************/
#include "ProducerConsumer.h"

/* Producer */

template <class T>
Producer<T>::Producer(RingBuffer<T> *queue, std::condition_variable *cv_producer,
                      std::condition_variable *cv_consumer, std::mutex *mutex):
    queue_(queue),
    cv_producer_(cv_producer),
    cv_consumer_(cv_consumer),
    mutex_(mutex)
{}

template <class T>
void Producer<T>::Put(int count, T *inputs)
{
    std::unique_lock<std::mutex> lock(*mutex_);
    cv_producer_->wait(lock, [&]{return (queue_->freeSpace() >= count);});
    queue_->write(count, inputs);
    cv_consumer_->notify_one(); // only notify consumer
}

/* Consumer */

template <class T>
Consumer<T>::Consumer(RingBuffer<T> *queue, std::condition_variable *cv_producer,
                      std::condition_variable *cv_consumer, std::mutex *mutex):
    queue_(queue),
    cv_producer_(cv_producer),
    cv_consumer_(cv_consumer),
    mutex_(mutex)
{}

template <class T>
void Consumer<T>::Get(int count, T *outputs)
{
    std::unique_lock<std::mutex> lock(*mutex_);
    cv_consumer_->wait(lock, [&]{return (queue_->busySpace() >= count);});
    queue_->read(count, outputs);
    cv_producer_->notify_one(); // only notify producer
}
