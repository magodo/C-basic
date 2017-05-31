/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 26 May 2017 03:12:07 PM CST
 Description: 
 ************************************************************************/

#include <thread>
#include <condition_variable>
#include <mutex>
#include <iostream>
#include <chrono>

#if 0
#include "IIpu.h"
#include "IVpu.h"
#include "FbDevBL.h"
#include "IFileParser.h"

using namespace vehicle::videoservice;
#endif

bool                        g_end = false;

bool                        g_to_init = false;
bool                        g_to_deinit = false;
bool                        g_inited = false;

bool                        g_to_start = false;
bool                        g_to_stop = false;
bool                        g_started = false;

bool                        g_hide = false;

std::mutex                  g_lock_send, g_lock_resp;
std::condition_variable     g_cv_send, g_cv_resp; 

void play_animation()
{
    std::cout << "Thread: enter" << std::endl;

    while (!g_end)
    {
        if (!g_inited)
        {
            {
                // wait to init/end signal
                std::unique_lock<std::mutex> locker(g_lock_send);
                std::cout << "Thread: waiting for 'Init'/'End' signal"  << std::endl;
                g_cv_send.wait(locker, []{return (g_to_init || g_end);});
                // got end signal
                if (g_end)
                {
                    std::cout << "Thread: 'End' signal caught"<< std::endl;
                    break;
                }
            }

            {
                std::lock_guard<std::mutex> locker(g_lock_resp);
                
                // init
                std::cout << "Thread: 'Init' signal caught" << std::endl;
                std::cout << "Thread: initiating..." << std::endl;
                g_inited = true;

                // send inited signal
                g_cv_resp.notify_one();
            }
        }

        if (!g_started)
        {
            bool to_start, to_deinit;
            {
                // wait start/deinit/end signal
                std::unique_lock<std::mutex> locker(g_lock_send);
                std::cout << "Thread: wait for 'Start'/'Deinit'/'End' signal"<< std::endl;
                g_cv_send.wait(locker, []{return (g_to_start || g_end || g_to_deinit);});

                // got end signal
                if (g_end)
                {
                    std::cout << "Thread: 'End' signal caught"<< std::endl;
                    break;
                }

                to_start = g_to_start;
                to_deinit = g_to_deinit;
            }

            // got start signal
            if (to_start)
            {
                std::lock_guard<std::mutex> locker(g_lock_resp);

                // change state
                std::cout << "Thread: 'Start' signal caught" << std::endl;
                g_started = true;

                // send started signal
                g_cv_resp.notify_one();
            }
            else if (to_deinit)
            {
                std::lock_guard<std::mutex> locker(g_lock_resp);
                
                // deinit
                std::cout << "Thread: 'Deinit' signal caught" << std::endl;
                std::cout << "Thread: deinitializing..." << std::endl;
                g_inited = false;

                // send deinited signal
                g_cv_resp.notify_one();
            }
        }

        while ((g_started && (!g_end)))
        {
            std::cout << "Thread: working" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            // decode one frame
            
            // transfer one frame
            
            // show one frame
        }
    }
    std::cout << "Thread: quit" << std::endl;
}

int main()
{
#if 0
    // parse configurations
    IFileParser::getInstance()->parseFile("/etc/vs_config.ini");

    // get components
    FbDevBL *fbbl = FbDevBL::getInstance();
    IIpu *ipu = IIpu::getInstance();
    IVpu *vpu = IVpu::getInstance();
#endif

    // start thread
    std::thread th(play_animation);
    
    while (1)
    {
        int choice;
        std::cout << "Enter what you want to do:" << std::endl;
        std::cout << "1. Init" << std::endl;
        std::cout << "2. Deinit" << std::endl;
        std::cout << "3. Show" << std::endl;
        std::cout << "4. Hide" << std::endl;
        std::cout << "5. Stop" << std::endl;
        std::cout << "6. End" << std::endl;
        std::cin  >> choice;

        switch (choice)
        {
            case 1:
                {
                    // to init
                    {
                        std::lock_guard<std::mutex> locker(g_lock_send);
                        g_to_init = true;
                    }
                    g_cv_send.notify_one();
                    
                    // wait inited
                    {
                        std::unique_lock<std::mutex> locker(g_lock_send);
                        g_cv_resp.wait(locker, []{return g_inited;});
                        g_to_init = false;
                    }
                }
                break;

            case 2:
                {
                    // to deinit
                    {
                        std::lock_guard<std::mutex> locker(g_lock_send);
                        g_to_deinit = true;
                    }
                    g_cv_send.notify_one();
                    
                    // wait inited be false
                    {
                        std::unique_lock<std::mutex> locker(g_lock_send);
                        g_cv_resp.wait(locker, []{return !g_inited;});
                        g_to_deinit = false;
                    }
                }
                break;
            case 3:
                {
                    // to start
                    {
                        std::lock_guard<std::mutex> locker(g_lock_send);
                        g_to_start = true;
                        g_hide = false; // take note this line
                    }
                    g_cv_send.notify_one();
                    
                    // wait started
                    {
                        std::unique_lock<std::mutex> locker(g_lock_send);
                        g_cv_resp.wait(locker, []{return g_started;});
                        g_to_start = false;
                    }
                }
                break;

            case 4:
                {
                    std::lock_guard<std::mutex> locker(g_lock_send);
                    g_hide = true;
                }
                break;

            case 5:
                {
                    std::lock_guard<std::mutex> locker(g_lock_send);
                    g_started = false;
                }
                break;

            case 6:
                {
                    {
                        std::lock_guard<std::mutex> locker(g_lock_send);
                        g_end = true;
                    }
                    g_cv_send.notify_one();

                    th.join();
                }
                return;
        }
    }
}

