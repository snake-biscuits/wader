#include <stdio.h>

#include "common.h"
#include "vpkfile.h"


#ifdef VPK_MAIN
int main(int argc, char* argv[]) {
    int               i;
    FILE             *vpk;
    ValveVpkHeader_t  vpk_header;

    if (argc == 1) {
        printf("Usage: vpk VPKFILE...\n");
        return 0;
    }

    #define VPKFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, argv[i], 1)
    #define READ_OK(dest, Type)        READ_OK_BASE(dest, Type, vpk)

    for (i = 1; i < argc; i++) {
        vpk = fopen(argv[i], "r");
        VPKFILE_ASSERT(!vpk, "Couldn't open %s!\n")
        VPKFILE_ASSERT(READ_OK(vpk_header, ValveVpkHeader_t), "Failed to read %s header!\n")

        printf("Parsing: %s\n", argv[i]);

        #define PRINT_ATTR(fmt, attr)  printf("%-22s = " fmt "\n", #attr, vpk_header.attr);
        PRINT_ATTR("%08X", magic)
        printf("%-22s = %d.%d\n", "version", vpk_header.version_major, vpk_header.version_minor);
        PRINT_ATTR("%d", tree_length)
        PRINT_ATTR("%d", embedded_chunk_length)
        PRINT_ATTR("%d", chunk_hashes_length)
        PRINT_ATTR("%d", self_hashes_length)
        PRINT_ATTR("%d", signature_size)
        #undef PRINT_ATTR

        #undef READ_OK
        CLOSE_FILE(vpk, argv[i])
        #undef VPKFILE_ASSERT
    };

    return 0;
}
#endif
