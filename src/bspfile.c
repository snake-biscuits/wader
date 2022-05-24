// --- Bsp File Reading --- //

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bspfile.h"
#include "common.h"
#include "wadfile.h"


// TODO: .bsp to .wad
// TODO: dump .bsp MipTextures to raw files and then repack into a .wad
int bsp_to_wad(char *bsp_filename, char* wad_filename) {  // returns out_len;
    FILE               *bsp, *wad;
    BspHeader_t         bsp_header;
    int                 miptex_length, i, miptex_header_size, miptex_size;
    char               *miptex_lump;
    MipTextureHeader_t  miptex_header;
    WadInfo_t           wad_header;
    LumpInfo_t          lump_info;

    #define BSPFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, bsp_filename, 1)
    #define READ_OK(dest, Type)        READ_OK_BASE(dest, Type, bsp)

    bsp = fopen(bsp_filename, "r");
    BSPFILE_ASSERT(!bsp, "Couldn't read %s!\n")
    BSPFILE_ASSERT(READ_OK(bsp_header, BspHeader_t), "%s is too short to be a bsp\n")
    BSPFILE_ASSERT(BSP_VERSION_OK(bsp_header), "%s format is unwadable\n")
    miptex_length = bsp_header.lumps[MIP_TEXTURES].length;
    BSPFILE_ASSERT(miptex_length > 0, "%s has no MipTextures\n")
    BSPFILE_ASSERT(!fseek(bsp, bsp_header.lumps[MIP_TEXTURES].offset, SEEK_SET), "%s MIP_TEXTURES lump not found\n")
    miptex_lump = malloc(miptex_length);
    BSPFILE_ASSERT(!(fread(miptex_lump, sizeof(char), miptex_length, bsp) - miptex_length), "%s MIP_TEXTURES lump too short\n")

    memcpy(&miptex_header.count, miptex_lump, sizeof(uint32_t));
    miptex_header.offsets = malloc(sizeof(uint32_t) * miptex_header.count);
    memcpy(&miptex_header.offsets, &miptex_lump[sizeof(uint32_t)], sizeof(uint32_t) * miptex_header.count);
    miptex_header_size = sizeof(uint32_t) * (miptex_header.count + 1);

    #define WADFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, wad_filename, 1)

    wad = fopen(wad_filename, "r");
    WADFILE_ASSERT(!wad, "Couldn't write %s!\n")
    wad_header.magic        = WAD2;
    wad_header.num_lumps    = miptex_header.count;
    wad_header.table_offset = sizeof(WadInfo_t) + miptex_length;
    fwrite(&wad_header, sizeof(WadInfo_t), 1, wad);

    fwrite(&miptex_lump[miptex_header_size], sizeof(char), miptex_length - miptex_header_size, wad);
    // NOTE: MipTextures lump stores MipTexture_t, data (full, half, quarter, eight) in sequence like WADs do
    for (i = 0; i < miptex_header.count - 1; i++) {
        miptex_size = miptex_header.offsets[i + 1] - miptex_header.offsets[i];
        lump_info.offset      = miptex_header.offsets[i] - miptex_header_size + sizeof(WadInfo_t);
        lump_info.disk_size   = miptex_size;
        lump_info.size        = miptex_size;
        lump_info.lump_type   = LUMP_MIP_TEXTURE;
        lump_info.compression = COMPRESSION_NONE;
        lump_info.padding     = 0x0000;
        memcpy(&lump_info.name[0], &miptex_lump[miptex_header.offsets[i]], sizeof(char) * 16);
        fwrite(&lump_info, sizeof(LumpInfo_t), 1, wad);
    }
    if (miptex_header.count) {  // final MipTexture
        miptex_size = miptex_header.offsets[i] - miptex_length;
        lump_info.offset      = miptex_header.offsets[i] - miptex_header_size + sizeof(WadInfo_t);
        lump_info.disk_size   = miptex_size;
        lump_info.size        = miptex_size;
        lump_info.lump_type   = LUMP_MIP_TEXTURE;
        lump_info.compression = COMPRESSION_NONE;
        lump_info.padding     = 0x0000;
        memcpy(&lump_info.name[0], &miptex_lump[miptex_header.offsets[i]], sizeof(char) * 16);
        fwrite(&lump_info, sizeof(LumpInfo_t), 1, wad);
    }

    CLOSE_FILE(bsp, bsp_filename)
    CLOSE_FILE(wad, wad_filename)
    #undef BSPFILE_ASSERT
    #undef READ_OK

    return miptex_header.count;
};
