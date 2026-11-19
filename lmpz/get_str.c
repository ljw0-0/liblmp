#include "lmp.h"
#include "impl.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

char *lmpz_get_str(char *str, const lmpz_t z, int base) {
    unlikely (base >= 63 || base < -36 || base == -1 || base == 0 || base == 1) return NULL;
    char is_alloc, *d; // 用作 bool
    if (!str) {
        size_t len = lmpz_sizeinbase(z, base) + 1 + !!(z->size & lmpz_SIGN_BIT);
        str = malloc(len);
        if (LMP_UNLIKELY(!str)) return NULL;
        is_alloc = 1;
    } else is_alloc = 0;
    d = str;
    if (lmpz_is_zero(z)) {
        str[0] = '0';
        str[1] = 0;
        return str;
    }
    if (lmpz_sgn_bit(z)) {str[0] = '-'; ++d;}
    if (lmpz_size(z) == 1) {
        char *l;
        limb_t value = lmpz_get_ui(z);
        switch (base) {
            limb_t u;
            #if __STDC_VERSION__ >= 202311L
            case 2: case -2: sprintf(d, "%"PRIbPTR, value); return str;
            #else
            case 2: case -2: for (limb_t u=value;u;u>>=1) {*d = (u&1)+'0'; ++d;} break;
            #endif
            case 4: case -4: for (limb_t u=value;u;u>>=2) {*d = (u&3)+'0'; ++d;} break;
            case 8: case -8: sprintf(d, "%"PRIoPTR, value); return str;
            case 16: sprintf(d, "%"PRIxPTR, value); return str;
            case -16: sprintf(d, "%"PRIXPTR, value); return str;
            case 10: case -10: sprintf(d, "%"PRIuPTR, value); return str;
            case 32:
                for (u=value;u;u>>=5) {
                    *d = (u&31) <= 9 ? ((u&31)+'0') : ((u&31)-10+'a');
                    ++d;
                }
                break;
            case -32:
                for (u=value;u;u>>=5) {
                    *d = (u&31) <= 9 ? ((u&31)+'0') : ((u&31)-10+'A');
                    ++d;
                }
                break;
            case 3:case 5:case 6:case 7:case 9: for (u=value;u;u/=base) {*d = (u%base) + '0'; ++d;} break;
            case -3:case -5:case -6:case -7:case -9:
                base = -base;
                for (u=value;u;u/=base) {*d = (u%base) + '0'; ++d;}
                break;
            case 11:case 12:case 13:case 14:case 15:case 17:case 18:case 19:case 20:case 21:case 22:case 23:case 24:case 25:case 26:case 27:case 28:case 29:case 30:case 31:case 33:case 34:case 35:case 36:
                for (u=value;u;u/=base) {
                    int v = u % base;
                    *d = (v < 10) ? (v + '0') : (v - 10 + 'a');
                    ++d;
                }
                break;
            case -11:case -12:case -13:case -14:case -15:case -17:case -18:case -19:case -20:case -21:case -22:case -23:case -24:case -25:case -26:case -27:case -28:case -29:case -30:case -31:case -33:case -34:case -35:case -36:
                base = -base;
                for (u=value;u;u/=base) {
                    int v = u % base;
                    *d = (v < 10) ? (v + '0') : (v - 10 + 'A');
                    ++d;
                }
                break;
            default:
                for (u=value;u;u/=base) {
                    int v = u % base;
                    *d = (v < 10) ? (v + '0') : (v < 36) ? (v - 10 + 'a') : (v - 36 + 'A');
                    ++d;
                }
                break;
        }
        *d = 0;
        --d;
        l = str + !!(z->size & ((lmpz_SIZE_MAX >> 1) + 1));
        while (l < d) {
            char w = *l;
            *l = *d;
            *d = w;
            ++l;
            --d;
        }
    } else {
        char *conv = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        size_t sz;
        if (base > 1) {if (base <= 36) conv = "0123456789abcdefghijklmnopqrstuvwxyz";}
        else if (base >= -36) base = -base;
        sz = mpn_get_str((unsigned char *)d, lmpz_ptr(z), lmpz_size(z), base);
        if (sz == 0) {
            if (is_alloc) free(str);
            return NULL;
        }
        for (size_t i=0;i<sz;++i) d[i] = conv[(unsigned char)d[i]];
    }
    return str;
}