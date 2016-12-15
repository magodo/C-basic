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
     * Concept
     * =======
     *
     * 1. Option with argument
     *
     * "--length 100" ---> "option element".
     * "length"       ---> "option character"
     * "100"          ---> "option argument"
     *
     * 2. Option with no argument
     *
     * "--enable-something"
     *
     * 3. Option with optional argument
     *
     * This is GNU libc extension, forbiddend by POSIX.1-2008, section 12.2
     *
     * External variables
     * ==================
     *
     * - extern char *optarg: 
     *      * required_argument/':' : accept the "option argument"
     *      * optional_argument/'::': accpet the "option argument" when provided, 0 otherwise(frowned down)
     *      * no_argument/''        : not defined
     *
     * - extern int optind:
     *      index of the next element to be processed in argv(should be reset to 1 beforehead)
     *
     * - extern int opterr:
     *      toggle if to print error message(0: disable)
     *
     * - extern int optopt:
     *      when option char not in optstring or miss argument, the option char is stored in optopt
     *
     * Return
     * ======
     * hit: 
     *     1. short option -> option char
     *     2. long option  -> "struct option::val" if "option::flag" is NULL,
     *                        0 otherwise
     * error:
     *     1. cmd-line arguments have been parsed          -> -1
     *     2. option character was not found in optstring  -> '?'
     *     3. option element miss argument                 -> ':' if optstring startswith ':'
     *                                                        '?' otherwise
     * Other Mode
     * ==========
     *
     * 1. optstring = "+....": option processing stops ASA a nonoption argument is met
     * 2. optstring = "-....": non-option argv-element is handled as if it were the argument of option with character code 1
     *
     */

     printf("\n");
     printf("At begining, content of \"argv\": \n");
     for (int i = 0; i < argc; i++)
         printf("%s ", argv[i]);
     printf("\n\n");

    printf("Now parsing arguments with getopt_long()...\n");
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

     /* getopt() will permutes the contents of argv as it scans, eventually all nonoptions(not starts with "-")
      * are at the end(the first argument's index in argv[] equals to "optind").
      */
     printf("\n");
     printf("After getopt_long(), the content of \"argv\" is changed into: \n");
     for (int i = 0; i < argc; i++)
         printf("%s ", argv[i]);
     printf("\n");
     printf("And optind = %d.", optind);
}
