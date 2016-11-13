/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Sat 12 Nov 2016 07:12:48 PM CST
 Description: Learn to use getopt() functions
 ************************************************************************/

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    /* 
     * External variables:
     *
     * - extern char *optarg: accept the option argument
     * - extern int optind: index of the next element to be processed in argv(should be reset to 1 beforehead)
     * - extern int opterr: toggle if to print error message(0: disable)
     * - extern int optopt: when option char not in optstring or miss argument, the option is stored in optopt
     *
     * getopt_long():
     *  :  means option requires an argument
     *  :: means argument is optional
     *  optstring "W" is reserved by POSIX.2
     * 
     *  return:
     *      hit: 
     *          on short option, return the option char
     *          on long option, return the "option::val" if "option::flag" is NULL,
     *          otherwise, return 0.
     *      error:
     *          return -1: cmd-line arguments have been parsed
     *          return ? : option character was not found or option miss argument(not specified in optstring)
     *          return : : option miss argument(specified in optstring)
     *
     */

    /* short opts */

    const char *optstring = ":vr:c:f:"; // the starting ":" here makes getopt_long() return ":" when option miss argument

    /* long opts */

     struct option longopts[] = 
     {
         {"version", no_argument, NULL, 'v'},
         {"rate", required_argument, NULL, 'r'},
         {"channel", required_argument, NULL, 'c'},
         {"format", required_argument, NULL, 'f'},
         {0,0,0,0}   // longopts should endswith zero
     };
     
     /* before parsing, set optind to 1(start from 1st argv-element) and opterr to 0(suppress err msg)*/
     optind = 1; 
     opterr = 0;

     char ret;
     while ((ret = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
     {
        switch (ret)
        {
            case 'v':
                printf("-v|--version is accpeted\n");
                break;
            case 'r':
                printf("-r|--rate is accpeted: %ld\n", strtol(optarg, NULL, 0));
                break;
            case 'f':
                printf("-f|--format is accepted: %s\n", optarg);
                break;
            case 'c':
                printf("-c|--channel is accepted: %ld\n", strtol(optarg, NULL, 0));
                break;
            case '?':
                printf("Unknown option: %c\n", optopt);
                break;
            case ':':
                printf("option requires an argument -- '%c'\n", optopt);
                break;
            default:
                printf("Not intend to be here!\n");
        }
     }
}
