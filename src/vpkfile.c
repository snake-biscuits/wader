#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "common.h"
#include "vpkfile.h"


#ifdef VPK_MAIN
void print_help() {
  printf("Usage: vpk [OPTIONS] VPKFILE...\n");
  printf("  -h    print help and exit\n");
};


int main(int argc, char* argv[]) {
    int              i;
    bool             respawn_format;
    FILE            *vpk;
    VpkId_t          vpk_id;
    VpkHeaderv2_0_t  vpk_header_valve;
    VpkHeaderv2_3_t  vpk_header_respawn;

    respawn_format = false;

    goto end_table_jmp;
args_error_jmp:
    fprintf(stderr, "invalid argument: %s\n", argv[i]);
help_jmp:
    print_help();
    return 0;
end_table_jmp:


    // handle options
    if (argc == 1) { goto help_jmp; }
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (strlen(argv[1]) != 2)
                goto args_error_jmp;
            switch (argv[i][1]) {
                case 'h':  goto help_jmp;       break;
                default:   goto args_error_jmp; break;
            }
        } else
            break;
    };

    #define VPKFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, argv[i], 1)
    #define READ_OK(dest, Type)        READ_OK_BASE(dest, Type, vpk)

    for (; i < argc; i++) {
        vpk = fopen(argv[i], "r");
        VPKFILE_ASSERT(!vpk, "Couldn't open %s!\n")
        printf("Parsing: %s\n", argv[i]);
        VPKFILE_ASSERT(READ_OK(vpk_id, VpkId_t), "%s is empty!\n")
        VPKFILE_ASSERT(vpk_id.magic != VPK_MAGIC, "%s is not a known .vpk format!\n")
        if (vpk_id.version.major == VALVE_VPK_MAJOR && vpk_id.version.minor == VALVE_VPK_MINOR)
            respawn_format = false;
        else if (vpk_id.version.major == RESPAWN_VPK_MAJOR && vpk_id.version.minor == RESPAWN_VPK_MINOR)
            respawn_format = true;
        else {
            fprintf(stderr, "%s is not a known .vpk format (v%d.%d)\n", argv[i], vpk_id.version.major, vpk_id.version.minor);
            return 1;
        }

        printf("%-22s = %08X\n",  "magic", vpk_id.magic);
        printf("%-22s = %d.%d\n", "version", vpk_id.version.major, vpk_id.version.minor);
        fseek(vpk, 0, SEEK_SET);
        if (!respawn_format) {  // Valve format
            VPKFILE_ASSERT(READ_OK(vpk_header_valve, VpkHeaderv2_0_t), "Failed to read %s header!\n")
            #define PRINT_ATTR(fmt, attr)  printf("%-22s = " fmt "\n", #attr, vpk_header_valve.attr);
            PRINT_ATTR("%d", tree_size)
            PRINT_ATTR("%d", embedded_chunk_size)
            PRINT_ATTR("%d", chunk_hashes_size)
            PRINT_ATTR("%d", self_hashes_size)
            PRINT_ATTR("%d", signature_size)
            #undef PRINT_ATTR
            // TODO: collect DirEntry tree & filenames
        } else {  // Titanfall 1 format
            VPKFILE_ASSERT(READ_OK(vpk_header_respawn, VpkHeaderv2_3_t), "Failed to read %s header!\n")
            #define PRINT_ATTR(fmt, attr)  printf("%-22s = " fmt "\n", #attr, vpk_header_respawn.attr);
            PRINT_ATTR("%d", tree_size)
            PRINT_ATTR("%d", embedded_chunk_size)
            #undef PRINT_ATTR
            // TODO: collect DirEntry tree & filenames
        }

        #undef READ_OK
        CLOSE_FILE(vpk, argv[i])
        #undef VPKFILE_ASSERT
    };

    return 0;
}
#endif
