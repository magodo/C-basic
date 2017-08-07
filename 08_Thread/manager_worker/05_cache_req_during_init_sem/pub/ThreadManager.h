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
@brief    Base manager thread 
@par      Project: GWM MY18
@par      SW Component: VideoService
@par      SW Package: VideoService 
@author   
@par      Configuration Management:
@verbatim
	%derived_by: uidq8219 %
	%full_filespec: ThreadManager.h-1:incl:GWM_Sha#1 % 
	%version: 1 % 
	%date_created: Tue Jul 18 19:51:24 2017 % 
@endverbatim

@todo     list of things to do
*/
#ifndef __THREADMANAGER_H__
#define __THREADMANAGER_H__

#include <thread>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ThreadWorker.h"

// forward delcare
class ThreadWorker;

/* current request on thread worker */
enum class ThreadWorkerReq
{
    New,
    Run,
    Stop,
    Quit
};

struct ThreadWorkerInfo
{
    std::shared_ptr<ThreadWorker> ptr;
    std::thread                   thread;
    ThreadWorkerReq               req;
};

class ThreadManager
{
    public:

        virtual ~ThreadManager(){}
        bool NewWorker(std::shared_ptr<ThreadWorker> p_worker);
        bool DeleteWorker(std::string worker_name);
        bool QuitWorker(std::string name);
        bool RunWorker(std::string name);
        bool StopWorker(std::string name);
    
    protected:

        std::map<std::string, ThreadWorkerInfo> named_workers_;
};

#endif
