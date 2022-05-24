#ifndef COMMON_H
#define COMMON_H

// --- Common Utilities --- //

#include <stdint.h>


#define FILE_MAGIC(a, b, c, d)  (d << 24 | c << 16 | b << 8 | a << 0)
// FILE IO helpers (<stdio.h> & <errno.h>)
#define FILE_ASSERT_BASE(cond, msg, filename, ret)  if (cond) { fprintf(stderr, msg, filename); return ret; }
#define READ_OK_BASE(dest, Type, file)  !(fread((char*) &dest, sizeof(Type), 1, file) - sizeof(Type))
#define CLOSE_FILE(file, filename) \
        errno = 0; \
        if (!fclose(file) && errno) { \
            int errsv = errno; \
            fprintf(stderr, "Couldn't close %s; errno = %d\n", filename, errsv); \
            return errsv; \
        };    


typedef struct MipTexture {
    char     name[16];  // null-terminated; may hold more data after the first NULL (start frame etc.)
    // *name    is animated like lava / water (texture scrolling)
    // +Xname   has up to 10 animation frames; where X is from '0' to '9'
    // {name    is transparent?
    // skyname  is animated like the sky (texture is split [A|B] & parallax scrolls)
    int32_t  width;
    int32_t  height;
    // NOTE: exact texture size varies based on type / animations
    int32_t  full;     // char*; len = (width / 1) * (length / 1) * 3  // RGB_888 pixels
    int32_t  half;     // char*; len = (width / 2) * (length / 2) * 3
    int32_t  quarter;  // char*; len = (width / 4) * (length / 4) * 3
    int32_t  eighth;   // char*; len = (width / 8) * (length / 8) * 3
} MipTexture_t;


#endif
