/* 
 * libbase32 version 1.0
 * Copyright (C) 2015 Trustifier Inc.
 * Copyright (C) 2015 Ahmed Masud
 * All rights reserved.
 * Created by Ahmed Masud on 6/15/15.
 */

#include <string.h>
#include <stdint.h>
#include "base32.h"

static int encode_block(char *dest, const void *_src);

static const char base32_encoding[] = {
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', /* 10 */
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',           /* 18 */
    'J', 'K',                                         /* 20 */
    'M', 'N',                                         /* 22 */
    'P', 'Q', 'R', 'S', 'T',                          /* 27 */
    'V', 'W', 'X', 'Y', 'Z'                           /* 32 */
};

static const char padding = '=';

/*
 *    11111111 10101010 10110011 10111100 10010100
 *->  11111 11110 10101 01011 00111 01111 00100 10100
 */

static int encode_block(char *dest, const void *_src)
{
    const unsigned char *src = _src;      /* cast it to a unsigned char */
    dest[0] = base32_encoding[(src[0] >> 3) & 0x1F];                          /* first 5 bits */
    dest[1] = base32_encoding[(src[0] << 2) & 0x1C | ((src[1] >> 6) & 0x3)];  /* last 3 bits + next 2 bits */
    dest[2] = base32_encoding[(src[1] >> 1) & 0x1F];                          /* next 5 bits (tail has 1 bit) */
    dest[3] = base32_encoding[(src[1] << 4) & 0x10 | ((src[2] >> 4) & 0xF)];  /* last bit + next four bits */
    dest[4] = base32_encoding[(src[2] << 1) & 0x1E | ((src[3] >> 7) & 0x1)];
    dest[5] = base32_encoding[(src[3] >> 2) & 0x1F];
    dest[6] = base32_encoding[(src[3] << 3) & 0x18 | ((src[4] >> 5) & 0x7)];
    dest[7] = base32_encoding[(src[4] & 0x1F)];
    return 0;
}

static int encode_tail(char *dest, const void *_src, size_t len)
{
    int k;
    const unsigned char *src = _src;

    if(len < 1 || len > 5)
        return -1;

    for(k = 0 ; k < 8; k++ )
        dest[k] = padding;

    switch(len) {
    case 5:
        dest[7] = base32_encoding[(src[4]) & 0x1F];
    case 4:
        dest[6] = base32_encoding[(src[3] << 3) & 0x18 | (dest[7] != padding ? (src[4] >> 5) & 0x7 : 0)];
        dest[5] = base32_encoding[(src[3] >> 2) & 0x1F];
    case 3:
        dest[4] = base32_encoding[(src[2] << 1) & 0x1E | (dest[5] != padding ? (src[3] >> 7) & 0x1 : 0)];
    case 2:
        dest[3] = base32_encoding[(src[1] << 4) & 0x10 | (dest[4] != padding ? (src[2] >> 4) & 0xF : 0)];
        dest[2] = base32_encoding[(src[1] >> 1) & 0x1F];
    case 1:
        dest[1] = base32_encoding[(src[0] << 2) & 0x1C | (dest[2] != padding ? (src[1] >> 6 & 0x3) : 0)];
        dest[0] = base32_encoding[(src[0] >> 3) & 0x1F];
        break;
    default:
        die(127, "Internal error, length passed to encode_tail MUST BE between 1 and 5\n");
        break;
    }

    return 0;
}

size_t base32enc(char *dest, const void *_src, size_t ssize)
{
    int dk = 0, sk = 0;
    const unsigned char *src = _src;


    if ( ssize >= 0 && ssize <= 5 ) {
        encode_tail(dest, src, ssize);
        dest[8] = 0;
        return 0;
    }

    ssize -= 5;

    for(dk = 0, sk = 0; sk <= ssize; sk+=5, dk+=8) {
        encode_block(&dest[dk], &src[sk]);
    }
    ssize += 5;

    if (ssize > sk) {
        encode_tail(&dest[dk], &src[sk], ssize - sk);
        dk += 8;
    }

    dest[dk] = 0;

    return dk;
}

