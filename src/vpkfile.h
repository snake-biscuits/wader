#ifndef VPKFILE_H
#define VPKFILE_H

// --- VPK File Structures --- //
// References:
// github.com/ValvePython/vpk
// github.com/squidgyberries/RSPNVPK (mrsteyk / taskinoz / squidyberries)
// forum.xentax.com/viewtopic.php?f=10&t=11208

#include <stdint.h>


#define VPK_MAGIC  0x55AA1234

#define VALVE_VPK_MAJOR  2
#define VALVE_VPK_MINOR  0

#define RESPAWN_VPK_MAJOR  2
#define RESPAWN_VPK_MINOR  3


////////////////
// Shared VPK //
////////////////
typedef struct VpkId {
    uint32_t      magic;  // VPK_MAGIC
    struct {
        uint16_t  major;  // VALVE/RESPAWN_VPK_MAJOR (2/2)
        uint16_t  minor;  // VALVE/RESPAWN_VPK_MINOR (0/3)
    } version;
} VpkId_t;


// MASTER STRUCTURE:
// VpkHeaderv2_x  (id & structure sizes)
// FilenameTree
// -- szExtension
// -- szDirectory
// -- szFilename
// NOTE: get 3 strings first, then any number (1..3) in a block
//       1 for new filename in dir, 2 for new dir, 3 for new ext
// -- MetaData / DirEntry (per-file)
// EmbeddedChunk (concatenate "*_%03d.vpk" if "*_dir.vpk" ?)

// "*_dir.vpk" are just the header & tree


///////////////
// Valve VPK //
///////////////
typedef struct VpkHeaderv2_0 {
    VpkId_t   id;
    uint32_t  tree_size;            // DirEntry / filename tree
    uint32_t  embedded_chunk_size;  // 0 if "*_dir.vpk"
    uint32_t  chunk_hashes_size;
    uint32_t  self_hashes_size;     // 48 ?
    uint32_t  signature_size;
} VpkHeaderv2_0_t;


typedef struct VpkDirEntryv2_0 {
    uint32_t  checksum;        // crc32 of data (bitmasked down to 32 bits)
    uint16_t  preload_size;    // almost always 0x0000
    uint16_t  archive_index;   // almost always 0x7FFF
    uint32_t  archive_offset;  // file offset - tree & header sizes
    uint32_t  file_size;
    uint16_t  suffix;          // always 0xFFFF (ensure read alignment)
} VpkDirEntryv2_0_t;


/////////////////
// Respawn VPK //
/////////////////
typedef struct VpkHeaderv2_3 {
    VpkId_t   id;
    uint32_t  tree_size;            // DirEntry / filename tree
    uint32_t  embedded_chunk_size;  // 0 for '*_dir.vpk'
} VpkHeaderv2_3_t;


typedef struct VpkDirEntryv2_3 {
    uint32_t  flags_1;    // what flags? what do they affect?
    uint16_t  flags_2;
    uint64_t  offset;
    uint64_t  disk_size;  // compressed
    uint64_t  size;       // decompressed (should be under 0x100000?)
} VpkDirEntryv2_3_t;


typedef struct RespawnVpkDirEntryBlock {
    uint32_t  checksum;
    uint16_t  size;
    uint16_t  file_index;
    // DirEntry, uint16_t, DirEntry, uint16_t; until uint16_t == 0xFFFF;
} RespawnVpkDirEntryBlock_t;


#endif
