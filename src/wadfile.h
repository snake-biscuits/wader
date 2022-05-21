#ifndef WADFILE_H
#define WADFILE_H

// --- WAD File Structures --- //
// References:
// -- github.com/Id-Software/Quake/QW/client/wad.h
// -- github.com/Id-Software/Quake/QW/client/wad.c

#include <stdint.h>

#include "common.h"


#define WAD2 FILE_MAGIC('W', 'A', 'D', '2')  // Textures

#define LUMP_NONE      0
#define LUMP_LABEL     1
#define LUMP_PALETTE  64
#define LUMP_QTEX     65
#define LUMP_QPIC     66
#define LUMP_SOUND    67
#define LUMP_MIPTEX   68

#define COMPRESSION_NONE  0
#define COMPRESSION_LZSS  1


//////////
// File //
//////////

typedef struct {
    int32_t  magic;  // should be WAD2 for Quake
    int32_t  num_lumps;
    int32_t  table_offset;  // start of LumpInfo_t[num_lumps]
} WadInfo_t;


typedef struct {
    int32_t  offset;
    int32_t  disk_size;  // < size if compressed
    int32_t  size;
    int8_t   lump_type;
    int8_t   compression;
    int16_t  padding;
    char     name[16];  // null-terminated
} LumpInfo_t;


///////////
// Lumps //
///////////

typedef struct {
    int32_t  width;
    int32_t  height;
    char    *data;
} QPic_t;


typedef struct {
    char     name[16];
    int32_t  width;
    int32_t  height;
    char*    full;  // (width * length) * 3 (RGB_888)
    char*    half;  // (width * length) / 4 * 3
    char*    quarter;  // (width * length) / 16 * 3
    char*    eighth;  // (width * length) / 64 * 3
} MipTex_t;

#endif