static uint8_t index_of(char c)
{
    int idx;

    if (!c)
        die(128, "Encountered null character in encoded string, bad format\n");

    if(c >= '0' && c <= '9')
        idx = c - '0';
    else if (c >= 'A' && c <= 'H')
        idx = c - 'A' + 10;         /* no I */
    else if (c >= 'J' && c <= 'K')  /* no L */
        idx = c - 'J' + 18;
    else if (c >= 'M' && c <= 'N')  /* no O */
        idx = c - 'M' + 20;
    else if (c >= 'P' && c <= 'T')  /* no U */
        idx = c - 'P' + 22;
    else if (c >= 'V' && c <= 'Z')
        idx = c - 'V' + 27;
    else if (c == '=')
        idx = 0;
    else
       die(128, "character [%d] out of bounds\n", c);

#if defined(DEBUG)
    fprintf(stderr, "I think that index_of(%c) == %d whereas base32_encoding[%d] = %c\n", c, idx, idx, base32_encoding[idx]);
#endif

    return idx;
}

static int decode_block(unsigned char *dest, const void *_src)
{
    uint8_t idx[8] = {};
    const unsigned char * src = _src;
    int k;
    int end = 0;
    for (k = 0; k < 8; k++) {
        if (src[k] == 0  || src[k] == padding) {
            end = 1;
            break;
        }
        idx[k] = index_of(src[k]);
    }


    dest[0] = (uint8_t)(((idx[0] << 3) & 0xF8) | ((idx[1] >> 2) & 0x7));
    dest[1] = (uint8_t)(((idx[1] << 6) & 0xC0) | ((idx[2] << 1) & 0x3E) | ((idx[3] >> 4) & 0x1));
    dest[2] = (uint8_t)(((idx[3] << 4) & 0xF0) | ((idx[4] >> 1) & 0xF));
    dest[3] = (uint8_t)(((idx[4] << 7) & 0x80) | ((idx[5] << 2) & 0x7C) | ((idx[6] >> 3) & 0x3));
    dest[4] = (uint8_t)(((idx[6] << 5) & 0xE0) | ((idx[7] & 0x1F)));

    if (end) {
        switch(k) {
            case 0:
                end = ((idx[0] << 3) & 0xF8) ? 1 : 0;
                break;
            case 1:
                end = ((idx[1] << 6) & 0xC0) ? 2 : 1;
                break;
            case 2:
                end = 2;
                break;
            case 3:
                end = ((idx[3] << 4) & 0xF0 ) ? 3 : 2;
                break;
            case 4:
                end = ((idx[4] << 7) & 0x80 ) ? 4 : 3;
                break;
            case 5:
                end = 4;
                break;
            case 6:
                end = ((idx[6] << 5) & 0xE0 ) ? 5 : 4;
                break;
            case 7:
                end = 5;
                break;

        }
    }
    return end;
}


size_t base32dec(void *dest, size_t buf_size, const char * src) {
    int sk, dk;
    size_t src_len;
    src_len = strlen(src);

    if(buf_size <= 5)  {
    	char small_buf[5]; /* small buffer for 1-5 bytes stuff */
    	decode_block(small_buf, src);
    	memcpy(dest, small_buf, buf_size);
    	return 0;
    }

    for(sk=0, dk=0; sk < src_len; sk+=8, dk+=5) {
        int end = decode_block((unsigned char *)dest+dk, &src[sk]);
        if ( end ) {
            dk += end - 1;
            break;
        }
    }

    return dk;
}

size_t base32encsize(size_t count)
{
    size_t size = -1;

    size = count << 3; /* multiply by 8 */

    if ( size % 5 == 0 )
        return size/5;
    else
        return 1+size/5;
}

size_t base32decsize(size_t count)
{
    size_t size = -1;
    size = count * 5;
    return (size >> 3) + (size % 8) ? 1 : 0;
}
