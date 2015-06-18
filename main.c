/* 
 * libbase32 version 1.0
 * Copyright (C) 2015 Trustifier Inc.
 * All rights reserved.
 * See LICENSE for details on distribution
 */

#include <string.h>
#include <asm-generic/errno-base.h>
#include "base32.h"

enum {
    ENCODE,
    DECODE
} mode;

static int fencode(FILE *in, FILE *out);
static int fdecode(FILE *in, FILE *out);

int main(int argc, char *argv[])
{
    FILE *in, *out;

    if(strstr(argv[0], "dec") != 0)
        mode = DECODE;
    else
        mode = ENCODE;

    if ( mode == ENCODE ) {
        in = stdin;
        out = stdout;
    }
    else {
        in = stdin;
        out = stdout;
    }
    if ( argc < 2 )
        mode == ENCODE ? fencode(in, out) : fdecode(in, out);

    return 0;
}

char buf[255];

static int fencode(FILE *in, FILE *out)
{
    off_t pos;
    char *encbuf = NULL;
    size_t buflen;
    size_t count;

    pos = 0;
    while(!feof(in)) {
        char *ptr;
        count = fread(buf, 1, sizeof(buf), in);
        if ( count <= 0 )
            return -1;

        buflen = base32encsize(count);

        ptr = realloc(encbuf, pos+buflen);

        if ( !ptr )
            die(ENOMEM, "Out of memory while requesting realloc");

        encbuf = ptr;

        buflen = base32enc(encbuf+pos, buf, count);

        fwrite(encbuf+pos, 1, buflen, out);
        pos += buflen;
    }

    return 0;
}

static int fdecode(FILE *in, FILE *out)
{
    off_t buflen;
    off_t count;
    char *decbuf = NULL;
    char *ptr = NULL;
    off_t pos;

    pos = 0;
    while(!feof(in)) {

        count = fread(buf, 1, sizeof(buf), in);
        if ( count <= 0 )
            return -1;

        ptr = realloc(decbuf, count+pos);

        if ( !ptr )
            die(ENOMEM, "Out of memory in fdecode");

        decbuf = ptr;
        memcpy(decbuf+pos, buf, count);
        pos += count;
    }

    ptr = malloc(base32decsize(pos));
    buflen = base32dec(ptr, pos, decbuf); // buflen is exact
    fwrite(ptr, 1, buflen, out);
    return 0;
}
