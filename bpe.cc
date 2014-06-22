// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>
//
// A file compressor tool that implements Byte Pair Encoding scheme.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "compress.h"
#include "decompress.h"

const int min_block_size        = 100;
const int max_block_size        = 65535;
const int default_block_size    = 8000;

struct Config
{
    bool help_opt;
    bool compress_opt;
    bool decompress_opt;
    bool block_size_opt;
    bool verbose_opt;
    int block_size;
    char *infile;
    char *outfile;
} config;

static void usage(const char *name)
{
    printf(
"bpe: a file compressor that implements Byte Pair Encoding.\n"
"\n"
"Usage: %s OPTIONS [<infile> <outfile>]\n"
"\n"
"  -c --compress        Compress <infile> and write output on <outfile>.\n"
"  -d --decompress      Decompress <infile> and write output on <outfile>.\n"
"  -b --block-size <n>  Block size (default: %d).\n"
"  -v --verbose         Verbose. Display the percentage reduction.\n"
"  -h --help            Output this help and exit.\n"
"\n"
"Examples:\n"
"  bpe -c -v test.txt test.txt.bpe\n"
"  bpe -d test.txt.bpe test.txt\n"
"\n",
    name, default_block_size);
}

static int parseOptions(int argc, char **argv)
{
    if(argc < 2)
    {
        usage(argv[0]);
        exit(1);
    }

    int i;
    for(i=1; i<argc; ++i)
    {
        if((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0))
        {
            config.help_opt = true;
        }
        else if((strcmp(argv[i], "-c") == 0) || (strcmp(argv[i], "--compress") == 0))
        {
            config.compress_opt = true;
        }
        else if((strcmp(argv[i], "-d") == 0) || (strcmp(argv[i], "--decompress") == 0))
        {
            config.decompress_opt = true;
        }
        else if((strcmp(argv[i], "-b") == 0) || (strcmp(argv[i], "--block-size") == 0))
        {
            config.block_size_opt = true;

            if((i+1 == argc) || (argv[i+1][0] == '-'))
                config.block_size = 0;
            else
                config.block_size = atoi(argv[++i]);
        }
        else if((strcmp(argv[i], "-v") == 0) || (strcmp(argv[i], "--verbose") == 0))
        {
            config.verbose_opt = true;
        }
        else
        {
            if(argv[i][0] == '-')
            {
                printf("Unrecognized option: '%s'\n", argv[i]);
                exit(1);
            }
            else
            {
                break;
            }
        }
    }

    return i;
}

static void validateOptions(int argc, char **argv, int lastopt)
{
    int lastargs = argc - lastopt;

    if(config.help_opt)
    {
        usage(argv[0]);
        exit(0);
    }

    if(config.compress_opt && config.decompress_opt)
    {
        printf("Incompatible options -c and -d\n");
        exit(1);
    }

    if(config.block_size_opt && !config.compress_opt)
    {
        printf("-b option is only compatible with -c option\n");
        exit(1);
    }

    if(config.verbose_opt && !config.compress_opt)
    {
        printf("-v option is only compatible with -c option\n");
        exit(1);
    }

    if((config.block_size < min_block_size) || (config.block_size > max_block_size))
    {
        printf("Incorrect block size. It must be in the range [%d, %d]\n",
            min_block_size, max_block_size);
        exit(1);
    }

    if(lastargs != 2)
    {
        printf("Bad number of arguments. Use -h to display more information\n");
        exit(1);
    }

    config.infile = argv[lastopt++];
    config.outfile = argv[lastopt];
}

int main(int argc, char **argv)
{
    config.help_opt = false;
    config.compress_opt = false;
    config.decompress_opt = false;
    config.block_size_opt = false;
    config.verbose_opt = false;
    config.block_size = default_block_size;
    config.infile = NULL;
    config.outfile = NULL;

    int lastopt = parseOptions(argc, argv);

    validateOptions(argc, argv, lastopt);

    if(config.compress_opt)
        compress(config.infile, config.outfile, config.block_size, config.verbose_opt);
    else if(config.decompress_opt)
        decompress(config.infile, config.outfile);

    return 0;
}
