/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 31 May 2017 11:38:40 AM CST
 Description: 
 ************************************************************************/
#ifndef __MANAGERWORKER_H__
#define __MANAGERWORKER_H__

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

        virtual ~Worker();
        virtual void init();
        virtual void deinit();
        virtual void run();
        virtual void stop();

        void setManager(Manager *manager);
        std::mutex& getLockReq(); 
        std::mutex& getLockResp(); 
        std::condition_variable& getCvReq(); 
        std::condition_variable& getCvResp(); 
        std::string getName();

    private:
        std::string name_;
        Manager *manager_;
};

class Manager
{
    friend class Worker;

    public:
        void attachWorker(std::unique_ptr<Worker> p_worker);
        void detachWorker(std::string name);

        void stopWorker(std::string name);
        void startWorker(std::string name);
        void deinitWorker(std::string name);
        void initWorker(std::string name);

    private:
        static void newThread(Worker &worker);

    private:

        std::map<std::string, std::unique_ptr<Worker>> named_workers;

        /* mutex and condition variable for sync between workers */
        std::mutex lock_req_, lock_resp_;
        std::condition_variable cv_req_, cv_resp_;
};

#endif
