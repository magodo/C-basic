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
@brief    Base worker thread 
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
#include <functional>
#include <condition_variable>
#include "ThreadManager.h"


class ThreadWorker
{

    enum WorkerState
    {
        kWorkerStateIniting,
        kWorkerStateInited,
        kWorkerStateRunning,
        kWorkerStateQuiting
    };

    public:

        ThreadWorker();
        ThreadWorker(std::string name);
        virtual ~ThreadWorker(){}

        // Following are called in a dedicated thread and meant to be overloaded
        
        virtual void Init();
        virtual void Run();
        virtual void Stop();
        virtual void Deinit();

        void Callback(unsigned int evt);

        // Used by manager thread
        
        inline std::string GetName() {return name_;}

        inline bool IsIniting() { return state_ == kWorkerStateIniting;}
        inline bool IsInited()  { return state_ == kWorkerStateInited; }
        inline bool IsRunning() { return state_ == kWorkerStateRunning;}
        inline bool IsQuiting() { return state_ == kWorkerStateQuiting;}

        inline void ToRun()         { to_run_ = true;}
        inline void ToStop()        { to_run_ = false;}
        inline void ToQuit()        { to_quit_ = true;}

        inline std::condition_variable& GetReqCv() { return cv_req_;}
        inline std::condition_variable& GetRespCv() { return cv_resp_;}
        inline std::mutex& GetReqMutex() {return mutex_req_;}
        inline std::mutex& GetRespMutex() {return mutex_resp_;}


        inline void SetCallback(std::function<void(unsigned int)> f)    {callback_ = f;}

        static void ThreadFunction(ThreadWorker& worker);

    private:

        bool to_run_;
        bool to_quit_;
        WorkerState state_;

        std::string name_;

        std::condition_variable cv_req_;   // manager -> worker
        std::mutex mutex_req_;
        std::condition_variable cv_resp_;  // worker -> manager
        std::mutex mutex_resp_;

        std::function<void(unsigned int)> callback_;

};

#endif
