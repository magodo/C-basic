/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sun 28 May 2017 04:50:34 PM CST
 Description: 
 ************************************************************************/
#include "ManagerWorker.h"

/* WORKER */

// C'tor
Worker::Worker():
    is_running(false),
    is_init(false),
    thread(),
    name_(),
    manager_(nullptr)
{}

Worker::Worker(std::string name):
    is_running(false),
    is_init(false),
    thread(),
    name_(name),
    manager_(nullptr)
{}

// Move assignment operator
Worker& Worker::operator=(Worker&& other)
{
    if (&other != this)
    {
        // other->this
        is_running = other.is_running;
        is_init = other.is_init;
        thread = std::move(other.thread);
        name_ = std::move(other.name_);
        manager_ = other.manager_;
        
        // reset other
        other.manager_ = nullptr;
    }
    return *this;
}

// Move C'tor
Worker::Worker(Worker&& other)
{
    // other->this
    is_running = other.is_running;
    is_init = other.is_init;
    thread = std::move(other.thread);
    name_ = std::move(other.name_);
    manager_ = other.manager_;

    // reset other
    other.manager_ = nullptr;
}

Worker::~Worker(){}

void Worker::init()
{
    std::cout << "Worker: init..." << std::endl;
}

void Worker::deinit()
{
    std::cout << "Worker: deinit..." << std::endl;
}

void Worker::run(bool b)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "Worker: running..." << std::endl;
}

void Worker::stop()
{
    std::cout << "Worker: stop..." << std::endl;
}

void Worker::setManager(Manager *manager)
{ manager_ = manager;}

std::mutex& Worker::getLockReq() {return manager_->lock_req_;}
std::mutex& Worker::getLockResp() {return manager_->lock_resp_;}
std::condition_variable& Worker::getCvReq() {return manager_->cv_req_;}
std::condition_variable& Worker::getCvResp() {return manager_->cv_resp_;}

std::string Worker::getName()
{
    return name_;;
}

/* MANAGER */

void Manager::attachWorker(Worker&& worker)
{
    worker.setManager(this);
    named_workers.emplace(worker.getName(),std::move(worker));
}

void Manager::detachWorker(std::string name)
{
    auto it = named_workers.find(name);
    if (it != named_workers.end())
    {
        /* deinit worker if it is inited */
        if (it->second.is_init)
            deinitWorker(name);
        named_workers.erase(it);
    }
}

void Manager::startWorker(std::string name)
{
    /* Check existence of the thread */
    if (named_workers.find(name) == named_workers.end())
        return;

    Worker &worker = named_workers[name];

    if (!worker.is_init || worker.is_running)
        return;

    /* Lock response mutex first */
    std::unique_lock<std::mutex> locker(lock_resp_);

    /* Change state and notify worker */
    {
        std::lock_guard<std::mutex> locker(lock_req_);
        worker.is_running = true;
    }
    cv_req_.notify_one();
    

    /* Relase response mutex here and wait.
     * This is to avoid worker notify before manager wait here. */
    cv_resp_.wait(locker);
}

void Manager::stopWorker(std::string name)
{
    /* Check existence of the thread */
    if (named_workers.find(name) == named_workers.end())
        return;

    Worker &worker = named_workers[name];

    if (!worker.is_running)
        return;

    // change worker state and wait notification
    {
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker.is_running = false;
        cv_resp_.wait(locker);
    }
}

void Manager::deinitWorker(std::string name)
{
    /* Check existence of the thread */
    if (named_workers.find(name) == named_workers.end())
        return;

    Worker &worker = named_workers[name];

    if (!worker.is_init)
        return;
    {
        // change worker state and wait notification
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker.is_init = false;
        /* If worker is not runing, it might be in one of following states:
         *
         * 1. going to wait for start or deinit but not yet
         * 2. already wating for start or deinit
         *
         * For case 1, a notification will do nothing.
         * For case 2, a notification is mandatory to wake up worker. */
        if (!worker.is_running)
        {
            // notify worker
            cv_req_.notify_one();
        }
        else
        {
            worker.is_running = false;
        }
        cv_resp_.wait(locker);
    }

    // wait for finish
    worker.thread.join();
}

void Manager::initWorker(std::string name)
{
    /* Check existence of the thread */
    if (named_workers.find(name) == named_workers.end())
        return;

    Worker &worker = named_workers[name];

    if (worker.is_init)
        return;

    /* new thread */
    worker.thread = std::thread(newThread, std::ref(worker));
    
    /* wait for notification */
    {
        std::unique_lock<std::mutex> locker(lock_resp_);
        worker.is_init = true;
        cv_resp_.wait(locker);
    }

}

void Manager::newThread(Worker &worker)
{
    worker.init();

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
            worker.run(true);
        }
        if (!worker.is_init)
            break;

        worker.stop();

        /* Notify finish of stopping */
        {
            /* Because manager always lock response mutex before worker lock here,
             * this guarantees that manager always wait before worker notify.*/
            std::lock_guard<std::mutex> locker(worker.getLockResp());
        }
        worker.getCvResp().notify_one();
    }

    worker.deinit();

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

