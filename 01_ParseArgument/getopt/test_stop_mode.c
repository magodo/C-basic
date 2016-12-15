/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Thu 15 Dec 2016 09:54:40 PM CST
 Description: Test stop on non-option argv mode (optstring = "+...")
              In this mode, option processing will stop as soon as a non-option
              argv-element is met, and the "optind" is of the index of that element.
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void main(int argc, char *argv[])
{
    optind = 1;
    opterr = 0;

    char *optstring = "+:l:c:";
    struct option longopt[] = {
        {"length", required_argument, NULL, 'l'},
        {"color", required_argument, NULL, 'c'},
        {0,0,0,0}
    };

    printf("\n");
    printf("At begining, content of \"argv\": \n");
    for (int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n\n");

    printf("Now parsing arguments with getopt_long()...\n");

    int val;
    while ((val = getopt_long(argc, argv, optstring, longopt, NULL)) != -1)
    {
        switch (val)
        {
            case 'l':
                printf("length option is caught, its argument is %d\n", strtol(optarg, NULL, 0));
                break;
            case 'c':
                printf("color option is caught, its argument is %s\n", optarg);
                break;
            case '?':
                printf("Unknown argument: %c\n", optopt);
                break;
            case ':':
                printf("option requires an argument -- '%c'\n", optopt);
                break;
            default:
                printf("Not intend to be here!\n");
        }
    }
    printf("\n");
    printf("After getopt_long(), the content of \"argv\" is changed into: \n");
    for (int i = 0; i < argc; i++)
        printf("%s ", argv[i]);
    printf("\n");
    printf("And optind = %d.", optind);
    printf("\n");
}



