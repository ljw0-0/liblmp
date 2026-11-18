#ifndef LIMB_IMPL_H
#define LIMB_IMPL_H

#if LIMB_MAX <= 0xFF
# define tmp_t uint16_t
# define stmp_t int16_t
#elif LIMB_MAX <= 0xFFFF
# define tmp_t uint32_t
# define stmp_t int32_t
#elif LIMB_MAX <= 0xFFFFFFFF
# define tmp_t uint64_t
# define stmp_t int64_t
#elif LIMB_MAX <= 0xFFFFFFFFFFFFFFFF && defined(__SIZEOF_INT128__)
# define tmp_t __uint128_t
# define stmp_t __int128_t
#elif __STDC_VERSION__ >= 202311L
# define tmp_t unsigned _BitInt(LIMB_WIDTH*2)
# define stmp_t signed _BitInt(LIMB_WIDTH*2)
#endif

#endif