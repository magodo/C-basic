/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sat 12 Nov 2016 08:21:01 PM CST
 Description: 
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>

#define MAXBUF 100

void pr_error(const char *fmt, ...)
{
    char buf[MAXBUF];

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ": %s\n", strerror(errno));
}

void convert(const char *str)
{
    long res;

    errno = 0;
    res = strtol(str, NULL, 0); // base set to 0 so that strtol knows 0x or 0 prefixes
    if (errno != 0)
        pr_error("%s -> %ld", str, res);
    else
        printf("%s -> %ld\n", str, res);
}

int main()
{
    extern int errno;

    /* normal case */

    convert("010");

    /* another normal case */

    convert("0x10");

    /* another normal case */

    convert("9223372036854775807");

    /* overflow case */

    convert("9223372036854775808");

    /* convert part of string */
    convert("123a");
}
