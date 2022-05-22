// --- Bsp File Reading --- //

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "bspfile.h"
#include "common.h"


// TODO: .bsp to .wad
int bsp_mip_textures(char *filename, MipTexture_t *out[]) {  // returns out_len;
    int                 i;
    FILE               *bsp;
    BspHeader_t         bsp_header;
    MipTextureHeader_t  miptex_header;
    MipTexture_t        miptex;

    #define FILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, filename, -1)
    #define READ_OK(dest, Type)     READ_OK_BASE(dest, Type, bsp)

    bsp = fopen(filename, "r");
    FILE_ASSERT(READ_OK(bsp_header, BspHeader_t), "%s is too short to be a bsp\n")
    FILE_ASSERT(bsp_header.version == 29 || bsp_header.version == 30 || bsp_header.version == BSP2, "%s format is unwadable\n")
    FILE_ASSERT(bsp_header.lumps[LUMP_MIP_TEXTURES].length > 0, "%s has no MipTextures\n")
    FILE_ASSERT(READ_OK(miptex_header, MipTextureHeader_t), "Couldn't read MipTextures from %s\n")

    // TODO: what are MipTexture offsets in a Wad relative to? the Wad? the MipTexture?
    out = malloc(sizeof(MipTexture_t*) * miptex_header.count);

    for (i = 0; i < miptex_header.count; i++) {
        FILE_ASSERT(READ_OK(miptex, MipTexture_t), "%s MipTexture Lump is malformed\n");
        // TODO: identify subtype
        // out[0] = malloc(sizeof(MipTexture_t) + full_len + half_len + quarter_len + eighth_len);
        // memcpy: miptex, full, half, quarter, eighth
    };

    #undef FILE_ASSERT
    #undef READ_OK

    return miptex_header.count;
};
