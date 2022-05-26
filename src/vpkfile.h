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


typedef struct ValveVpkHeader {
    uint32_t  magic;                  // VPK_MAGIC
    uint16_t  version_major;          // VALVE_VPK_MAJOR (2)
    uint16_t  version_minor;          // VALVE_VPK_MINOR (0)
    uint32_t  tree_length;
    uint32_t  embedded_chunk_length;  // 0 if "*_dir.vpk"
    uint32_t  chunk_hashes_length;
    uint32_t  self_hashes_length;     // 48 ?
    uint32_t  signature_size;
    // Next comes the tree...
} ValveVpkHeader_t;


typedef struct ValveVpkChunkMetadata {  // embed chunk structures
    uint32_t  checksum;        // crc32
    uint16_t  preload_length;  // 0x0000 ?
    uint16_t  archive_index;   // 0x7FFF ?
    uint32_t  archive_offset;  // file offset - tree & header sizes
    uint32_t  file_length;
    uint16_t  suffix;          // 0xFFFF ? block terminator?
} ValveVpkChunkMetadata;


/////////////////
// Respawn VPK //
/////////////////
typedef struct RespawnVpkHeader {
    uint32_t  magic;          // VPK_MAGIC
    uint16_t  version_major;  // RESPAWN_VPK_MAJOR (2)
    uint16_t  version_minor;  // RESPAWN_VPK_MINOR (3)
    uint32_t  directory_size;
    uint32_t  embedded_chunk_size;
    // uint32_t  self_hashes_size;
    // uint32_t  signature_size;
} RespawnVpkHeader_t;


typedef struct RespawnVpkDirEntry {
    uint32_t  flags_1;
    uint16_t  flags_2;
    uint64_t  offset;
    uint64_t  disk_size;  // compressed
    uint64_t  size;       // decompressed (should be under 0x100000?)
} RespawnVpkDirEntry_t;


typedef struct RespawnVpkDirEntryBlock {
    uint32_t  checksum;
    uint16_t  length;
    uint16_t  file_index;
    // DirEntry, uint16_t, DirEntry, uint16_t; until uint16_t == 0xFFFF;
} RespawnVpkDirEntryBlock_t;

#endif
