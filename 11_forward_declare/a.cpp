/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Fri 16 Jun 2017 04:26:28 PM CST
 Description: 
 ************************************************************************/

#include "a.h"

void A::Do()
{
}

void A::Delegate(B *b)
{
    b->Do();
}
