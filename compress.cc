// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>

#include "compress.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <list>
#include <unordered_set>
#include <vector>
#include "bpe.h"
#include "endian.h"

using namespace std;

struct BytePairOccurs
{
    uint8_t byte1;
    uint8_t byte2;
    int occurs;
};

extern Config config;

static void replaceBytes(
    uint8_t byte,
    uint8_t bytepair1,
    uint8_t bytepair2,
    list<uint8_t> &byte_block,
    vector<vector<int> > &byte_pairs,
    BytePairOccurs &best_byte_pair)
{
    list<uint8_t>::iterator it1 = byte_block.begin();
    list<uint8_t>::iterator it2 = next(it1);
    list<uint8_t>::iterator prev_it, next_it;

    best_byte_pair.occurs = 0;

    while(it2 != byte_block.end())
    {
        if((*it1 == bytepair1) && (*it2 == bytepair2))
        {
            // Update byte pairs
            prev_it = prev(it1);
            next_it = next(it2);

            --byte_pairs[bytepair1][bytepair2];

            if(prev_it != byte_block.end())
            {
                --byte_pairs[*prev_it][bytepair1];
                int occurs = ++byte_pairs[*prev_it][byte];
                if(occurs > best_byte_pair.occurs)
                {
                    best_byte_pair.byte1 = *prev_it;
                    best_byte_pair.byte2 = byte;
                    best_byte_pair.occurs = occurs;
                }
            }

            if(next_it != byte_block.end())
            {
                --byte_pairs[bytepair2][*next_it];
                int occurs = ++byte_pairs[byte][*next_it];
                if(occurs > best_byte_pair.occurs)
                {
                    best_byte_pair.byte1 = byte;
                    best_byte_pair.byte2 = *next_it;
                    best_byte_pair.occurs = occurs;
                }
            }

            // Perform the replacement
            it1 = byte_block.erase(it1, next_it);
            byte_block.insert(it1, byte);

            it2 = next(it1);
        }
        else
        {
            ++it1;
            ++it2;
        }
    }
}

static void compressBlock(
    uint8_t *buffer,
    int len,
    list<uint8_t> &byte_block,
    vector<pair<uint8_t, uint16_t> > &dictionary)
{
    unordered_set<uint8_t> unused_bytes;
    unordered_set<uint8_t>::iterator it_set;
    vector<vector<int> > byte_pairs(256, vector<int>(256, 0));
    BytePairOccurs best_byte_pair;
    uint8_t byte;

    best_byte_pair.byte1 = 0;
    best_byte_pair.byte2 = 0;
    best_byte_pair.occurs = 0;

    for(int i=0; i<256; ++i)
        unused_bytes.emplace(i);

    for(int i=0; i<len; ++i)
    {
        byte_block.push_back(buffer[i]);

        it_set = unused_bytes.find(buffer[i]);
        if(it_set != unused_bytes.end())
            unused_bytes.erase(it_set);

        if((i + 1) < len)
        {
            int occurs = ++byte_pairs[buffer[i]][buffer[i + 1]];
            if(occurs > best_byte_pair.occurs)
            {
                best_byte_pair.byte1 = buffer[i];
                best_byte_pair.byte2 = buffer[i + 1];
                best_byte_pair.occurs = occurs;
            }
        }
    }

    while(!unused_bytes.empty())
    {
        if(best_byte_pair.occurs < 4)
            break;

        it_set = unused_bytes.begin();
        byte = *it_set;

        dictionary.push_back(make_pair(byte, (best_byte_pair.byte1 << 8) | best_byte_pair.byte2));

        replaceBytes(
            byte,
            best_byte_pair.byte1,
            best_byte_pair.byte2,
            byte_block,
            byte_pairs,
            best_byte_pair);

        unused_bytes.erase(it_set);
    }
}

static int writeBlock(
    FILE *outfile,
    const list<uint8_t> &byte_block,
    const vector<pair<uint8_t, uint16_t> > &dictionary)
{
    uint8_t dict_size = dictionary.size();
    uint16_t data_size = byte_block.size();
    uint8_t buffer[data_size];
    int block_size = 3 + 3 * dict_size + data_size;

    fwrite(&dict_size, 1, 1, outfile);

    for(int i=dict_size-1; i>=0; --i)
    {
        fwrite(&(dictionary[i].first), 1, 1, outfile);
        fwrite16(dictionary[i].second, outfile);
    }

    fwrite16(data_size, outfile);

    copy(byte_block.begin(), byte_block.end(), buffer);
    fwrite(buffer, 1, data_size, outfile);

    return block_size;
}

void compress()
{
    uint8_t buffer[config.block_size];
    size_t read_size;
    long long total_size=0, total_compressed_size=0;

    FILE *infile = fopen(config.infile, "rb");
    if(infile == NULL)
    {
        perror(config.infile);
        exit(1);
    }

    FILE *outfile = NULL;
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
        read_size = fread(buffer, sizeof(uint8_t), config.block_size, infile);
        total_size += read_size;

        list<uint8_t> byte_block;
        vector<pair<uint8_t, uint16_t> > dictionary;
        compressBlock(buffer, read_size, byte_block, dictionary);

        total_compressed_size += writeBlock(outfile, byte_block, dictionary);

    } while(!feof(infile));

    if(config.verbose_opt)
    {
        double reduction = (total_size - total_compressed_size) / (double)total_size;
        fprintf(stderr, "Reduction: %.2f%%\n", reduction * 100.0);
    }

    fclose(infile);

    if(!config.stdout_opt)
    {
        fclose(outfile);

        remove(config.infile);
    }
}
