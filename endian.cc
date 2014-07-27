// Copyright (c) 2014 Vicente Romero Calero
//
// Distributed under the MIT software license, see the file LICENSE
//
// Author: Vicente Romero <vteromero@gmail.com>

#include "endian.h"

bool isBigEndian()
{
    uint16_t i = 1;
    uint8_t *p = (uint8_t *) &i;
    return (p[0] == 0);
}

bool fwrite8(uint8_t value, FILE *file)
{
    return (fwrite(&value, 1, 1, file) == 1);
}

bool fwrite16(uint16_t value, FILE *file)
{
    if(isBigEndian())
    {
        return (fwrite(&value, 2, 1, file) == 1);
    }
    else
    {
        uint8_t aux[2];

        aux[0] = (value >> 8) & 0xff;
        aux[1] = value & 0xff;

        return (fwrite(aux, 1, 2, file) == 2);
    }
}

bool fwrite32(uint32_t value, FILE *file)
{
    if(isBigEndian())
    {
        return (fwrite(&value, 4, 1, file) == 1);
    }
    else
    {
        uint8_t aux[4];

        aux[0] = (value >> 24) & 0xff;
        aux[1] = (value >> 16) & 0xff;
        aux[2] = (value >> 8) & 0xff;
        aux[3] = value & 0xff;

        return (fwrite(aux, 1, 4, file) == 4);
    }
}

bool fread8(uint8_t &value, FILE *file)
{
    return (fread(&value, 1, 1, file) == 1);
}

bool fread16(uint16_t &value, FILE *file)
{
    if(isBigEndian())
    {
        return (fread(&value, 2, 1, file) == 1);
    }
    else
    {
        uint8_t aux[2];

        size_t count = fread(aux, 1, 2, file);
        if(count != 2)
            return false;

        value = (aux[0] << 8) | aux[1];

        return true;
    }
}

bool fread32(uint32_t &value, FILE *file)
{
    if(isBigEndian())
    {
        return (fread(&value, 4, 1, file) == 1);
    }
    else
    {
        uint8_t aux[4];

        size_t count = fread(aux, 1, 4, file);
        if(count != 4)
            return false;

        value = (aux[0] << 24) | (aux[1] << 16) | (aux[2] << 8) | aux[3];

        return true;
    }
}

