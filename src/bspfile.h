#ifndef BSPFILE_H
#define BSPFILE_H

// --- BSP File Formats --- //
// Games / Engine Branches Supported:
// -- GoldSrc
// -- Half-Life: Blue Shift
// -- Hexen II
// -- ReMakeQuake
// -- Quake

#include <stdint.h>


#define BSP2 FILE_MAGIC('B', 'S', 'P', '2')  // ReMakeQuake

#define QUAKE_VERSION      29  // includes Hexen II
#define GOLDSRC_VERSION    30  // includes Blue Shift
#define REMAKE_VERSION   BSP2

#define BSP_VERSION_OK(h) h.version == QUAKE_VERSION || h.version == GOLDSRC_VERSION || h.version == REMAKE_VERSION

#define MIP_TEXTURES                  2  // bsp lump header index
#define MAX_MIP_LEVELS                4  // number of pointers after name & size
#define MAX_MIP_TEXTURES            512
#define MAX_MIP_TEXTURES_SIZE  0x200000  // max byte-size of lump


typedef struct LumpHeader {
    uint32_t  offset;
    uint32_t  length;
} LumpHeader_t;


typedef struct BspHeader {
    uint32_t      version;
    LumpHeader_t  lumps[15];
} BspHeader_t;


typedef struct MipTextureHeader {
    uint32_t  count;
    uint32_t *offsets;  // uint32_t[count]; offsets to MipTextures in lump
} MipTextureHeader_t;


#endif
