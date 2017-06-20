/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 16 Jun 2017 04:27:57 PM CST
 Description: 
 ************************************************************************/

#ifndef __B_H__
#define __B_H__

#include "a.h"

class A;
class B
{
    public:
        void Delegate(A *a);
        void Do();
};


#endif
