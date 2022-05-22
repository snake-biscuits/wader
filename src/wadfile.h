#ifndef WADFILE_H
#define WADFILE_H

// --- WAD File Structures --- //
// References:
// -- github.com/Id-Software/Quake/QW/client/wad.h (GPLv2+)
// -- github.com/Id-Software/Quake/QW/client/wad.c (GPLv2+)
// -- gamers.org/dEngine/quake/spec/quake-spec34/qkspec_7.htm
// -- hlbsp.sourceforge.net/index.php?content=waddef

#include <stdint.h>

#include "common.h"


#define WAD2 FILE_MAGIC('W', 'A', 'D', '2')  // Textures
#define WAD3 FILE_MAGIC('W', 'A', 'D', '3')  // untested; should function the same

#define LUMP_NONE          0
#define LUMP_LABEL         1
#define LUMP_PALETTE      64
#define LUMP_QTEXTURE     65  // QTEX
#define LUMP_QPICTURE     66  // QPIC
#define LUMP_SOUND        67
#define LUMP_MIP_TEXTURE  68  // MIPTEX

#define COMPRESSION_NONE  0
#define COMPRESSION_LZSS  1  // Lempel–Ziv–Storer–Szymanski (LZ77 Extension)
// Public Domain LZSS interpreter written by Haruhiko Okumura (奥村 晴彦 Okumura Haruhiko) in 1989:
// oku.edu.mie-u.ac.jp/~okumura/compression/lzss.c

//////////
// File //
//////////

typedef struct WadInfo {
    int32_t  magic;  // should be WAD2 for Quake
    int32_t  num_lumps;
    int32_t  table_offset;  // start of LumpInfo_t[num_lumps]
} WadInfo_t;


typedef struct LumpInfo {
    int32_t  offset;
    int32_t  disk_size;  // < size if compressed
    int32_t  size;
    int8_t   lump_type;
    int8_t   compression;
    int16_t  padding;
    char     name[16];  // null-terminated; name may denote subtype
} LumpInfo_t;


///////////
// Lumps //
///////////

typedef struct QPicture {
    int32_t  width;
    int32_t  height;
    char    *data;  // &uint8_t[width * height]; indexes Palette
} QPicture_t;


typedef struct Palette {  // unchanged since DOOM
    uint8_t  colour[3][256];  // 256 RGB_888 Colours
} Palette_t;

// TODO: QTexture, Sound


#endif
