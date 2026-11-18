#include "lmp.h"
/*
void lmpn_ntt_dif(limb_t *restrict a, size_t n, limb_t p, const limb_t *restrict wk) {
    assert(n > 0 && (n & (n - 1)) == 0); // n must be a power of 2
    assert(p <= LIMB_MAX / 2);
    // wk[0] = w^1, wk[1] = w^2, ..., wk[log_n-1] = w^(n/2)

    for (size_t len = n; len >= 2; len >>= 1) {
        limb_t w = *wk, r = limb_div(w, 0, p);
        for (size_t i = 0; i < n; i += len) {
            limb_t cur = 1;
            for (size_t j = i, mx = i + (len >> 1); j < mx; ++j) {
                limb_t u = a[j], v = a[j + (len >> 1)];

                // 蝶形
                limb_t s = u + v;
                if (s >= p) s -= p;
                a[j] = s;

                limb_t d = (u < v) ? (u - v + p) : (u - v);

                // d = d * cur mod p，用 Shoup
                d = d * w - limb_mul_hi(d, r) * p;
                if (d >= p) d -= p;
                a[j + (len >> 1)] = d;

                // cur *= w mod p，用 Shoup
                cur = cur * w - limb_mul_hi(cur, r) * p;
                if (cur >= p) cur -= p;
            }
        }
        ++wk;
    }
}*/

void lmpn_ntt_dif(limb_t *restrict a, size_t n, limb_t p, const limb_t *restrict wk) {
    assert(n > 0 && (n & (n - 1)) == 0);
    assert(p <= LIMB_MAX / 2);

    for (size_t len = n; len >= 2; len >>= 1) {
        limb_t w = *wk, rw = limb_div(w, 0, p);
        for (size_t i = 0; i < n; i += len) {
            limb_t cur = 1;
            limb_t rcur = limb_div(1, 0, p);   // floor(1 * 2^W / p)
            for (size_t j = i, mx = i + (len >> 1); j < mx; ++j) {
                limb_t u = a[j], v = a[j + (len >> 1)];

                // 蝶形
                limb_t s = u + v;
                if (s >= p) s -= p;
                a[j] = s;

                limb_t d = (u < v) ? (u - v + p) : (u - v);

                // d = d * cur mod p，用 Shoup（rcur 对应 cur）
                d = d * cur - limb_mul_hi(d, rcur) * p;
                if (d >= p) d -= p;
                a[j + (len >> 1)] = d;

                // cur *= w mod p，用 Shoup（rw 对应 w）
                cur = cur * w - limb_mul_hi(cur, rw) * p;
                if (cur >= p) cur -= p;
                rcur = limb_div(cur, 0, p);   // 更新 rcur
            }
        }
        ++wk;
    }
}