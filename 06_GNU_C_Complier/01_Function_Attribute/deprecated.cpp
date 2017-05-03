/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 03 May 2017 01:08:11 PM CST
 Description: 
 ************************************************************************/

void foo() __attribute__(( deprecated("Use bar() instead!") ));
void bar();

void foo()
{}

void bar()
{}

int main()
{
    foo();
}
