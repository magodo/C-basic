/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 27 Jun 2017 10:42:05 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <memory>
#include <sstream>
#include "ThreadManager.h"
#include "ThreadWorker.h"

int main()
{
    ThreadManager manager;

    while (1)
    {
        int choice;
        std::string worker_name, names;
        std::vector<std::string> worker_names;

        std::cout << "1. New worker thread\n2. Delete worker thread\n3. Init worker thread\n4. Init workers simultaneously\n5. Deinit worker thread\n6. Run worker thread\n7. Stop worker thread\n8. Quit\nEnter the number: ";
        std::cin >> choice;
        if (choice == 8)
            return 1;
        if (choice < 8 && choice > 0)
        {
            if (choice != 4)
            {
                std::cout << "Please enter worker name: ";
                std::cin >> worker_name;
            }
            else
            {
                std::cout << "Please enter workers' names(separate by space): ";
                std::getline(std::cin, names); // feed the last new line character when reading choice
                std::getline(std::cin, names);
                std::istringstream is(names);
                std::string name;
                while (is >> name)
                    worker_names.push_back(name);
            }
        }
        switch (choice)
        {
            case 1:
                manager.NewWorker(std::make_shared<ThreadWorker>(worker_name));
                break;
            case 2:
                manager.DeleteWorker(worker_name);
                break;
            case 3:
                manager.InitWorker(worker_name);
                break;
            case 4:
                manager.InitWorkers(worker_names);
                break;
            case 5:
                manager.DeinitWorker(worker_name);
                break;
            case 6:
                manager.RunWorker(worker_name);
                break;
            case 7:
                manager.StopWorker(worker_name);
                break;
            default:
                std::cerr << "???" << std::endl;
                break;
        }
    }
}
