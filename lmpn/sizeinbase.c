#include "lmp.h"
#include <math.h>

size_t lmpn_sizeinbase(const limb_t *a, size_t n, int base) {
    // limb_clz 这里要求 a[n-1] != 0，但是 mpn 接口要求 a 的最高位非零，这是调用者的基本要求，否则该骂
    size_t bits = n*LIMB_WIDTH-limb_clz(a[n-1]);
    switch (base) {
        case 2:   return bits;
        case 4:   return bits + 1 >> 1;
        case 8:   return (bits + 2) / 3;
        case 16:  return bits + 3 >> 2;
        case 32:  return (bits + 4) / 5;
        case 64:  return (bits + 5) / 6;
        case 128: return (bits + 6) / 7;
        case 256: return bits + 7 >> 3;
        default:  return floor(bits/log2(base)) + 1;
    }
}