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
#include <map>

class Manager;

class Worker
{
    public:
        bool is_running;
        bool is_init;
        std::thread thread;
    public:
        Worker();
        Worker(std::string name);
        Worker(Worker&& other);
        Worker& operator=(Worker&& other);

        void setManager(Manager *manager);
        std::mutex& getLockReq(); 
        std::mutex& getLockResp(); 
        std::condition_variable& getCvReq(); 
        std::condition_variable& getCvResp(); 

    private:
        std::string name_;
        Manager *manager_;
};

class Manager
{
    friend class Worker;

    public:
        void attachWorker(std::string name);
        void detachWorker(std::string name);

        void stopWorker(std::string name);
        void startWorker(std::string name);
        void deinitWorker(std::string name);
        void initWorker(std::string name);

    private:
        static void newThread(Worker &worker);

    private:

        std::map<std::string, Worker> named_workers;

        /* mutex and condition variable for sync between workers */
        std::mutex lock_req_, lock_resp_;
        std::condition_variable cv_req_, cv_resp_;
};


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


void Worker::setManager(Manager *manager)
{ manager_ = manager;}

std::mutex& Worker::getLockReq() {return manager_->lock_req_;}
std::mutex& Worker::getLockResp() {return manager_->lock_resp_;}
std::condition_variable& Worker::getCvReq() {return manager_->cv_req_;}
std::condition_variable& Worker::getCvResp() {return manager_->cv_resp_;}

/* MANAGER */

void Manager::attachWorker(std::string name)
{
    Worker worker(name);
    worker.setManager(this);
    named_workers[name] = std::move(worker);
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

/* MAIN */

int main()
{
    Manager manager;
    int choice;
    std::string name;
    bool to_quit = false;

    while (!to_quit)
    {
        std::cout << "0. New worker\n1. Delete worker\n2. Init worker\n3. Start worker\n4. Stop worker\n5. Deinit worker\n6. Quit\nEnter the number of what you want to do: ";
        std::cin >> choice;
        if (choice != 6)
        {
            std::cout << "Thread name: ";
            std::cin >> name;
        }
        
        switch (choice)
        {
            case 0:
                manager.attachWorker(name);
                break;
            case 1:
                manager.detachWorker(name);
                break;
            case 2:
                manager.initWorker(name);
                break;
            case 3:
                manager.startWorker(name);
                break;
            case 4:
                manager.stopWorker(name);
                break;
            case 5:
                manager.deinitWorker(name);
                break;
            case 6:
                to_quit = true;
                break;
            default:
                std::cerr << "???" << std::endl;
                break;
        }
    }
    return 0;
}
