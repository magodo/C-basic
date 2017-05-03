/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Wed 03 May 2017 01:20:15 PM CST
 Description: 
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char *old_format = "today is: %d (DOM)";

#if 0
const char *my_dgettext(const char *my_domain, const char *my_format) __attribute__ (( format_arg(2) ));
#endif

const char *my_dgettext(const char *my_domain, int my_format) __attribute__ (( format_arg(2) )); 

int main()
{
#if 0
    printf(my_dgettext("[FOO]", old_format), 12);
#endif
    printf(my_dgettext("[FOO]", 123), 12);
    putchar('\n');
}

const char *my_dgettext(const char *my_domain, int my_format)
{
    return my_domain;
}
#if 0
const char *my_dgettext(const char *my_domain, const char *my_format)
{
    char *buf = (char*)malloc(strlen(my_domain)+strlen(my_format)+10);
    const char *sep = ": ";
    int offset = 0;

    memcpy(buf+offset, my_domain, strlen(my_domain));
    offset += strlen(my_domain);

    memcpy(buf+offset, sep, strlen(sep));
    offset += strlen(sep);

    memcpy(buf+offset, my_format, strlen(my_format));
    offset += strlen(my_format);

    *(buf+offset) = '\0';

    return buf;
}

#endif
