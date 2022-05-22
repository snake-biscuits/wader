#include <errno.h>
#include <fnmatch.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wadfile.h"


#define VERSION_MAJOR  0
#define VERSION_MINOR  1
#define VERSION_PATCH  0


void print_version() {
    printf("wader %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    printf("Copyright (C) 2022 snake-biscuits\n");
    printf("License LGPLv3+: GNU Lesser GPL version 3 or later <https://gnu.org/licenses/gpl.html>.\n");
    printf("This is free software: you are free to change and redistribute it.\n");
    printf("There is NO WARRANTY, to the extent permitted by law.\n");
}


void print_help() {
    printf("Usage: wader OPTION... WADFILE...\n");
    printf("  -h, --help               display help and exit\n");
    printf("  -v, --version            display version and exit\n");
    printf("  -o, --output FILEPATH    where to create generated files\n");
    printf("                           FOLDER for readER / WADFILE for makER\n");
    printf("  -V, --verbose            print lump headers\n");
    printf("WAD readER\n");
    printf("  -l, --list               list lump names and exit\n");
    printf("  -e, --extract PATTERN    extract lumps which match PATTERN\n");
    printf("                           PATTERN must include leading special characters!\n");
    // TODO: make --extract a flag & default PATTERN = "*"
    // TODO:  -p, --pattern PATTERN    only touch lumps which match pattern
    printf("  -i, --index INDEX        same as --extract, but extracts lump @ INDEX\n");  // ignores PATTERN
    // NOTE: INDEX & PATTERN are mutually exclusive; also, each may only be set once
    // TODO: WAD makER
    // TODO:  -b, --bsp BSPFILE...     make a WAD from each BSPFILE's embedded MIP_TEXTURES
    // TODO:  -f, --files FILES...     make a WAD from FILES (accepted types, conversions & output WADFILE)
}


void sanitise(char name[17]) {
    char tmp[17];

    switch (name[0]) {
        case '+':  // +Xname -> _Xname
        case '*':  // *name  -> _name
            strcpy(tmp, &name[1]);
            strcpy(name, "_");
            strcat(name, tmp);
        default:
            break;
    }
}


int main(int argc, char *argv[]) {
    int         i, j, k, index, wad_tell;
    FILE       *wad, *out;
    WadInfo_t   wad_header;
    LumpInfo_t *wad_lumps;
    bool        list, extract, verbose;
    char        pattern[64], outdir[4096 - 17], outfile[4096], lump_name[17], compression[5], lump_type[12];

    index = -1;
    list = extract = verbose = false;
    strcpy(outdir, "./");

// [OPTION]... jump table
#define ARGS_ASSERT(cond, msg) if (cond) { fprintf(stderr, msg); return 1; }
    goto end_jmp_table;
help_jmp:
    print_help();
    return 0;
version_jmp:
    print_version();
    return 0;
list_jmp:
    list = true;
    goto next_arg_jmp;
extract_jmp:
    ARGS_ASSERT(extract, "--extract cannot use more than one PATTERN!\n")
    extract = true;
    ARGS_ASSERT(++i == argc, "--extract has no NAME to extract!\n")
    strcpy(pattern, argv[i]);
    goto next_arg_jmp;
index_jmp:
    ARGS_ASSERT(index != -1, "--index cannot target more than one INDEX!\n")
    index = atoi(argv[i]);
    goto next_arg_jmp;
output_jmp:
    ARGS_ASSERT(strcmp(outdir, "./"), "--output cannot target more than one FOLDER!\n")
    ARGS_ASSERT(++i == argc, "--output has no FOLDER!\n")
    strcpy(outdir, argv[i]);
    strcat(outdir, "/");
    goto next_arg_jmp;
verbose_jmp:
    verbose = true;
    goto next_arg_jmp;
unknown_jmp:
    fprintf(stderr, "Unknown option: %s\n", argv[i]);
    goto next_arg_jmp;
end_jmp_table:
#undef ARGS_ASSERT

    // parse args: [OPTION]...
    if (argc == 1) { goto help_jmp; };
    for (i = 1; i < argc; i++) {
        if (!strncmp(argv[i], "-", 1)) {
            if (strlen(argv[1]) == 2) {  // short `-a` args
                switch (argv[i][1]) {
                    case 'h':  goto help_jmp;
                    case 'v':  goto version_jmp;
                    case 'l':  goto list_jmp;
                    case 'e':  goto extract_jmp;
                    case 'i':  goto index_jmp;
                    case 'o':  goto output_jmp;
                    case 'V':  goto verbose_jmp;
                    default:   goto unknown_jmp;
                }
            } else if (strncmp(argv[i], "--", 2)) {  // long `--argument` args
                #define IS_OPTION(option)  (strcmp(argv[i], option))
                if      IS_OPTION("--help")     goto help_jmp;
                else if IS_OPTION("--version")  goto version_jmp;
                else if IS_OPTION("--list")     goto list_jmp;
                else if IS_OPTION("--extract")  goto extract_jmp;
                else if IS_OPTION("--index")    goto index_jmp;
                else if IS_OPTION("--output")   goto output_jmp;
                else if IS_OPTION("--verbose")  goto verbose_jmp;
                else                            goto unknown_jmp;
                #undef IS_OPTION
            }
        } else
            break;  // end of [OPTION]...; WADFILE... begins
next_arg_jmp:
        continue;
    };

    // parse args: WADFILE...
    if (i == argc) {
        fprintf(stderr, "no WADFILE!\n");
        return 1;
    } else if (i == 1) {
        fprintf(stderr, "please select at least one OPTION\n");
        return 1;
    }

    #define WADFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, argv[i], 1)
    #define OUTFILE_ASSERT(cond, msg)  FILE_ASSERT_BASE(cond, msg, outfile, 1)
    #define READ_OK(dest, Type)        READ_OK_BASE(dest, Type, wad)

    for (; i < argc; i++) {
        wad = fopen(argv[i], "r");
        WADFILE_ASSERT(!wad, "Couldn't open %s\n")
        if (verbose)
            printf("Reading %s:\n", argv[i]);
        WADFILE_ASSERT(READ_OK(wad_header, WadInfo_t), "%s is too short to be valid\n")
        WADFILE_ASSERT(!(wad_header.magic == WAD2), "%s is not a Quake .wad (WAD2)\n")
        fseek(wad, wad_header.table_offset, SEEK_SET);
        wad_lumps = malloc(sizeof(LumpInfo_t) * wad_header.num_lumps);
        for (j = 0; j < wad_header.num_lumps; j++) {
            WADFILE_ASSERT(READ_OK(wad_lumps[j], LumpInfo_t), "%s is too short to be valid\n")
            if (list) {
                if (verbose) {
                    printf("Lump %d:\n", j);
                    #define PRINT_ATTR(format, attr)  printf("\t" #attr " = " format "\n", wad_lumps[j].attr);
                    PRINT_ATTR("%d", offset)
                    PRINT_ATTR("%d", disk_size)
                    PRINT_ATTR("%d", size)
                    switch (wad_lumps[j].lump_type) {
                        case LUMP_NONE:
                            strcpy(lump_type, "NONE");
                            break;
                        case LUMP_LABEL:
                            strcpy(lump_type, "LABEL");
                            break;
                        case LUMP_PALETTE:      strcpy(lump_type, "PALETTE");     break;
                        case LUMP_QTEXTURE:     strcpy(lump_type, "QTEXTURE");    break;
                        case LUMP_QPICTURE:     strcpy(lump_type, "QPICTURE");    break;
                        case LUMP_SOUND:        strcpy(lump_type, "SOUND");       break;
                        case LUMP_MIP_TEXTURE:  strcpy(lump_type, "MIP_TEXTURE"); break;
                        default:                strcpy(lump_type, "UNKNOWN");
                    }
                    printf("\tlump_type = %hhd (%s)\n", wad_lumps[j].lump_type, lump_type);
                    switch (wad_lumps[j].compression) {
                        case COMPRESSION_NONE:  strcpy(compression, "NONE"); break;
                        case COMPRESSION_LZSS:  strcpy(compression, "LZSS"); break;
                        default:                strcpy(compression, "????");
                    }
                    printf("\tcompression = %hhd (%s)\n", wad_lumps[j].compression, compression);
                    PRINT_ATTR("%s", name)
                    #undef PRINT_ATTR
                } else
                    printf("%s\n", wad_lumps[j].name);
            }
            if (extract) {
                if (fnmatch(pattern, wad_lumps[j].name, FNM_EXTMATCH)) {
                    strcpy(lump_name, wad_lumps[j].name);
                    sanitise(lump_name);
                    strcpy(outfile, outdir);
                    strcat(outfile, lump_name);
                    if (verbose)
                        printf("Writing to %s\n", outfile);
                    out = fopen(outfile, "w");
                    OUTFILE_ASSERT(!out, "Couldn't open %s\n")
                    wad_tell = ftell(wad);
                    fseek(wad, wad_lumps[j].offset, SEEK_SET);
                    for (k = 0; k < wad_lumps[j].size; k++) {
                        fputc(fgetc(wad), out);  // NOTE: no buffer, no checks;  probably very slow
                    };
                    fseek(wad, wad_tell, SEEK_SET);
                    CLOSE_FILE(out, outfile)
                }
            }
        };

        WADFILE_ASSERT(index >= wad_header.num_lumps, "INDEX is greater than %s num_lumps\n")
        else if (index != -1) {
            strcpy(lump_name, wad_lumps[index].name);
            sanitise(lump_name);
            strcpy(outfile, outdir);
            strcat(outfile, lump_name);
            out = fopen(outfile, "w");
            OUTFILE_ASSERT(!out, "Couldn't open %s\n")
            fseek(wad, wad_lumps[index].offset, SEEK_SET);
            for (k = 0; k < wad_lumps[index].size; k++) {
                fputc(fgetc(wad), out);  // NOTE: no buffer, no checks;  probably very slow
            };
            CLOSE_FILE(out, outfile)
        }
        #undef OUTFILE_ASSERT

        free(wad_lumps);
        #undef READ_OK

        CLOSE_FILE(wad, argv[i])
        #undef WADFILE_ASSERT
    };

    return 0;
};
