/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 03 May 2017 12:44:28 PM CST
 Description: 
 ************************************************************************/

#include <iostream>
using namespace std;

// '#' is used to convert to 'exp' tokens into string (i.e. stringizing)
#define OUTPUT_EXP(exp) \
    do { \
        cout << "Expression: " << #exp << endl; \
    } while(0);

// '##' is used to concatenate tokens together to be another token
#define decodes(a,b,c,d) a ## b ## c ## d
#define begin decodes(m,a,i,n)

int begin()
{
    OUTPUT_EXP(3 != 0);
}


