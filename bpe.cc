// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>
//
// A file compression tool that implements the Byte Pair Encoding algorithm.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bpe.h"
#include "compress.h"
#include "decompress.h"

Config config;

static void calculateOutputFileName()
{
    config.outfile = NULL;

    if(config.stdout_opt || (config.infile == NULL))
        return;

    if(config.compress_opt)
    {
        int len = strlen(config.infile) + 5;

        config.outfile = new char[len];

        strcpy(config.outfile, config.infile);
        strcat(config.outfile, ".bpe");
    }
    else if(config.decompress_opt)
    {
        char *last_dot = strrchr(config.infile, '.');
        if((last_dot == NULL) || (strcmp(last_dot, ".bpe") != 0))
        {
            int len = strlen(config.infile) + 6;

            config.outfile = new char[len];

            strcpy(config.outfile, config.infile);
            strcat(config.outfile, ".orig");
        }
        else
        {
            int len = last_dot - config.infile;

            config.outfile = new char[len + 1];

            strncpy(config.outfile, config.infile, len);
            config.outfile[len] = '\0';
        }
    }
}

static void usage(const char *name)
{
    fprintf(stderr,
"bpe: a file compressor that implements Byte Pair Encoding.\n"
"\n"
"Usage: %s OPTIONS [<file>]\n"
"\n"
"  -c --compress        Compress <file>. If -o option is not set, the output is written to the\n"
"                       file \"<file>.bpe\".\n"
"  -d --decompress      Decompress <file>.\n"
"  -o --stdout          Write output on standard output. Keep the original file unchanged.\n"
"  -b --block-size <n>  Block size (default: %d).\n"
"  -v --verbose         Verbose. Display the percentage reduction.\n"
"  -h --help            Output this help and exit.\n"
"\n"
"Examples:\n"
"  bpe -c -v test.txt\n"
"  bpe -d test.txt.bpe\n"
"\n",
    name, kDefaultBlockSize);
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
        else if((strcmp(argv[i], "-o") == 0) || (strcmp(argv[i], "--stdout") == 0))
        {
            config.stdout_opt = true;
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
                fprintf(stderr, "Unrecognized option: '%s'\n", argv[i]);
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

    if(!config.compress_opt && !config.decompress_opt)
    {
        fprintf(stderr, "Incorrect arguments: you have to specify either -c option or -d option\n");
        exit(1);
    }

    if(config.compress_opt && config.decompress_opt)
    {
        fprintf(stderr, "Incompatible options -c and -d\n");
        exit(1);
    }

    if(config.block_size_opt && !config.compress_opt)
    {
        fprintf(stderr, "-b option is only compatible with -c option\n");
        exit(1);
    }

    if(config.verbose_opt && !config.compress_opt)
    {
        fprintf(stderr, "-v option is only compatible with -c option\n");
        exit(1);
    }

    if((config.block_size < kMinBlockSize) || (config.block_size > kMaxBlockSize))
    {
        fprintf(stderr, "Incorrect block size. It must be in the range [%d, %d]\n",
            kMinBlockSize, kMaxBlockSize);
        exit(1);
    }

    if(lastargs != 1)
    {
        fprintf(stderr, "Bad number of arguments. Use -h to display more information\n");
        exit(1);
    }

    config.infile = argv[lastopt++];

    calculateOutputFileName();
}

int main(int argc, char **argv)
{
    config.help_opt = false;
    config.compress_opt = false;
    config.decompress_opt = false;
    config.stdout_opt = false;
    config.block_size_opt = false;
    config.verbose_opt = false;
    config.block_size = kDefaultBlockSize;
    config.infile = NULL;
    config.outfile = NULL;

    int lastopt = parseOptions(argc, argv);

    validateOptions(argc, argv, lastopt);

    if(config.compress_opt)
        compress();
    else if(config.decompress_opt)
        decompress();

    return 0;
}
