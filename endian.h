// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>
//
// Functions to deal with endianness

#ifndef ENDIAN_H_
#define ENDIAN_H_

#include <cstdint>
#include <cstdio>

bool isBigEndian();

bool fwrite8(uint8_t value, FILE *file);
bool fwrite16(uint16_t value, FILE *file);
bool fwrite32(uint32_t value, FILE *file);
bool fread8(uint8_t &value, FILE *file);
bool fread16(uint16_t &value, FILE *file);
bool fread32(uint32_t &value, FILE *file);

#endif // ENDIAN_H_

