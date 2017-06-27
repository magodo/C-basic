/* ************************************************************************
* Copyright (c) Continental AG and subsidiaries
* All rights reserved
*
* The reproduction, transmission or use of this document or its contents is
* not permitted without express written authority.
* Offenders will be liable for damages. All rights, including rights created
* by patent grant or registration of a utility model or design, are reserved.
* ************************************************************************/


/**
@file     FbDevBL.h
@brief    Manager-Worker framework
@par      Project: GWM MY18
@par      SW Component: VideoService
@par      SW Package: VideoService 
@author   
@par      Configuration Management:
@verbatim
	%derived_by: uidp7757 %
	%full_filespec: IFbCtrl.h-1 % 
	%version: 1 % 
	%date_created: Tue May 16 17:49:00 2017 % 
@endverbatim

@todo     list of things to do
*/

#ifndef __MANAGERWORKER_H__
#define __MANAGERWORKER_H__

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <map>

namespace vehicle
{
    namespace videoservice
    {
        class Manager;

        class Worker
        {
            friend class Manager;

            public:
                Worker();
                Worker(std::string name);

                virtual ~Worker();
                virtual void init();
                virtual void deinit();
                virtual void run();
                virtual void stop();
                void setManager(Manager *manager);
                std::string getName();
                void notify(unsigned int evt);

            protected:
                bool is_running;
                bool is_init;
                std::thread thread;

            private:
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
                virtual ~Manager(){};
                void attachWorker(Worker *p_worker);
                void detachWorker(std::string name);
                void initWorker(std::string name);
                void deinitWorker(std::string name);
                void startWorker(std::string name);
                void stopWorker(std::string name);
                virtual void callback(unsigned int evt);

            protected:
                std::map<std::string, Worker*> named_workers_;

            private:
                static void newThread(Worker& worker);
            private:
                /* mutex and condition variable for sync between workers */
                std::mutex lock_req_, lock_resp_;
                std::condition_variable cv_req_, cv_resp_;
        };
    };
};

#endif
