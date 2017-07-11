/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Mon 10 Jul 2017 03:43:38 PM CST
 Description: 
 ************************************************************************/

#include "ProducerConsumerFactory.h"

#include <cstdlib>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>

std::mutex mutex;
std::condition_variable cv_consumer;
std::condition_variable cv_producer;
bool new_request = false;
bool to_exit = false;

void ConsumerThread(int id)
{
    Consumer<int> *ptr_consumer = ProducerConsumerFactory<int>::NewConsumer();
    int output;

    while (!to_exit)
    {
        {
            std::unique_lock<std::mutex> lock_req(mutex);
            /* wait for user new get request or exit */
            cv_consumer.wait(lock_req, [&]{return ((new_request == true) || (to_exit));}) ;
            new_request = false;
        }

        if (to_exit)
            return;

        ptr_consumer->Get(1, &output);
        std::cout << "Consumer[" << id << "] : get output: " << output << std::endl;
    }
}

void ProducerThread(int id)
{
    Producer<int> *ptr_producer = ProducerConsumerFactory<int>::NewProducer();
    int input;

    while (!to_exit)
    {
        {
            std::unique_lock<std::mutex> lock_req(mutex);
            /* wait for user new put request or exit */
            cv_producer.wait(lock_req, [&]{return ((new_request == true) || (to_exit));}) ;
            new_request = false;
        }

        if (to_exit)
            return;

        std::srand(std::time(nullptr));
        input = std::rand();
        ptr_producer->Put(1, &input);
        std::cout << "Producer[" << id << "] : put input: " << input << std::endl;
    }
}

int main()
{
    int choice;

    std::thread consumer1(ConsumerThread, 1);
    std::thread consumer2(ConsumerThread, 2);
    std::thread producer1(ProducerThread, 1);
    std::thread producer2(ProducerThread, 2);

    do
    {
        std::cout << "Supported actions:\n0. Quit(make sure consumer/producer are not suspended because of queue empty/full)\n1. Random Producer Put\n2. Random Consumer Read\nPlease enter number of your request:" << std::endl;
        std::cin >> choice;

        switch (choice)
        {
            case 0:
                {
                    std::lock_guard<std::mutex> lock_req(mutex);
                    to_exit = true;
                }
                cv_producer.notify_all();
                cv_consumer.notify_all();
                break;

            case 1:
                {
                    std::lock_guard<std::mutex> lock_req(mutex);
                    new_request = 1;
                }
                cv_producer.notify_one();
                break;

            case 2:
                {
                    std::lock_guard<std::mutex> lock_req(mutex);
                    new_request = 1;
                }

                cv_consumer.notify_one();
                break;
        }
    }while (choice != 0);
}
