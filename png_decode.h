/*Header File*/
#pragma once
// standard library includes
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <algorithm>

// user-defined types

namespace png_decode
{


typedef struct chunk
{
    char type[5];
    uint32_t length;
    uint32_t crc;
    unsigned char* data;
} chunk_t;


// note: the array must already be in host-byte order
uint32_t byte_array_to_number(unsigned char *bytes);



} // namespace png_decode
