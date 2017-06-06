/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 06 Jun 2017 01:18:29 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <functional>

class Worker
{
    public:

        Worker():
            callback_(nullptr)
        {}

        void doSomething()
        {
            /* doing something... */
            callback();
        }

        void callback()
        {
            if (callback_ != nullptr)
                callback_();
        }

        void setCallback(std::function<void()> f)
        {
            callback_ = f;
        }

    private:
        std::function<void()> callback_;
};

class Interface
{
    public:
        void notify()
        {
            std::cout << "Notify outside world some event" << std::endl;
        }
};

class Manager
{
    public:

        Manager():
            worker_(),
            interface_()
        {
            worker_.setCallback(std::bind(&Interface::notify, &interface_));
        }

        void doSomething()
        {
            worker_.doSomething();
        }

    private:
        Interface   interface_;
        Worker      worker_;
};

int main()
{
    Manager mgr;

    mgr.doSomething();
}
