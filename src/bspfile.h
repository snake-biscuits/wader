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

#define QUAKE_VERSION     29  // includes Hexen II
#define GOLDSRC_VERSION   30  // includes Blue Shift
#define REMAKE_VERSION  BSP2

#define LUMP_MIP_TEXTURES  2


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
