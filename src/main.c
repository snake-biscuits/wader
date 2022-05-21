#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wadfile.h"


int main(int argc, char **argv) {
    int         i;
    FILE       *wad;
    WadInfo_t   wad_header;
    LumpInfo_t *wad_lumps;

    printf("WADer: WAD reader\n");
    // wader OPTION ... FILE ...
    // TODO: catch invalid calls fast
    // TODO: print help if argc == 1
    for (i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-", 1)) {
            printf("Option: %s not yet implemented\n", argv[i]);
            // TODO: strcmp option & select additional args
            // -- or hash string and use a switch enum
            // Options:
            //  -h         help
            //  -l         list names
            //  -e target  get data of target (LumpInfo.name stricmp)
            //  -o output  destination file for data
            //  -i index   same as -e, but go straight to this index
            //
            //  -a; -aVAL; -a=VAL; -a VAL; many arg types to support
        } else
            break;
    }

    #define FILE_ASSERT(cond, msg) \
        if (cond) { \
            fprintf(stderr, msg, argv[i]); \
            return 1; \
        }

    for (; i < argc; i++) {
        wad = fopen(argv[i], "r");
        FILE_ASSERT(!wad, "Couldn't open %s\n")
        printf("Reading %s:\n", argv[i]);

        #define READ_OK(dest, Type)  !(fread((char*) &dest, sizeof(Type), 1, wad) - sizeof(Type))

        FILE_ASSERT(READ_OK(wad_header, WadInfo_t), "%s is too short to be valid\n")
        FILE_ASSERT(!(wad_header.magic == WAD2), "%s is not a Quake .wad (WAD2)\n")
        fseek(wad, wad_header.table_offset, SEEK_SET);
        wad_lumps = malloc(sizeof(LumpInfo_t) * wad_header.num_lumps);
        for (int j = 0; j < wad_header.num_lumps; j++) {
            FILE_ASSERT(READ_OK(wad_lumps[j], LumpInfo_t), "%s is too short to be valid\n")
            // TODO: print data user requested (case switch set by options?)
#if 0
            printf("Lump %d:\n", j);
            #define PRINT_ATTR(format, attr)  printf("\t" #attr " = " format "\n", wad_lumps[j].attr);
            PRINT_ATTR("%d", offset)
            PRINT_ATTR("%d", disk_size)
            PRINT_ATTR("%d", size)
            PRINT_ATTR("%hhd", lump_type)
            // TODO: int -> "LUMP_*" macro
            PRINT_ATTR("%hhd", compression)
            // TODO: int -> "COMPRESSION_*" macro
            PRINT_ATTR("%s", name)
            #undef PRINT_ATTR
#endif
        };

        // TODO: any options that require all headers to be read first

        #undef READ_OK

        free(wad_lumps);

        errno = 0;  // fclose might return -1; implementations vary
        if (!fclose(wad) && errno) {
            int errsv = errno;
            fprintf(stderr, "Couldn't close %s; errno = %d\n", argv[i], errsv);
            return errsv;
        };
    };

    #undef FILE_ASSERT

    return 0;
};
