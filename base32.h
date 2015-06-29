/**
 * libbase32 version ${VERSION}
 * Copyright (C) 2015 Trustifier Inc.
 * All rights reserved.
 *
 * Distributed under MIT License
 *
 * Created by Ahmed Masud <ahmed.masud@trustifier.com>
 *
 * Simple implementation of Douglas Crockford's excellent base32
 * encoding/decoding scheme
 *
 * Canonical information about the scheme is found at:
 * http://www.crockford.com/wrmg/base32.html
 */


#ifndef _LIBBASE32_BASE32_H
#define _LIBBASE32_BASE32_H

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

static inline void die(int code, const char *fmt, ...)
    __attribute__((format(__printf__, 2, 3), noreturn));

static inline void die(int code, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(code);
}

/**
 * @function base32enc
 * Encodes s_len bytes from src to base32 and stores it in dest.
 *
 * @param char * dest  pointer to destination where encoding will be stored
 * @param const void * src   pointer to data to be encoded
 * @param size_t s_len length of data to be encoded
 * @return size of encoded string
 */

size_t base32enc(char *dest, const void *src, size_t s_len);

/**
 * @function base32dec
 *
 * Decodes {@link src} bytes into dest
 *
 *
 * This function decodes an arbitrary length source buffer into
 * destination buffer, until dest is filled.
 *
 *
 * @param dest      where decoded content will end up
 * @param dest_len  size of destination (data gets truncated)
 * @param src       encoded string
 * @return  0 on success, -1 on failure
 *
 * @todo Should return offset into src up to where decoding occured
 *
 */
size_t base32dec(void *dest, size_t dest_len, const char *src);

/*
 * @function base32encsize
 *
 * Calculate size of encoded text for a buffer of count size
 *
 * @param count
 *
 * @return size of encoded buffer
 */

size_t base32encsize(size_t count);

/* size of decoded text */
size_t base32decsize(size_t count);

#endif //LIBBASE32_BASE32_H
