/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Tue 06 Jun 2017 11:17:57 AM CST
 Description: 
 ************************************************************************/

#include <iostream>
#include <functional>

using namespace std::placeholders;

class Foobar
{
    public:
        Foobar():
            value_(0)
        {}

        Foobar(const Foobar &o):
            value_(0) // reset value to 0
        {
            std::cout << "Foobar: copy C'tor" << std::endl;
        }

        int setValue(int v)
        {value_ = v;}

        int printValue()
        {
            std::cout << value_ << "(caller: " << std::showbase << this << ")" << std::endl;
        }

    private:
        int value_;
};

int main()
{
    Foobar foobar;
    foobar.setValue(100);

    // If pass object to bind(), the copy C'tor will be called.
    // A new object is constructed.
    std::cout << "\nPassing Object: " << std::endl;
    auto f = std::bind(&Foobar::printValue, foobar);
    f();

    std::cout << "\nPassing pointer: " << std::endl;
    auto ff = std::bind(&Foobar::printValue, &foobar);
    ff();

    std::cout << "\nPassing reference: " << std::endl;
    auto fff = std::bind(&Foobar::printValue, std::ref(foobar));
    fff();
}
