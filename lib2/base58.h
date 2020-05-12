#ifndef LIBBASE58_H
#define LIBBASE58_H


#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>


#ifdef __cplusplus
extern "C" {
#endif

extern bool (*sha256_impl)(void *, const void *, size_t);

extern bool base58_decode(void *bin, size_t *binsz, const char *b58, size_t b58sz);
extern int base58_check(const void *bin, size_t binsz, const char *b58, size_t b58sz);

extern bool base58_encode(char *b58, size_t *b58sz, const void *bin, size_t binsz);
extern bool base58_check_encode(char *b58c, size_t *b58c_sz, uint8_t ver, const void *data, size_t datasz);

#ifdef __cplusplus
}
#endif

#endif
