// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>
//
// A file compression tool that implements the Byte Pair Encoding algorithm.

#ifndef BPE_H_
#define BPE_H_

struct Config
{
    bool help_opt;
    bool compress_opt;
    bool decompress_opt;
    bool stdout_opt;
    bool block_size_opt;
    bool verbose_opt;
    int block_size;
    char *infile;
    char *outfile;
};

const int kMinBlockSize         = 100;
const int kMaxBlockSize         = 65535;
const int kDefaultBlockSize     = 8000;

#endif // BPE_H_
