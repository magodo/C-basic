/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 16 Jun 2017 04:26:28 PM CST
 Description: 
 ************************************************************************/

#include "b.h"

void B::Do()
{
}

void B::Delegate(A *a)
{
    a->Do();
}
