/*************************************************************************
 Author: Zhaoting Weng
 Created Time: Thu 15 Dec 2016 09:20:30 PM CST
 Description: Let's say we have a program named "foobar", it has following usage:
                Usage: foobar [COMMAND] [OPTION] 
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>

/***********
 * Declare
 **********/

struct subcommand
{
    const char *name;
    int (*func)(int argc, char *argv[]);
};

static int foo(int argc, char *argv[]);
static int bar(int argc, char *argv[]);

/*******************
 * Global variables
 *******************/

/* used to store name of this program */
static char *program_name = 0;

static struct subcommand subcommands[] = {
    {"foo", foo},
    {"bar", bar}
};

/*******************************************************
 * Usage function for every subcommand and the program itself.
 *******************************************************/

static void usage(const char *subcommand)
{
    /* no subcommand is specified */
    if (subcommand == NULL)
    {
        printf("Usage: %s [COMMAND] [OPTION]\n", program_name);

        printf("\nSupported [COMMAND] includes:\n");
        unsigned i;
        unsigned n_of_subcommands = sizeof(subcommands)/sizeof(subcommands[0]);
        for (i = 0; i < n_of_subcommands; ++i)
            printf("* %s\n", subcommands[i].name);
    }
    else
    {
        if (!strcmp(subcommand, "foo"))
        {
            printf("Usage: %s %s [OPTION]\n", program_name, subcommand);
        }
        else if (!strcmp(subcommand, "bar"))
        {
            printf("Usage: %s %s [OPTION] [TARGET]\n", program_name, subcommand);
        }
        // no need to check "else" cases, this is guaranteed because it will only be called by corresponding function
    }
}

/************************
 * Sub-command functions
 * *********************/


static int foo(int argc, char *argv[])
{

    optind = 1; // reset the index

    const char *optstring = ":a:h";
    const struct option longopts[] = {
        {"aa", required_argument, NULL, 'a'},
        {"help", no_argument, NULL, 'h'},
        {0,0,0,0}
    };

    int val;
    while ((val = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch (val)
        {
            case 'a':
                printf("option a is accepted, its argument is: %s\n", optarg);
                break;
            case 'h':
                usage(argv[0]);
                return 0;
            case '?':
                printf("unknown option: %c\n", optopt);
                return -1;
            case ':':
                printf("option requires an argument -- %c\n", optopt);
                return -1;
            default:
                printf("will never reach here\n");
                return -1;
        }
    }
    return 0;
}

static int bar(int argc, char *argv[])
{
    optind = 1; // reset the index

    const char *optstring = ":b:h";
    const struct option longopts[] = {
        {"bb", required_argument, NULL, 'b'},
        {"help", no_argument, NULL, 'h'},
        {0,0,0,0}
    };

    int val;
    while ((val = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
    {
        switch (val)
        {
            case 'b':
                printf("option b is accepted, its argument is: %s\n", optarg);
                break;
            case 'h':
                usage(argv[0]);
                return 0;
            case '?':
                printf("unknown option: %c\n", optopt);
                return -1;
            case ':':
                printf("option requires an argument -- %c\n", optopt);
                return -1;
            default:
                printf("will never reach here\n");
                return -1;
        }
    }

    /* means there is no non-option argv-element exists */
    if (argv[optind] == '\0')
    {
        printf("No non-option argv-element exists! Exiting...\n");
        return -1;
    }
    else
    {
        printf("We get target: %s\n", argv[optind]);
        return 0;
    }
}

/*************
 * MAIN
 *************/

int main(int argc, char *argv[])
{
    int ret;

    /* array of function names of */
    optind = 1;
    opterr = 0;

    /* store program name in global variable */
    program_name = strdup(argv[0]);

    /* check if there is a sub-command(it is guaranteed to be the argv[1] element)*/
    if (argv[1] != '\0' && *argv[1] != '-')
    {
        /* A subcommand is specified */

        unsigned index;
        unsigned n_of_subcommands = sizeof(subcommands)/sizeof(subcommands[0]);
        for (index = 0; index < n_of_subcommands; ++index)
        {
            if (!strcmp(subcommands[index].name, argv[1]))
            {
                /* exclude the first argv-element */
                --argc;
                argv = &argv[1];

                /* call subcommand function */
                ret = subcommands[index].func(argc, argv);
                break;
            }
        }
        if (index == n_of_subcommands)
        {
            printf("Unknown subcommand: %s\n", argv[1]);
            ret = -1;
        }
    }
    else
    {
        /* No subcommand specified, run program directly*/
        const char *optstring = ":h";
        const struct option longopts[] = {
            {"help", required_argument, NULL, 'h'},
            {0,0,0,0}
        };
        int val;

        while ((val = getopt_long(argc, argv, optstring, longopts, NULL)) != -1)
        {
            switch (val)
            {
                case 'h':
                    usage(NULL);
                    return 0;
                case '?':
                    printf("unknown option: %c\n", optopt);
                    return -1;
                case ':':
                    printf("option requires an argument -- %c\n", optopt);
                    return -1;
                default:
                    printf("will never reach here\n");
                    return -1;
            }
        }
    }

    /* free allocated memeory before leave */
    free(program_name);

    return 0;
}
