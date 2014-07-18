// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>

#include "decompress.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <vector>
#include "bpe.h"
#include "endian.h"

using namespace std;

extern Config config;

FILE *infile=NULL, *outfile=NULL;

static void closeAndExit(int exit_code)
{
    if(infile != NULL)
        fclose(infile);

    if((outfile != NULL) && !config.stdout_opt)
        fclose(outfile);

    if((exit_code == 0) && !config.stdout_opt)
        remove(config.infile);

    exit(exit_code);
}

static int replaceBytes(
    uint8_t *buffer,
    int len,
    const vector<pair<uint8_t, uint16_t> > &dictionary)
{
    list<uint8_t> byte_block(buffer, buffer+len);
    list<uint8_t>::iterator it;
    uint8_t byte, bytepair1, bytepair2;
    int dict_size = dictionary.size();
    int data_size;

    for(int i=0; i<dict_size; ++i)
    {
        byte = dictionary[i].first;
        bytepair1 = dictionary[i].second >> 8;
        bytepair2 = dictionary[i].second & 0xFF;

        it = byte_block.begin();
        while(it != byte_block.end())
        {
            if(*it == byte)
            {
                it = byte_block.erase(it);
                byte_block.insert(it, bytepair1);
                byte_block.insert(it, bytepair2);
            }
            else
                ++it;
        }
    }

    data_size = byte_block.size();
    copy(byte_block.begin(), byte_block.end(), buffer);

    return data_size;
}

void decompress()
{
    uint8_t dict_size, byte;
    uint16_t data_size, bytepair;
    size_t read_size;
    uint8_t buffer[kMaxBlockSize];

    infile = fopen(config.infile, "rb");
    if(infile == NULL)
    {
        perror(config.infile);
        closeAndExit(1);
    }

    if(config.stdout_opt)
    {
        outfile = stdout;
    }
    else
    {
        outfile = fopen(config.outfile, "wb");
        if(outfile == NULL)
        {
           perror(config.outfile);
           exit(1);
        }
    }

    do
    {
        vector<pair<uint8_t, uint16_t> > dictionary;

        read_size = fread(&dict_size, 1, 1, infile);

        if(feof(infile))
            break;

        if(read_size != 1)
        {
            fprintf(stderr, "%s: Bad format\n", config.infile);
            closeAndExit(1);
        }

        for(int i=0; i<dict_size; ++i)
        {
            read_size = fread(&byte, 1, 1, infile);
            if(read_size != 1)
            {
                fprintf(stderr, "%s: Bad format\n", config.infile);
                closeAndExit(1);
            }

            if(!fread16(bytepair, infile))
            {
                fprintf(stderr, "%s: Bad format\n", config.infile);
                closeAndExit(1);
            }

            dictionary.push_back(make_pair(byte, bytepair));
        }

        if(!fread16(data_size, infile))
        {
            fprintf(stderr, "%s: Bad format\n", config.infile);
            closeAndExit(1);
        }

        read_size = fread(buffer, 1, data_size, infile);
        if(read_size != data_size)
        {
            fprintf(stderr, "%s: Bad format\n", config.infile);
            closeAndExit(1);
        }

        if(dict_size > 0)
            data_size = replaceBytes(buffer, data_size, dictionary);

        fwrite(buffer, 1, data_size, outfile);

    } while(!feof(infile));

    closeAndExit(0);
}
