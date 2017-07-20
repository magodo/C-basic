/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 19 Jul 2017 02:10:17 PM CST
 Description: 
 ************************************************************************/

#ifndef PACKETFORMAT_H
#define PACKETFORMAT_H

#include <stdint.h>

struct PacketFormat
{
    uint8_t op_code;
    union 
    {
        int i;
        char *str;
    } op_value;
};
#endif
