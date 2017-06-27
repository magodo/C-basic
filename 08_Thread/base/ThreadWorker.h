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
@brief    Manager-Worker framework - worker part
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

#ifndef __THREADWORKER_H__
#define __THREADWORKER_H__

#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "ThreadManager.h"

namespace vehicle
{
    namespace videoservice
    {
        class ThreadWorker
        {
            public:

                ThreadWorker();
                ThreadWorker(std::string name);
                virtual ~ThreadWorker(){}

                // Following are called in a dedicated thread
                void Init();
                void Deinit();
                void Run();
                void Stop();

                void Join();

                inline std::string GetName() {return name_;}
                inline std::condition_variable GetReqCv() { return cv_req_;}
                inline std::condition_variable GetRespCv() { return cv_resp_;}
                inline std::mutex GetReqMutex() {return mutex_req_;}
                inline std::mutex GetRespMutex() {return mutex_resp_;}

                inline void SetToDeinit(bool to_deinit) {to_deinit_ = to_deinit;}
                inline void SetToRun(bool to_run)       {to_run_ = to_run;}

                void Notify(unsigned int evt);
            
            protected:

                bool to_deinit_;
                bool to_run_;

            private:

                static void ThreadFunction(ThreadWorker& worker);

            private:

                std::string name_;
                std::condition_variable cv_req_;   // manager -> worker
                std::mutex mutex_req_;
                std::condition_variable cv_resp_;  // worker -> manager
                std::mutex mutex_resp_;

        };
    };
};

#endif
