#include "lmp.h"
#include "impl.h"
#include <string.h>

static size_t stein(limb_t *g, limb_t a[2], limb_t b[2]) {
    #define move(a) do {                                                \
        if (!a[0]) {a[0] = a[1] >> limb_ctz(a[1]); a[1] = 0;}           \
        else if (!(a[0] & 1)) {                                         \
            int ctz = limb_ctz(a[0]);                                   \
            a[0] = (a[0] >> ctz) | (a[1] << (LIMB_WIDTH - ctz));        \
            a[1] >>= ctz;                                               \
        }                                                               \
    } while(0)

    #define move_and_ctz(ctz, a) do {                                   \
        if (!a[0]) {                                                    \
            ctz = limb_ctz(a[1]);                                       \
            a[0] = a[1] >> ctz; a[1] = 0;                               \
            ctz += LIMB_WIDTH;                                          \
        } else if (!(a[0] & 1)) {                                       \
            ctz = limb_ctz(a[0]);                                       \
            a[0] = (a[0] >> ctz) | (a[1] << (LIMB_WIDTH - ctz));        \
            a[1] >>= ctz;                                               \
        }                                                               \
    } while(0)

    int ctza, ctzb;
    move_and_ctz(ctza, a);
    move_and_ctz(ctzb, b);
    if (ctza > ctzb) ctza = ctzb;
    while (1) {
        if (!a[1]) {
            g[0] = limb_gcd(b[1] ? limb_mod(b[1], b[0], a[0]) : b[0], a[0]);
            if (ctza) break; else return 1;
        }
        if (!b[1]) {
            g[0] = limb_gcd(limb_mod(a[1], a[0], b[0]), b[0]);
            if (ctza) break; else return 1;
        }
        if (a[0] == b[0] && a[1] == b[1]) {
            if (ctza) {
                g[0] = a[0] << ctza;
                g[1] = (a[0] >> (LIMB_WIDTH - ctza)) | (a[1] << ctza);
            } else {
                g[0] = a[0];
                g[1] = a[1];
            }
            return 2;
        }
        if (a[1] > b[1] || (a[1] == b[1] && a[0] > b[0])) {
            a[0] -= b[0];
            a[1] -= b[1] + (a[0] > b[0]);
            move(a);
        } else {
            b[0] -= a[0];
            b[1] -= a[1] + (b[0] > a[0]);
            move(b);
        }
    }
    #undef move
    #undef move1
    if (ctza >= LIMB_WIDTH) {
        g[1] = g[0] << (ctza - LIMB_WIDTH);
        g[0] = 0;
        return 2;
    }
    g[1] = g[0] >> (LIMB_WIDTH - ctza);
    g[0] <<= ctza;
    return 1 + (g[1] == 0);
}

static size_t lehmer(limb_t *g, limb_t *a, size_t n, limb_t *b, size_t m) {
    while (m) {
        /*
         [[A  B  x]]
         [[C  D  y]]
         sgn: ABCD
        */
        limb_t x, y, A, B, C, D, w, *tmp;
        int sgn = 8;
        while (n > m) {
            amodb:
            unlikely(lmpn_div_r(a, a, n, b, m)) return 0;
            n = lmpn_norm(a, m);
            if (n == 0) {
                memcpy(g, a, n*sizeof(limb_t));
                return n;
            }
            if (n == 1) {
                *g = lmpn_gcd_1(b, m, a[0]);
                return 1;
            }
            swap(n, m, size_t)
            swap(a, b, limb_t*)
        }
        assert(n == m);
        x=a[n-1]; y=b[m-1];
        if (!~y) goto amodb;
        w = (~x) ? (x+1) / y : LIMB_MAX / y + !(y & (y-1));
        if (w != x/(y+1)) goto amodb;
        A = 0; B = 1;  x = b[m-1];
        C = 1; D = -w; y = a[n-1] - w*b[m-1];
        /*
         8 = (1 0 0 0)_2
              v v v v
              D C B A
        */
        while (1) {
            limb_t At=A, Bt=B, xt=x; int sgnt=sgn;

            if ((C+y == 0) || (D+y == 0)) break;
            w = (x + A) / (y + C);
            if (w != (x + B) / (y + D)) break;

            A = C;         B = D;         x = y;

            sgn >>= 2;
            if (((sgnt&1) && !(sgnt&4)) || (!((sgnt&1) || (sgnt&4)) && At < w*C)) sgn |= 4; /* 0b0100 */
            else sgn &= 3;  /* 0b0011 */
            if (((sgnt&2) && !(sgnt&8)) || (!((sgnt&2) || (sgnt&8)) && Bt < w*D)) sgn |= 8; /* 0b1000 */
            else sgn &= 7;  /* 0b0111 */
 
            C = At - w*C;  D = Bt - w*D;  y = xt - w*y;
        }

        assert(sgn == 8 || sgn == 6 || sgn == 9);

        tmp = lmpn_alloc(n);
        unlikely(!tmp) return 0;
        
        if (sgn == 6) {
            // B, C < 0
            memcpy(tmp, a, n*sizeof(limb_t));
            lmpn_mul_1(a, tmp, n, A);
            lmpn_submul_1(a, b, n, -B);
            lmpn_mul_1(b, b, n, D);
            lmpn_submul_1(b, tmp, n, -C);
        } else {
            // D < 0
            memcpy(tmp, b, n*sizeof(limb_t));
            lmpn_mul_1(b, a, n, C);
            lmpn_submul_1(b, tmp, n, -D);
            if (sgn == 9) {
                // A < 0
                lmpn_mul_1(a, a, n, -A);
                lmpn_neg(a, a, n);
                lmpn_addmul_1(a, tmp, n, B);
            } else {
                lmpn_mul_1(a, a, n, A);
                lmpn_addmul_1(a, tmp, n, B);
            }
        }

        n = lmpn_norm(a, n);
        m = lmpn_norm(b, m);

        assert(n>=m && (n > m || lmpn_cmp(a, b, n) >= 0));
        free(tmp);

        if (m==1) {
            *g = lmpn_gcd_1(a, n, b[0]);
            return 1;
        }
    }
    memcpy(g, a, n*sizeof(limb_t));
    return n;
}

size_t lmpn_gcd(limb_t *g, limb_t *a, size_t n, limb_t *b, size_t m) {
    assert(n >= m && m >= 1);
    if (m == 1) {
        *g = lmpn_gcd_1(a, n, b[0]);
        return 1;
    }
    if (n == 2) return stein(g, a, b);
    return lehmer(g, a, n, b, m);
}
