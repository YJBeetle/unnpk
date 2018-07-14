#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <getopt.h>

#include "args.h"

void args_init(argsdata *data)
{
    data->in_filename = 0;
    data->out_filename = 0;
    data->format = MARKDOWN;
    data->type = HEX;
}

void args_help(const char *argv0)
{
    char *argv0_copy = strdup(argv0);
    char *argv0_base = basename(argv0_copy);

    printf("Usage: %s [OPTIONS] \n", argv0_base);
    printf("%s - \n", argv0_base);
    puts("");
    printf("  -i, --input\tinput file\n");
    printf("  -o, --output\toutput file\n");
    printf("  -f, --format\toutput format (markdown csv)\n");
    printf("  -t, --type\tdata type (hex int original)\n");
    printf("  -h, --help\tGive this help list\n");
    puts("");

    free(argv0_copy);
}

void args_usage(const char *argv0)
{
    char *argv0_copy = strdup(argv0);
    char *argv0_base = basename(argv0_copy);

    printf("Usage: %s [OPTION] \n", argv0_base);
    printf("Try `%s --help' for more information.\n", argv0_base);

    free(argv0_copy);
}

void args_parse(int argc, char *argv[], argsdata *data)
{
    static const char args_optstring[] = "i:o:f:t:h";
    static struct option args_options[] =
        {
            /* name,		has_arg,	flag,	val */
            {"input", 1, 0, 'i'},
            {"output", 1, 0, 'o'},
            {"format", 1, 0, 'f'},
            {"type", 1, 0, 't'},
            {"help", 0, 0, 'h'},
            {0}};

    int c;

    while (1)
    {
        c = getopt_long(argc, argv, args_optstring, args_options, NULL);
        if (c == -1)
            break;

        //printf("|%c,%s|",c,optarg);

        switch (c)
        {
        case 'i':
            data->in_filename = optarg;
            break;
        case 'o':
            data->out_filename = optarg;
            break;
        case 'f':
            if (strcasecmp(optarg, "markdown") == 0)
                data->format = MARKDOWN;
            else if (strcasecmp(optarg, "csv") == 0)
                data->format = CSV;
            break;
        case 't':
            if (strcasecmp(optarg, "hex") == 0)
                data->type = HEX;
            else if (strcasecmp(optarg, "int") == 0)
                data->type = INT;
            else if (strcasecmp(optarg, "original") == 0)
                data->type = ORIGINAL;
            break;
        case 'h':
            args_help(argv[0]);
            exit(1);
        case '?':
        default:
            args_usage(argv[0]);
            exit(1);
            break;
        }
    }

    if (argc - optind >= 1)
    {
        data->in_filename = argv[optind++];
        if (argc - optind >= 1)
        {
            data->out_filename = argv[optind++];
            if (argc - optind > 0)
            {
                args_usage(argv[0]);
                exit(1);
            }
        }
    }

    if (!data->in_filename)
    {
        args_usage(argv[0]);
        exit(1);
    }
}
