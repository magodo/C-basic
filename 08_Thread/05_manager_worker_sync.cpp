/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sun 28 May 2017 04:50:34 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

class Manager;

class Worker
{
    public:
        Worker(Manager *manager);
        bool is_running;
        bool is_init;
        std::mutex& getLockReq(); 
        std::mutex& getLockResp(); 
        std::condition_variable& getCvReq(); 
        std::condition_variable& getCvResp(); 

    private:
        Manager *manager_;
};

class Manager
{
    friend class Worker;

    public:
        Manager();
        void stopWorker();
        void startWorker();
        void deinitWorker();
        void initWorker();

    private:
        static void newThread(Worker &worker);

    private:
        Worker worker_;
        std::thread thread_worker_;

        /* mutex and condition variable for sync between workers */
        std::mutex lock_req_, lock_resp_;
        std::condition_variable cv_req_, cv_resp_;
};


Worker::Worker(Manager *mgr):
    is_running(false),
    is_init(false),
    manager_(mgr)
{}

std::mutex& Worker::getLockReq() {return manager_->lock_req_;}
std::mutex& Worker::getLockResp() {return manager_->lock_resp_;}
std::condition_variable& Worker::getCvReq() {return manager_->cv_req_;}
std::condition_variable& Worker::getCvResp() {return manager_->cv_resp_;}

Manager::Manager():
    worker_(this)
{}


void Manager::startWorker()
{
    if (!worker_.is_init || worker_.is_running)
        return;

    /* Lock response mutex first */
    std::unique_lock<std::mutex> locker(lock_resp_);

    /* Change state and notify worker */
    {
        std::lock_guard<std::mutex> locker(lock_req_);
        worker_.is_running = true;
    }
    cv_req_.notify_one();
    

    /* Relase response mutex here and wait.
     * This is to avoid worker notify before manager wait here. */
    cv_resp_.wait(locker);
}

void Manager::stopWorker()
{
    if (!worker_.is_running)
        return;

    // change worker state and wait notification
    {
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker_.is_running = false;
        cv_resp_.wait(locker);
    }
}

void Manager::deinitWorker()
{
    if (!worker_.is_init)
        return;
    {
        // change worker state and wait notification
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker_.is_init = false;
        /* If worker is not runing, it might be in one of following states:
         *
         * 1. going to wait for start or deinit but not yet
         * 2. already wating for start or deinit
         *
         * For case 1, a notification will do nothing.
         * For case 2, a notification is mandatory to wake up worker. */
        if (!worker_.is_running)
        {
            // notify worker
            cv_req_.notify_one();
        }
        else
        {
            worker_.is_running = false;
        }
        cv_resp_.wait(locker);
    }

    // wait for finish
    thread_worker_.join();
}

void Manager::initWorker()
{
    if (worker_.is_init)
        return;

    /* new thread */
    thread_worker_ = std::thread(newThread, std::ref(worker_));
    
    /* wait for notification */
    {
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker_.is_init = true;
        cv_resp_.wait(locker);
    }

}

void Manager::newThread(Worker &worker)
{
    std::cout << "Worker: initiating..." << std::endl;

    /* Notify manager finish of initiating */
    {
        std::lock_guard<std::mutex> locker(worker.getLockResp());
    }
    std::cout << "Worker: initiate done" << std::endl;
    worker.getCvResp().notify_one();

    while (worker.is_init)
    {
        /* Wait for start of running */
        {
            /* If worker reach here first, then it will block and wait for following two situation:
             * 1. manager change worker to run
             * 2. manager change worker to deinit
             *
             * If manager change state before worker reach here, then no block here.*/
            std::unique_lock<std::mutex> locker(worker.getLockReq());
            std::cout << "Worker: waiting for run..." << std::endl;
            worker.getCvReq().wait(locker, [&worker]{return (worker.is_running || !worker.is_init);});
        }
        if (!worker.is_init)
            break;

        std::cout << "Worker: start running..." << std::endl;

        /* Notify finish of starting running */
        {
            /* Because manager always lock response mutex before worker lock here,
             * this guarantees that manager always wait before worker notify.*/
            std::lock_guard<std::mutex> locker(worker.getLockResp());
        }
        std::cout << "Worker: finish starting" << std::endl;
        worker.getCvResp().notify_one();

        /* Running */

        while (worker.is_running && worker.is_init)
        {
            std::cout << "Worker: doing something..." << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        if (!worker.is_init)
            break;

        std::cout << "Worker: stopping..." << std::endl;

        /* Notify finish of stopping */
        {
            /* Because manager always lock response mutex before worker lock here,
             * this guarantees that manager always wait before worker notify.*/
            std::lock_guard<std::mutex> locker(worker.getLockResp());
        }
        worker.getCvResp().notify_one();
    }

    std::cout << "Worker: deinitiating..." << std::endl;

    /* Notify finish of  deinitiate */
    {
        /* Because manager always lock response mutex before worker lock here,
         * this guarantees that manager always wait before worker notify.*/
        std::lock_guard<std::mutex> locker(worker.getLockResp());
    }
    std::cout << "Worker: deinitiate done" << std::endl;
    worker.getCvResp().notify_one();

    return;
}

int main()
{
    Manager manager;
    int choice;
    bool to_quit = false;

    while (!to_quit)
    {
        std::cout << "1. Init worker\n2. Start worker\n3. Stop worker\n4. Deinit worker\n5. Quit\nEnter the number of what you want to do: ";
        std::cin >> choice;
        switch (choice)
        {
            case 1:
                manager.initWorker();
                break;
            case 2:
                manager.startWorker();
                break;
            case 3:
                manager.stopWorker();
                break;
            case 4:
                manager.deinitWorker();
                break;
            case 5:
                to_quit = true;
                break;
            default:
                std::cerr << "???" << std::endl;
                break;
        }
    }
    return 0;
}
