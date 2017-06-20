/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 20 Jun 2017 09:38:19 AM CST
 Description: 
 ************************************************************************/

#include <memory>
#include <iostream>

enum State { kState1, kState2 };

class BaseState;
class State1; class State2;

class Foobar
{
    public:
        Foobar();
        void DoSomething();
        void Update(State state);
        void ChangeState(std::shared_ptr<BaseState> state_ptr);

        void OnEnter();
        void OnLeave();

    private:
        std::shared_ptr<BaseState> state_;
};

class BaseState
{
    public:
        virtual ~BaseState(){}
        virtual void DoSomething() = 0;
        virtual void Update(Foobar&, State) = 0;

        virtual void OnEnter() = 0;
        virtual void OnLeave() = 0;

};

class State1: public BaseState
{
    public:
        virtual void DoSomething();
        virtual void Update(Foobar &foobar, State state);
        virtual inline void OnEnter() {std::cout << "Enter State1" << std::endl;}
        virtual inline void OnLeave() {std::cout << "Leave State1" << std::endl;}
};

class State2: public BaseState
{
    public:
        virtual void DoSomething();
        virtual void Update(Foobar &foobar, State state);
        virtual inline void OnEnter() {std::cout << "Enter State2" << std::endl;}
        virtual inline void OnLeave() {std::cout << "Leave State2" << std::endl;}
};

Foobar::Foobar()
{state_ = std::make_shared<State1>();}

void Foobar::DoSomething()
{state_->DoSomething();}

void Foobar::Update(State state)
{
    state_->Update(*this, state);
}

void Foobar::ChangeState(std::shared_ptr<BaseState> state_ptr)
{ state_ = state_ptr;}

void Foobar::OnEnter()
{ state_->OnEnter();}

void Foobar::OnLeave()
{ state_->OnLeave();}

void State1::DoSomething()
{std::cout << "State1" << std::endl;}

void State1::Update(Foobar &foobar, State state)
{
    switch (state)
    {
        case kState1:
            std::cout << "Already in State1" << std::endl;
            break;
        case kState2:
            std::cout << "State1 -> State2" << std::endl;
            foobar.OnLeave();
            foobar.ChangeState(std::make_shared<State2>());
            foobar.OnEnter();
            break;
    }
}

void State2::DoSomething()
{std::cout << "State2" << std::endl;}

void State2::Update(Foobar &foobar, State state)
{
    switch (state)
    {
        case kState2:
            std::cout << "Already in State2" << std::endl;
            break;
        case kState1:
            std::cout << "State2 -> State1" << std::endl;
            foobar.OnLeave();
            foobar.ChangeState(std::make_shared<State1>());
            foobar.OnEnter();
            break;
    }
}


int main()
{
    int choice;
    Foobar foobar;

    while (1)
    {
        std::cout << "1. DoSomething\n2. State1\n3. State2\nPlease enter number: ";
        std::cin >> choice;
        switch (choice)
        {
            case 1:
                foobar.DoSomething();
                break;
            case 2:
                foobar.Update(kState1);
                break;
            case 3:
                foobar.Update(kState2);
                break;
        }
    }
}
