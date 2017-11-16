/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 14 Nov 2017 12:03:45 PM CST
 Description: 
 ************************************************************************/

#include <iostream>

using namespace std;

class B
{
    public:
        // C'tor
        B(int value)
        {
            cout << "C'tor(" << value << ")" << endl;
            p = new int(value);
        }

        ~B()
        {
            if (p != nullptr)
            {
                cout << "D'tor(" << *p << ")" << endl;
                delete p;
            }
        }

        // copy
        B(const B& o)
        {
            cout << "Copy C'tor: " << *o.p << endl;
            p = new int(*(o.p));
        }

        B& operator=(const B& o)
        {
            cout << "Copy=" << endl;
            if (&o != this)
            {
                delete p;
                p = new int(*(o.p));
            }
        }

        // move
        B(B&& o)
        {
            cout << "Move C'tor: " << *o.p << endl;
            p = o.p;
            o.p = nullptr;
        }

        B& operator=(B&& o)
        {
            cout << "Move=" << endl;
            p = o.p;
            o.p = nullptr;
        }

        int *p;
};

B ret_Bref()
{
    B b{321};
    return std::move(b);
}

B ret_Bval()
{
    return (B{666});
}

int main()
{
//    B b1{123};
    B b2 = ret_Bref();
    B&& b3 = ret_Bval();
}
