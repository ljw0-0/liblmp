#ifndef lmpn_IMPL_H
#define lmpn_IMPL_H

#if __STDC_VERSION__ < 202311L
# if SIZE_MAX == 0xFFFFFFFFFFFFFFFF
#  define SIZE_WIDTH 64
# elif SIZE_MAX == 0xFFFFFFFF
#  define SIZE_WIDTH 32
# elif SIZE_MAX == 0xFFFF
#  define SIZE_WIDTH 16
# else
#  error "Unsupported type size_t."
# endif
#else
# include <limits.h>
#endif

#define log2n(n) ((n) >= 2) + ((n) >= 4) + ((n) >= 8) + ((n) >= 16) + ((n) >= 32) + ((n) >= 64) + ((n) >= 128) + ((n) >= 256)

#define MUL_BASECASE_MAX_UN      200
#define MUL_TOOM22_THRESHOLD     32
#define MUL_TOOM32_THRESHOLD     64
#define MUL_TOOM33_THRESHOLD     128
#define MUL_TOOM43_THRESHOLD     256
#define MUL_TOOM44_THRESHOLD     512
#define MUL_NTT_THRESHOLD        1024

#define NTT_START LMPN_NTT_START

#define cltag(x)  clean_##x             // clean tag
#define tfree(x)  cltag(x): free(x)     // tags and free

#if defined(__GNUC__) || defined(__clang__)
# define unlikely(x) if (__builtin_expect(!!(x), 0))
#else
# define unlikely(x) if (x)
#endif

#define swap(x, y, type) {type tmp##x##y = x; x = y; y = tmp##x##y;}

#endif