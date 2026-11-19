#include "lmp.h"
#include "impl.h"
#include <ctype.h>
#include <string.h>

int lmpz_set_str(lmpz_t z, const char *str, int base) {
    int base_gt_36 = base > 36 ? 26 : 0, ret;
    size_t len, j = 0, size, sgn = 0;
    unsigned char *tmp;

    if (LMP_UNLIKELY(base > 62 || base == 1 || base < 0)) return LMP_EBASE;
    while (isspace(*str)) ++str;
    if (*str == '\0') {
        zero:
        lmpz_zero(z);
        return 0;
    }
    if (*str == '-') {
        sgn = LMPZ_SIGN_BIT;
        ++str;
    } else if (*str == '+') ++str;
    if (LMP_UNLIKELY(!isalnum(*str))) return LMP_EPARSE;
    if (*str == '\0') goto zero;
    if (base == 0) {
        if (*str == '0') {
            ++str;
            if (*str == 'x' || *str == 'X') {
                ++str;
                base = 16;
            } else if (*str == 'b' || *str == 'B') {
                ++str;
                base = 2;
            } else if (*str == 'o' || *str == 'O') {
                ++str;
                base = 8;
            } else if (*str == '\0') goto zero;
            else base = 8;
        } else base = 10;
    }
    if (*str == '\0') goto zero;
    while (isspace(*str)) ++str;
    if (LMP_UNLIKELY(*str == '_')) return LMP_EPARSE;
    len = strlen(str);
    while (len && isspace((unsigned char)str[len - 1])) --len;
    if (!len) goto zero;
    if (LMP_UNLIKELY(*str == '_')) return LMP_EPARSE;

    tmp = malloc(len);
    if (LMP_UNLIKELY(!tmp)) return LMP_ENOMEM;

    do {
        char c = *str;
        ++str;
        unsigned char val;
        if (c == '_' || isspace(c)) continue;
        switch (c) {
            #define CASE(x, y) case x: val = y; break;
            case 0: if (LMP_LIKELY(j)) {val=0; break;} else goto end;
            CASE('1', 1)
            CASE('2', 2)
            CASE('3', 3)
            CASE('4', 4)
            CASE('5', 5)
            CASE('6', 6)
            CASE('7', 7)
            CASE('8', 8)
            CASE('9', 9)
            CASE('A', 10)
            CASE('B', 11)
            CASE('C', 12)
            CASE('D', 13)
            CASE('E', 14)
            CASE('F', 15)
            CASE('G', 16)
            CASE('H', 17)
            CASE('I', 18)
            CASE('J', 19)
            CASE('K', 20)
            CASE('L', 21)
            CASE('M', 22)
            CASE('N', 23)
            CASE('O', 24)
            CASE('P', 25)
            CASE('Q', 26)
            CASE('R', 27)
            CASE('S', 28)
            CASE('T', 29)
            CASE('U', 30)
            CASE('V', 31)
            CASE('W', 32)
            CASE('X', 33)
            CASE('Y', 34)
            CASE('Z', 35)
            CASE('a', 10 + base_gt_36)
            CASE('b', 11 + base_gt_36)
            CASE('c', 12 + base_gt_36)
            CASE('d', 13 + base_gt_36)
            CASE('e', 14 + base_gt_36)
            CASE('f', 15 + base_gt_36)
            CASE('g', 16 + base_gt_36)
            CASE('h', 17 + base_gt_36)
            CASE('i', 18 + base_gt_36)
            CASE('j', 19 + base_gt_36)
            CASE('k', 20 + base_gt_36)
            CASE('l', 21 + base_gt_36)
            CASE('m', 22 + base_gt_36)
            CASE('n', 23 + base_gt_36)
            CASE('o', 24 + base_gt_36)
            CASE('p', 25 + base_gt_36)
            CASE('q', 26 + base_gt_36)
            CASE('r', 27 + base_gt_36)
            CASE('s', 28 + base_gt_36)
            CASE('t', 29 + base_gt_36)
            CASE('u', 30 + base_gt_36)
            CASE('v', 31 + base_gt_36)
            CASE('w', 32 + base_gt_36)
            CASE('x', 33 + base_gt_36)
            CASE('y', 34 + base_gt_36)
            CASE('z', 35 + base_gt_36)
            default:
                free(tmp);
                return LMP_EPARSE;
            #undef CASE
        }
        tmp[j] = val;
        ++j;
        end:
    } while (--len);

    /*
       * base**(j-1) <= z < base**j
       * B**n <= z < B**(n+1)
       * B**n < base**j, B**(n+1) > base**(j-1)
       * logB(base)*(j-1)-1 < n < logB(base)*j
    */

    ret = lmpz_reserve(z, lmpn_limbs_for_digits(j, base));
    if (LMP_UNLIKELY(ret)) {
        free(tmp);
        return ret;
    }

    size = lmpn_set_str(lmpz_limbs(z), tmp, j, base);
    free(tmp);

    if (LMP_UNLIKELY(!size)) return LMP_ENOMEM;

    z->size = size | sgn;

    return 0;
}