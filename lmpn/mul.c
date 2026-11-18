#include "lmp.h"
#include "impl.h"
#include <string.h>

// Algorithm

static void basecase(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    memset(c, 0, sizeof(limb_t)*(n+m));
    for (size_t i=0;i<n;++i) {
        limb_t carry = 0;
        for (size_t j=0;j<m;++j) {
            limb_t hi, lo;
            limb_mul(&hi, &lo, a[i], b[j]);
            c[i+j] += carry + lo;
            carry = hi + (carry+lo < lo) + (c[i+j] < carry+lo);
            assert(carry != 0 || hi < LIMB_MAX-1);
        }
        c[i+m] = carry;
    }
}

static int toom22(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    assert(n < m*2);

    // 处理小数字
    //if (m < MUL_TOOM22_THRESHOLD) {
        //basecase(c, a, n, b, m);
        //return;
    // }

    size_t ha = n >> 1, l = n - ha, hb = m - l;
    // therefore, hb <= ha <= l

    assert(ha <= l);
    assert(hb <= l);

    /*
     * a = a1 * 2 ^ l + a0
     * b = b1 * 2 ^ l + b0
     * a*b = a1*b1 * 2^(2l) + (a1*b0 + a0*b1) * 2^(l) + a0*b0
     * (a1*b0 + a0*b1) = (a0+a1) * (b0+b1) - a0*b0 - a1*b1
     */

    const limb_t *a1 = a + l, *b1 = b + l;

    limb_t *a0pa1 = lmpn_alloc(l+1);
    unlikely(!a0pa1) {return 1;}
    limb_t *b0pb1 = lmpn_alloc(l+1);
    unlikely(!b0pb1) {clean1: free(a0pa1); return 1;}

    a0pa1[l] = lmpn_add(a0pa1, a, l, a1, ha);
    b0pb1[l] = lmpn_add(b0pb1, b, l, b1, hb);

    limb_t *pmul = lmpn_alloc((l+1)<<1);
    unlikely(!pmul) {free(b0pb1); goto clean1;}

    int r = lmpn_mul_n(pmul, a0pa1, b0pb1, l+1);

    free(a0pa1);
    free(b0pb1);

    unlikely(r) goto clean;

    r = lmpn_mul_n(c, a, b, l);
    unlikely(r) goto clean;

    r = lmpn_mul(c+(l<<1), a1, ha, b1, hb);
    unlikely(r) goto clean;

    lmpn_sub(pmul, pmul, (l<<1)+2, c,       (l<<1));
    lmpn_sub(pmul, pmul, (l<<1)+2, c+(l<<1), ha+hb);

    lmpn_add(c+l, c+l, n+m-l, pmul, (l<<1)+2);

    clean:
    free(pmul);
    return r;
}

static int toom33(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    assert(n < m + (m/2));

    size_t l = (n + 2) / 3, ha = n - (l<<1), hb = m - (l<<1);
    
    assert(ha <= l && hb <= l);

    /*
     * a = a2*2^(2l) + a1*2^l + a0
     * b = b2*2^(2l) + b1*2^l + b0
     * a*b = (a2*b2)*2^(4l) + (a2*b1+a1*b2)*2^(3l) + (a2*b0+a1*b1+a0*b2)*2^(2l) + (a1*b0+a0*b1)*2^l + a0*b0
     *     = c4 * 2^4l + c3 * 2^3l + c2 * 2^2l + c1 * 2^l + c0
     * 
     * p0   = a0
     * p1   = a0+a1+a2
     * pm1  = a0-a1+a2
     * p2   = a0+2*a1+4*a2
     * pinf = a2
     * 
     * q0   = b0
     * q1   = b0+b1+b2
     * qm1  = b0-b1+b2
     * q2   = b0+2*b1+4*b2
     * qinf = b2
     * 
     * v0   = p0*q0
     * v1   = p1*q1
     * vm1  = pm1*qm1
     * v2   = p2*q2
     * vinf = pinf*qinf
     * 
     * c0 = v0
     * c4 = vinf
     * c2 = (v1 + vm1) / 2 - c0 - c4
     * c3 = (v2 - 16*c4 - 4*c2 - c0 - (v1 - vm1)) / 6
     * c1 = (v1 - vm1) / 2 - c3
     */

    const limb_t *a1 = a + l, *a2 = a + (l<<1);
    const limb_t *b1 = b + l, *b2 = b + (l<<1);

    limb_t *v0 = lmpn_alloc(l<<1);
    unlikely(!v0)  return 1;
    limb_t *v1 = lmpn_alloc((l<<1)+2);
    unlikely(!v1)  goto cltag(v0);
    limb_t *vm1 = lmpn_alloc((l<<1)+2);
    unlikely(!vm1) goto cltag(v1);
    limb_t *v2 = lmpn_alloc((l<<1)+2);
    unlikely(!v2)  goto cltag(vm1);
    limb_t *vinf = lmpn_alloc(ha+hb);
    unlikely(!vinf) goto cltag(v2);

    limb_t *p1 = lmpn_alloc(l+1);
    unlikely(!p1)  goto cltag(vinf);
    limb_t *pm1 = lmpn_alloc(l+1);
    unlikely(!pm1) goto cltag(p1);
    limb_t *p2 = lmpn_alloc(l+1);
    unlikely(!p2)  goto cltag(pm1);
    limb_t *q1 = lmpn_alloc(l+1);
    unlikely(!q1)  goto cltag(p2);
    limb_t *qm1 = lmpn_alloc(l+1);
    unlikely(!qm1) goto cltag(q1);
    limb_t *q2 = lmpn_alloc(l+1);

    unlikely(!q2) {
        tfree(q2);
        tfree(qm1);
        tfree(q1);

        tfree(p2);
        tfree(pm1);
        tfree(p1);

        tfree(vinf);
        tfree(v2);
        tfree(vm1);
        tfree(v1);
        tfree(v0);
        return 1;
    }

    // p1, pm1, and p2
    pm1[l] = lmpn_add(pm1, a, l, a2, ha);
    lmpn_add(p1, pm1, l+1, a1, l);
    int sgn = lmpn_sub(pm1, pm1, l+1, a1, l);
    if (sgn) lmpn_neg(pm1, pm1, l+1);

    p2[l] = lmpn_addlsh1_n(p2, a, a1, l);
    lmpn_addlsh2(p2, p2, l+1, a2, ha);

    // q1, qm1, and q2
    qm1[l] = lmpn_add(qm1, b, l, b2, hb);
    lmpn_add(q1, qm1, l+1, b1, l);
    if (lmpn_sub(qm1, qm1, l+1, b1, l)) {lmpn_neg(qm1, qm1, l+1); sgn ^= 1;}

    q2[l] = lmpn_addlsh1_n(q2, b, b1, l);
    lmpn_addlsh2(q2, q2, l+1, b2, hb);

    // mul

    unlikely(lmpn_mul_n(v0, a, b, l) || lmpn_mul_n(v1, p1, q1, l+1) || lmpn_mul_n(vm1, pm1, qm1, l+1) || lmpn_mul_n(v2, p2, q2, l+1) || lmpn_mul(vinf, a2, ha, b2, hb)) goto cltag(q2);

    free(p1);
    free(pm1);
    free(p2);
    free(q1);
    free(qm1);
    free(q2);

    assert(v1[(l<<1)+1] == 0);
    assert(vm1[(l<<1)+1] == 0);
    assert(v2[(l<<1)+1] == 0);

    size_t t = (l<<1)+1;

    limb_t *c2 = lmpn_alloc(t);
    unlikely(!c2) goto cltag(vinf);
    // c0 will be stored in v0, c1 will be stored in v1, c3 will be stored in v2, c4 will be stored in vinf

    if (sgn) {
        lmpn_sub_n(c2, v1, vm1, t);
        lmpn_add_n(v1, v1, vm1, t);
    } else {
        lmpn_add_n(c2, v1, vm1, t);
        lmpn_sub_n(v1, v1, vm1, t);
    }
    free(vm1);
    lmpn_rshift(c2, c2, t, 1);
    c2[l<<1] -= lmpn_sub_n(c2, c2, v0, l<<1);
    lmpn_sub(c2, c2, t, vinf, ha+hb);

    assert(c2[l<<1] <= 2);

    lmpn_sublsh(v2, v2, t, vinf, ha+hb, 4);
    lmpn_sublsh2_n(v2, v2, c2, t);
    lmpn_sub(v2, v2, t, v0, l<<1);
    lmpn_sub_n(v2, v2, v1, t);

    //printf("testing toom33\n");
    lmpn_divexact_1(v2, v2, t, 6);
    //printf("testing toom33 end\n");

    lmpn_rshift(v1, v1, t, 1);
    lmpn_sub_n(v1, v1, v2, t);

    memset(c, 0, (n+m)*sizeof(limb_t));
    memcpy(c, v0, (l<<1)*sizeof(limb_t));                     // [c, c+2*l)
    lmpn_add(c+l,      v1,   t, c+l, l);      // [c+l   ~ c+l+t)
    lmpn_add(c+(l<<1), c2,   t, c+(l<<1), l+1); // [c+2*l ~ c+2*l+t)
    lmpn_add(c+(l*3),  v2,   t, c+l*3,    l+1); // [c+3*l ~ c+3*l+t)
    lmpn_add(c+(l<<2), vinf, ha+hb, c+(l<<2), l+1); // 因为最高位太小，因此进位是不可能的

    free(c2);
    free(v0);
    free(v1);
    free(vinf);

    return 0;
}

static int toom44(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    assert(n >= 4);
    assert(n - m/3 <= m - 4);

    size_t l = (n+3) >> 2, ha = n - 3*l, hb = m - 3*l;
    assert(hb <= ha);
    assert(ha <= l);

    const limb_t *a1 = a + l, *a2 = a + 2*l, *a3 = a + 3*l;
    const limb_t *b1 = b + l, *b2 = b + 2*l, *b3 = b + 3*l;

    limb_t *p1 = lmpn_alloc(l+1);
    unlikely(!p1) return 1;
    limb_t *pm1 = lmpn_alloc(l+1);
    unlikely(!pm1) goto cltag(p1);
    limb_t *p2 = lmpn_alloc(l+1);
    unlikely(!p2) goto cltag(pm1);
    limb_t *pm2 = lmpn_alloc(l+1);
    unlikely(!pm2) goto cltag(p2);
    limb_t *p12 = lmpn_alloc(l+1);
    unlikely(!p12) goto cltag(pm2);
    limb_t *q1 = lmpn_alloc(l+1);
    unlikely(!q1) goto cltag(p12);
    limb_t *qm1 = lmpn_alloc(l+1);
    unlikely(!qm1) goto cltag(q1);
    limb_t *q2 = lmpn_alloc(l+1);
    unlikely(!q2) goto cltag(qm1);
    limb_t *qm2 = lmpn_alloc(l+1);
    unlikely(!qm2) goto cltag(q2);
    limb_t *q12 = lmpn_alloc(l+1);
    unlikely(!q12) goto cltag(qm2);
    limb_t *tmp = lmpn_alloc(l+1);

    unlikely(!tmp) {
        tfree(tmp);
        tfree(q12); tfree(qm2); tfree(q2); tfree(qm1); tfree(q1);
        tfree(p12); tfree(pm2); tfree(p2); tfree(pm1); tfree(p1);
        return 1;
    }

    tmp[l] = lmpn_add(tmp, a1, l, a3, ha);
    pm1[l] = lmpn_add_n(pm1, a, a2, l);
    lmpn_add_n(p1, pm1, tmp, l+1);
    int sgn1 = lmpn_sub_n(pm1, pm1, tmp, l+1);
    if (sgn1) lmpn_neg(pm1, pm1, l+1);

    tmp[l] = lmpn_addlsh2(tmp, a1, l, a3, ha);
    pm2[l] = lmpn_addlsh2_n(pm2, a, a2, l);
    lmpn_addlsh1_n(p2, pm2, tmp, l+1);
    int sgn2 = lmpn_sublsh1_n(pm2, pm2, tmp, l+1);
    if (sgn2) lmpn_neg(pm2, pm2, l+1);

    tmp[l] = lmpn_add(tmp, b1, l, b3, hb);
    qm1[l] = lmpn_add_n(qm1, b, b2, l);
    lmpn_add_n(q1, qm1, tmp, l+1);
    if (lmpn_sub_n(qm1, qm1, tmp, l+1)) {
        lmpn_neg(qm1, qm1, l+1);
        sgn1 ^= 1;
    }

    tmp[l] = lmpn_addlsh2(tmp, b1, l, b3, hb);
    qm2[l] = lmpn_addlsh2_n(qm2, b, b2, l);
    lmpn_addlsh1_n(q2, qm2, tmp, l+1);
    if (lmpn_sublsh1_n(qm2, qm2, tmp, l+1)) {
        lmpn_neg(qm2, qm2, l+1);
        sgn2 ^= 1;
    }

    free(tmp);

    p12[l] = lmpn_addlsh1_n(p12, a1, a, l);    // p12 = a1 + 2*a0 = 2a0 + a1
    p12[l] = 2*p12[l] + lmpn_addlsh1_n(p12, a2, p12, l); // p12 = 2*(2a0+a1) + a2
    lmpn_lshift(p12, p12, l+1, 1);
    lmpn_add(p12, p12, l+1, a3, ha);

    q12[l] = lmpn_addlsh1_n(q12, b1, b, l);    // q12 = b1 + 2*b0 = 2b0 + b1
    q12[l] = 2*q12[l] + lmpn_addlsh1_n(q12, b2, q12, l); // q12 = 2*(2b0+b1) + b2
    lmpn_lshift(q12, q12, l+1, 1);
    lmpn_add(q12, q12, l+1, b3, hb);

    limb_t *v0 = lmpn_alloc(l<<2);
    unlikely(!v0) goto cltag(q12);
    limb_t *v1 = lmpn_alloc((l+1)<<1);
    unlikely(!v1) goto cltag(v0);
    limb_t *vm1 = lmpn_alloc((l+1)<<1);
    unlikely(!vm1) goto cltag(v1);
    limb_t *v2 = lmpn_alloc((l+1)<<1);
    unlikely(!v2) goto cltag(vm1);
    limb_t *vm2 = lmpn_alloc((l+1)<<1);
    unlikely(!vm2) goto cltag(v2);
    limb_t *v12 = lmpn_alloc((l+1)<<1);
    unlikely(!v12) goto cltag(vm2);
    limb_t *vinf = lmpn_alloc(ha+hb);

    unlikely(!vinf) {
        tfree(vinf); tfree(v12); tfree(vm2); tfree(v2); tfree(vm1); tfree(v1); tfree(v0);
        goto cltag(q12);
    }

    unlikely(lmpn_mul_n(v0, a, b, l)   ||
        lmpn_mul_n(v1, p1, q1, l+1)    ||
        lmpn_mul_n(vm1, pm1, qm1, l+1) ||
        lmpn_mul_n(v2, p2, q2, l+1)    ||
        lmpn_mul_n(vm2, pm2, qm2, l+1) ||
        lmpn_mul_n(v12, p12, q12, l+1) ||
        lmpn_mul(vinf, a3, ha, b3, hb)) goto cltag(vinf);

    free(p1); free(pm1); free(p2); free(pm2); free(p12);
    free(q1); free(qm1); free(q2); free(qm2); free(q12);

    assert(v1[(l<<1)+1] == 0);
    assert(vm1[(l<<1)+1] == 0);
    assert(v2[(l<<1)+1] == 0);
    assert(vm2[(l<<1)+1] == 0);
    assert(v12[(l<<1)+1] == 0);

    size_t t = (l+1)<<1;
    limb_t *c1 = lmpn_alloc(t);
    unlikely(!c1) goto clt;
    limb_t *c2 = lmpn_alloc(t);
    unlikely(!c2) goto cltag(c1);
    limb_t *c3 = lmpn_alloc(t);
    unlikely(!c3) goto cltag(c2);
    limb_t *c4 = lmpn_alloc(t);
    unlikely(!c4) goto cltag(c3);
    limb_t *c5 = lmpn_alloc(t);

    unlikely(!c5) {
        tfree(c5); tfree(c4); tfree(c3); tfree(c2); tfree(c1);
        clt: free(vinf); free(v12); free(vm2); free(v2); free(vm1); free(v1); free(v0);
        return 1;
    }

    // 完善一下
    /*
    c5 = v12 + v2
    c1 = (v2 - vm2)/2
    c4 = v2 - v0
    c4 = (c4 - c1)/4 - vinf*16
    c3 = (v1 - vm1)/2
    c2 = v1 - c3

    c5 = c5 - c2*65
    c2 = c2 - vinf - v0
    c5 = (c5 + c2*45)/2
    c4 = (c4 - c2)/3
    c2 = c2 - c4

    c1 = c5 - c1
    c5 = (c5 - c3*8)/9
    c3 = c3 - c5
    c1 = (c1/15 + c5)/2
    c5 = c5 - c1
    */
    lmpn_add_n(c5, v12, v2, t);

    if (sgn2) lmpn_add_n(c1, v2, vm2, t);
    else lmpn_sub_n(c1, v2, vm2, t);
    lmpn_rshift(c1, c1, t, 1);

    lmpn_sub(c4, v2, t, v0, l<<1);

    lmpn_sub_n(c4, c4, c1, t);
    lmpn_rshift(c4, c4, t, 2);
    lmpn_sublsh(c4, c4, t, vinf, ha+hb, 4);

    if (sgn1) lmpn_add_n(c3, v1, vm1, t);
    else lmpn_sub_n(c3, v1, vm1, t);
    lmpn_rshift(c3, c3, t, 1);

    lmpn_sub_n(c2, v1, c3, t);

    lmpn_submul_1(c5, c2, t, 65);

    lmpn_sub(c2, c2, t, vinf, ha+hb);
    lmpn_sub(c2, c2, t, v0, l<<1);

    lmpn_addmul_1(c5, c2, t, 45);
    lmpn_rshift(c5, c5, t, 1);

    lmpn_sub_n(c4, c4, c2, t);
    lmpn_divexact_by3(c4, c4, t);

    lmpn_sub_n(c2, c2, c4, t);

    lmpn_sub_n(c1, c5, c1, t);
    //if (sgn) lmpn_neg(c1, c1, t);

    lmpn_sublsh_n(c5, c5, c3, t, 3);
    //printf("testing toom44\n");
    lmpn_divexact_1(c5, c5, t, 9);

    lmpn_sub_n(c3, c3, c5, t);

    //printf("toom44 div3\n");
    lmpn_divexact_1(c1, c1, t, 15);
    //printf("toom44 div3 end\n");
    //printf("testing toom44 end\n");
    //if (sgn) lmpn_sub_n(c1, c5, c1, t);
    lmpn_add_n(c1, c1, c5, t);
    lmpn_rshift(c1, c1, t, 1);
    lmpn_sub_n(c5, c5, c1, t);

    memset(c, 0, (n + m) * sizeof(limb_t));
    memcpy(c, v0, (l<<1)*sizeof(limb_t));
    lmpn_add(c+l, c1, t, c+l, l);
    lmpn_add(c+(l<<1), c2, t, c+(l<<1), l+1);
    lmpn_add(c+(l*3), c3, t, c+(l*3), l+1);
    lmpn_add(c+(l<<2), c4, t, c+(l<<2), l+1);
    size_t c5n = n + m - 5*l;
    if (c5n > t) c5n = t;
    assert(c5n >= l+1);
    lmpn_add(c+(l*5), c5, c5n, c+(l*5), l+1);
    lmpn_add_n(c+(l*6), c+(l*6), vinf, ha+hb);

    free(v0); free(v1); free(vm1); free(v2); free(vm2); free(v12); free(vinf);
    free(c1); free(c2); free(c3); free(c4); free(c5);

    return 0;
}

static int toom32(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(m+2 < n && n < 3*m-8);
    size_t l = 2*n >= 3*m ? (n+2) / 3 : (m+1) >> 1, ha = n - 2*l, hb = m - l;
    const limb_t *a1 = a + l, *a2 = a + 2*l;
    const limb_t *b1 = b + l;
    assert(ha <= l && hb <= l && ha + hb > l);

    limb_t *v0 = lmpn_alloc(l<<1);
    unlikely(!v0) return 1;
    limb_t *v1 = lmpn_alloc((l+1)<<1);
    unlikely(!v1) goto cltag(v0);
    limb_t *vm1 = lmpn_alloc((l<<1)+1);
    unlikely(!vm1) goto cltag(v1);
    limb_t *vinf = lmpn_alloc(ha+hb);
    unlikely(!vinf) goto cltag(vm1);

    limb_t *p1 = lmpn_alloc(l+1);
    unlikely(!p1) goto cltag(vinf);
    limb_t *pm1 = lmpn_alloc(l+1);
    unlikely(!pm1) goto cltag(p1);
    limb_t *q1 = lmpn_alloc(l+1);
    unlikely(!q1) goto cltag(pm1);
    limb_t *qm1 = lmpn_alloc(l);

    unlikely(!qm1) {
        tfree(qm1); tfree(q1); tfree(pm1); tfree(p1);
        tfree(vinf); tfree(vm1); tfree(v1); tfree(v0);
        return 1;
    }

    pm1[l] = lmpn_add(pm1, a, l, a2, ha);
    lmpn_add(p1, pm1, l+1, a1, l);
    int sgn = lmpn_sub(pm1, pm1, l+1, a1, l);
    if (sgn) lmpn_neg(pm1, pm1, l+1);

    q1[l] = lmpn_add(q1, b, l, b1, hb);
    if (lmpn_sub(qm1, b, l, b1, hb)) {
        lmpn_neg(qm1, qm1, l);
        sgn ^= 1;
    }

    unlikely(lmpn_mul_n(v0, a, b, l) ||
             lmpn_mul_n(v1, p1, q1, l+1) ||
             lmpn_mul(vm1, pm1, l+1, qm1, l) ||
             (ha >= hb ? lmpn_mul(vinf, a2, ha, b1, hb)
                       : lmpn_mul(vinf, b1, hb, a2, ha))) goto cltag(qm1);

    free(p1); free(pm1); free(q1); free(qm1);

    assert(v1[(l<<1)+1] == 0);

    size_t t = (l<<1)+1;
    limb_t *c2 = lmpn_alloc(t);
    unlikely(!c2) goto cltag(vinf);

    if (sgn) {
        lmpn_sub_n(c2, v1, vm1, t);
        lmpn_add_n(v1, v1, vm1, t);
    } else {
        lmpn_add_n(c2, v1, vm1, t);
        lmpn_sub_n(v1, v1, vm1, t);
    }

    lmpn_rshift(c2, c2, t, 1);
    lmpn_sub(c2, c2, t, v0, l<<1);
    lmpn_rshift(v1, v1, t, 1);
    lmpn_sub(v1, v1, t, vinf, ha+hb);

    memset(c, 0, (n + m) * sizeof(limb_t));
    memcpy(c, v0, (l<<1)*sizeof(limb_t));
    lmpn_add(c+l, v1, t, c+l, l);
    lmpn_add(c+(l<<1), c2, t, c+(l<<1), l+1);
    lmpn_add(c+l*3, vinf, ha+hb, c+l*3, l+1);

    free(v0); free(v1); free(vm1); free(vinf); free(c2);
    return 0;
}

static int toom42(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    assert(n*2 > 3*m + 5 && n < 4*m-15);

    size_t l = n >= 2*m ? (n + 3) >> 2 : (m + 1) >> 1;
    size_t ha = n - 3 * l, hb = m - l;
    assert(ha <= l && hb <= l && ha > 0 && hb > 0 && ha + hb > l);

    const limb_t *a1 = a + l, *a2 = a + 2 * l, *a3 = a + 3 * l;
    const limb_t *b1 = b + l;

    limb_t *tmp = lmpn_alloc(l + 1);
    unlikely(!tmp) return 1;
    limb_t *p1 = lmpn_alloc(l + 1);
    unlikely(!p1) goto cltag(tmp);
    limb_t *pm1 = lmpn_alloc(l + 1);
    unlikely(!pm1) goto cltag(p1);
    limb_t *p2 = lmpn_alloc(l + 1);
    unlikely(!p2) goto cltag(pm1);
    limb_t *q1 = lmpn_alloc(l + 1);
    unlikely(!q1) goto cltag(p2);
    limb_t *qm1 = lmpn_alloc(l);
    unlikely(!qm1) goto cltag(q1);
    limb_t *q2 = lmpn_alloc(l + 1);
    unlikely(!q2) goto cltag(qm1);

    tmp[l] = lmpn_add(tmp, a1, l, a3, ha);
    pm1[l] = lmpn_add_n(pm1, a, a2, l);
    lmpn_add(p1, pm1, l + 1, tmp, l + 1);
    int sgn = lmpn_sub(pm1, pm1, l + 1, tmp, l + 1);
    if (sgn) lmpn_neg(pm1, pm1, l + 1);

    p2[l] = lmpn_addlsh1(p2, a, l, a1, l);
    lmpn_addlsh2(p2, p2, l + 1, a2, l);
    lmpn_addlsh(p2, p2, l + 1, a3, ha, 3);

    q1[l] = lmpn_add(q1, b, l, b1, hb);
    if (lmpn_sub(qm1, b, l, b1, hb)) {
        lmpn_neg(qm1, qm1, l);
        sgn ^= 1;
    }

    q2[l] = lmpn_addlsh1(q2, b, l, b1, hb);

    free(tmp); tmp = NULL;

    limb_t *v0 = lmpn_alloc(l << 1);
    unlikely(!v0) goto cltag(q2);
    limb_t *v1 = lmpn_alloc((l << 1) + 2);
    unlikely(!v1) goto cltag(v0);
    limb_t *vm1 = lmpn_alloc((l << 1) + 1);
    unlikely(!vm1) goto cltag(v1);
    limb_t *v2 = lmpn_alloc((l << 1) + 2);
    unlikely(!v2) goto cltag(vm1);
    limb_t *vinf = lmpn_alloc(ha + hb);
    unlikely(!vinf) goto cltag(v2);

    unlikely(lmpn_mul_n(v0, a, b, l) ||
             lmpn_mul_n(v1, p1, q1, l + 1) ||
             lmpn_mul(vm1, pm1, l + 1, qm1, l) ||
             lmpn_mul_n(v2, p2, q2, l + 1) ||
             (ha >= hb ? lmpn_mul(vinf, a3, ha, b1, hb)
                       : lmpn_mul(vinf, b1, hb, a3, ha)))
        goto cltag(vinf);

    free(p1); p1 = NULL;
    free(pm1); pm1 = NULL;
    free(p2); p2 = NULL;
    free(q1); q1 = NULL;
    free(qm1); qm1 = NULL;
    free(q2); q2 = NULL;

    assert(v1[(l << 1) + 1] == 0);
    assert(v2[(l << 1) + 1] == 0);

    size_t t = (l << 1) + 1;

    limb_t *c2 = lmpn_alloc(t);
    unlikely(!c2) goto cltag(vinf);
    limb_t *c3 = lmpn_alloc(t);
    unlikely(!c3) {
        tfree(c3);
        tfree(c2);
        tfree(vinf);
        tfree(v2); tfree(vm1); tfree(v1); tfree(v0);
        tfree(q2); tfree(qm1); tfree(q1);
        tfree(p2); tfree(pm1); tfree(p1);
        tfree(tmp);
        return 1;
    }

    if (sgn) {
        lmpn_sub_n(c2, v1, vm1, t);
        lmpn_add_n(v1, v1, vm1, t);
    } else {
        lmpn_add_n(c2, v1, vm1, t);
        lmpn_sub_n(v1, v1, vm1, t);
    }
    lmpn_rshift(c2, c2, t, 1);
    c2[l << 1] -= lmpn_sub_n(c2, c2, v0, l << 1);
    assert(c2[l<<1] + 1);
    lmpn_sub(c2, c2, t, vinf, ha + hb);
    assert(c2[l << 1] <= 2);

    int  u = lmpn_sublsh(c3, v2, t, vinf, ha + hb, 4);
    assert(!u);
    u  =  lmpn_sublsh2_n(c3, c3, c2, t);
    assert(!u);
    u=lmpn_sub(c3, c3, t, v0, l << 1);
    assert(!u);
    u=lmpn_sub_n(c3, c3, v1, t);
    assert(!u);
    lmpn_divexact_1(c3, c3, t, 6);
    assert(!u);

    lmpn_rshift(v1, v1, t, 1);
    lmpn_sub_n(v1, v1, c3, t);

    free(vm1);
    free(v2);

    memset(c, 0, (n + m) * sizeof(limb_t));
    memcpy(c, v0, (l << 1) * sizeof(limb_t));
    lmpn_add(c + l,        v1,   t,       c + l,        l);
    lmpn_add(c + (l << 1), c2,   t,       c + (l << 1), l + 1);
    lmpn_add(c + l * 3,    c3,   t,       c + l * 3,    l + 1);
    lmpn_add(c + (l << 2), vinf, ha + hb, c + (l << 2), l + 1);

    free(v0); free(v1); free(vinf);
    free(c2); free(c3);
    return 0;
}

static int toom43(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);
    assert(n < 2 * m);
    assert(n * 3 > 4 * m);

    size_t l = n >= m + m/3 ? (n+3) >> 2 : (m+2)/3;
    size_t ha = n - 3 * l;
    size_t hb = m - 2 * l;
    assert(ha <= l && hb <= l && ha > 0 && hb > 0);

    const limb_t *a1 = a + l, *a2 = a + 2*l, *a3 = a + 3*l;
    const limb_t *b1 = b + l, *b2 = b + 2*l;

    limb_t *tmp1 = lmpn_alloc(l + 2);
    unlikely(!tmp1) return 1;
    limb_t *tmp2 = lmpn_alloc(l + 2);
    unlikely(!tmp2) goto cltag(tmp1);
    limb_t *p1 = lmpn_alloc(l+1);
    unlikely(!p1) goto cltag(tmp2);
    limb_t *pm1 = lmpn_alloc(l+1);
    unlikely(!pm1) goto cltag(p1);
    limb_t *p2 = lmpn_alloc(l+1);
    unlikely(!p2) goto cltag(pm1);
    limb_t *pm2 = lmpn_alloc(l+1);
    unlikely(!pm2) goto cltag(p2);
    limb_t *q1 = lmpn_alloc(l+1);
    unlikely(!q1) goto cltag(pm2);
    limb_t *qm1 = lmpn_alloc(l+1);
    unlikely(!qm1) goto cltag(q1);
    limb_t *q2 = lmpn_alloc(l+1);
    unlikely(!q2) goto cltag(qm1);
    limb_t *qm2 = lmpn_alloc(l+1);
    unlikely(!qm2) goto cltag(q2);

    // p1 = a0+a1+a2+a3, pm1 = |a0-a1+a2-a3|
    tmp1[l] = lmpn_add(tmp1, a1, l, a3, ha);
    pm1[l] = lmpn_add_n(pm1, a, a2, l);
    lmpn_add(p1, pm1, l+1, tmp1, l+1);
    int sgn1 = lmpn_sub(pm1, pm1, l+1, tmp1, l+1);
    if (sgn1) lmpn_neg(pm1, pm1, l+1);

    // p2 = a0+2a1+4a2+8a3, pm2 = |a0-2a1+4a2-8a3|
    tmp1[l] = lmpn_addlsh2(tmp1, a1, l, a3, ha);
    lmpn_lshift(tmp1, tmp1, l+1, 1);
    pm2[l] = lmpn_addlsh2(pm2, a, l, a2, l);
    lmpn_add(p2, pm2, l+1, tmp1, l+1);
    int sgn2 = lmpn_sub(pm2, pm2, l+1, tmp1, l+1);
    if (sgn2) lmpn_neg(pm2, pm2, l+1);

    // q1 = b0+b1+b2, qm1 = |b0-b1+b2|
    tmp1[l] = lmpn_add(tmp1, b, l, b2, hb);
    lmpn_add(q1, tmp1, l+1, b1, l);
    if (lmpn_sub(qm1, tmp1, l+1, b1, l)) {
        lmpn_neg(qm1, qm1, l+1);
        sgn1 ^= 1;
    }

    // q2 = b0+2b1+4b2, qm2 = |b0-2b1+4b2|
    tmp1[l] = lmpn_addlsh2(tmp1, b, l, b2, hb);
    tmp2[l] = lmpn_lshift(tmp2, b1, l, 1);
    lmpn_add(q2, tmp1, l+1, tmp2, l+1);
    if (lmpn_sub(qm2, tmp1, l+1, tmp2, l+1)) {
        lmpn_neg(qm2, qm2, l+1);
        sgn2 ^= 1;
    }

    free(tmp1); tmp1 = NULL;
    free(tmp2); tmp2 = NULL;

    limb_t *v0 = lmpn_alloc(l << 1);
    unlikely(!v0) goto cltag(qm2);
    limb_t *v1 = lmpn_alloc((l+1) << 1);
    unlikely(!v1) goto cltag(v0);
    limb_t *vm1 = lmpn_alloc((l+1) << 1);
    unlikely(!vm1) goto cltag(v1);
    limb_t *v2 = lmpn_alloc((l+1) << 1);
    unlikely(!v2) goto cltag(vm1);
    limb_t *vm2 = lmpn_alloc((l+1) << 1);
    unlikely(!vm2) goto cltag(v2);
    limb_t *vinf = lmpn_alloc(ha + hb);
    unlikely(!vinf) goto cltag(vm2);

    unlikely(lmpn_mul_n(v0, a, b, l) ||
             lmpn_mul_n(v1, p1, q1, l+1) ||
             lmpn_mul_n(vm1, pm1, qm1, l+1) ||
             lmpn_mul_n(v2, p2, q2, l+1) ||
             lmpn_mul_n(vm2, pm2, qm2, l+1) ||
             (ha >= hb ? lmpn_mul(vinf, a3, ha, b2, hb)
                       : lmpn_mul(vinf, b2, hb, a3, ha)))
        goto cltag(vinf);

    free(p1); p1 = NULL;
    free(pm1); pm1 = NULL;
    free(p2); p2 = NULL;
    free(pm2); pm2 = NULL;
    free(q1); q1 = NULL;
    free(qm1); qm1 = NULL;
    free(q2); q2 = NULL;
    free(qm2); qm2 = NULL;

    size_t t2 = (l << 1) + 4;

    limb_t *A = lmpn_alloc(t2);
    unlikely(!A) goto cltag(vinf);
    limb_t *B = lmpn_alloc(t2);
    unlikely(!B) goto cltag(A);
    limb_t *C = lmpn_alloc(t2);
    unlikely(!C) goto cltag(B);
    limb_t *D = lmpn_alloc(t2);
    unlikely(!D) goto cltag(C);
    limb_t *c1 = lmpn_alloc(t2);
    unlikely(!c1) goto cltag(D);
    limb_t *c2 = lmpn_alloc(t2);
    unlikely(!c2) goto cltag(c1);
    limb_t *c3 = lmpn_alloc(t2);
    unlikely(!c3) goto cltag(c2);
    limb_t *c4 = lmpn_alloc(t2);
    unlikely(!c4) goto cltag(c3);
    limb_t *tmp = lmpn_alloc(t2 + 1);

    unlikely(!tmp) {
        tfree(tmp);
        tfree(c4); tfree(c3); tfree(c2); tfree(c1);
        tfree(D); tfree(C); tfree(B); tfree(A);
        tfree(vinf);
        tfree(vm2); tfree(v2); tfree(vm1); tfree(v1); tfree(v0);
        tfree(qm2); tfree(q2); tfree(qm1); tfree(q1);
        tfree(pm2); tfree(p2); tfree(pm1); tfree(p1);
        tfree(tmp2); tfree(tmp1);
        return 1;
    }

    // A = (v1 ± vm1)/2 - v0 ; B = (v1 ∓ vm1)/2 - vinf
    if (sgn1) {
        lmpn_sub_n(A, v1, vm1, (l<<1)+2);
        lmpn_add_n(B, v1, vm1, (l<<1)+2);
    } else {
        lmpn_add_n(A, v1, vm1, (l<<1)+2);
        lmpn_sub_n(B, v1, vm1, (l<<1)+2);
    }
    for (size_t i = (l<<1)+2; i < t2; ++i) { A[i] = 0; B[i] = 0; }
    lmpn_rshift(A, A, t2, 1);
    lmpn_rshift(B, B, t2, 1);
    memcpy(tmp, v0, (l<<1) * sizeof(limb_t));
    memset(tmp + (l<<1), 0, (t2 - (l<<1)) * sizeof(limb_t));
    lmpn_sub_n(A, A, tmp, t2);
    memcpy(tmp, vinf, (ha+hb) * sizeof(limb_t));
    memset(tmp + (ha+hb), 0, (t2 - (ha+hb)) * sizeof(limb_t));
    lmpn_sub_n(B, B, tmp, t2);

    // C = (v2 ± vm2)/2 - v0 ; D = (v2 ∓ vm2)/4 - 16*vinf
    if (sgn2) {
        lmpn_sub_n(C, v2, vm2, (l<<1)+2);
        lmpn_add_n(D, v2, vm2, (l<<1)+2);
    } else {
        lmpn_add_n(C, v2, vm2, (l<<1)+2);
        lmpn_sub_n(D, v2, vm2, (l<<1)+2);
    }
    for (size_t i = (l<<1)+2; i < t2; ++i) { C[i] = 0; D[i] = 0; }
    lmpn_rshift(C, C, t2, 1);
    lmpn_rshift(D, D, t2, 2);
    memcpy(tmp, v0, (l<<1) * sizeof(limb_t));
    memset(tmp + (l<<1), 0, (t2 - (l<<1)) * sizeof(limb_t));
    lmpn_sub_n(C, C, tmp, t2);
    memset(tmp, 0, (t2+1) * sizeof(limb_t));
    limb_t ov = lmpn_lshift(tmp, vinf, ha+hb, 4);
    if (ov) tmp[ha+hb] = ov;
    lmpn_sub_n(D, D, tmp, t2);

    // c4 = (C - 4A)/12
    memcpy(c4, C, t2 * sizeof(limb_t));
    memset(tmp, 0, (t2+1) * sizeof(limb_t));
    (void)lmpn_lshift(tmp, A, t2, 2);
    lmpn_sub_n(c4, c4, tmp, t2);
    {
        size_t nc = lmpn_norm(c4, t2);
        if (nc > 0) lmpn_divexact_1(c4, c4, nc, 12);
        for (size_t i = nc; i < t2; ++i) c4[i] = 0;
    }

    // c2 = A - c4
    memcpy(c2, A, t2 * sizeof(limb_t));
    lmpn_sub_n(c2, c2, c4, t2);

    // c3 = (D - B)/3
    memcpy(c3, D, t2 * sizeof(limb_t));
    lmpn_sub_n(c3, c3, B, t2);
    {
        size_t nc = lmpn_norm(c3, t2);
        if (nc > 0) lmpn_divexact_1(c3, c3, nc, 3);
        for (size_t i = nc; i < t2; ++i) c3[i] = 0;
    }

    // c1 = B - c3
    memcpy(c1, B, t2 * sizeof(limb_t));
    lmpn_sub_n(c1, c1, c3, t2);

    memset(c, 0, (n + m) * sizeof(limb_t));
    memcpy(c, v0, (l << 1) * sizeof(limb_t));
    lmpn_add(c + l,   c1, t2, c + l,   l);
    lmpn_add(c + 2*l, c2, t2, c + 2*l, l + 1);
    lmpn_add(c + 3*l, c3, t2, c + 3*l, l + 1);
    lmpn_add(c + 4*l, c4, t2, c + 4*l, l + 1);
    lmpn_add(c + 5*l, vinf, ha + hb, c + 5*l, ha + hb);

    free(A); free(B); free(C); free(D);
    free(c1); free(c2); free(c3); free(c4);
    free(tmp);
    free(v0); free(v1); free(vm1);
    free(v2); free(vm2); free(vinf);
    return 0;
}

static int ntt(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    const size_t l = n+m;
    size_t L = 1, k = 0;
    while (L < l) {L <<= 1; ++k;}
    assert(L <= LIMB_MAX);
    limb_t *A[3], *B[3];
    unlikely(!(A[0] = lmpn_alloc(L))) return 1;
    unlikely(!(A[1] = lmpn_alloc(L))) goto clean_A0;
    unlikely(!(A[2] = lmpn_alloc(L))) goto clean_A1;
    unlikely(!(B[0] = lmpn_alloc(L))) goto clean_A2;
    unlikely(!(B[1] = lmpn_alloc(L))) goto clean_B0;
    unlikely(!(B[2] = lmpn_alloc(L))) {
        clean_B2: free(B[2]);
        clean_B1: free(B[1]);
        clean_B0: free(B[0]);
        clean_A2: free(A[2]);
        clean_A1: free(A[1]);
        clean_A0: free(A[0]);
        return 1;
    }
    const limb_t p1 = lmpn_ntt_primes[k-NTT_START][0].p, p2 = lmpn_ntt_primes[k-NTT_START][1].p, p3 = lmpn_ntt_primes[k-NTT_START][2].p;
    const limb_t *wk1 = lmpn_ntt_primes[k-NTT_START][0].wk, *wk2 = lmpn_ntt_primes[k-NTT_START][1].wk, *wk3 = lmpn_ntt_primes[k-NTT_START][2].wk;
    const limb_t *wki1 = lmpn_ntt_primes[k-NTT_START][0].wki, *wki2 = lmpn_ntt_primes[k-NTT_START][1].wki, *wki3 = lmpn_ntt_primes[k-NTT_START][2].wki;
    assert(p1 < p2 && p2 < p3);

    for (size_t i=0;i<n;++i) {
        A[0][i] = a[i] % p1;
        A[1][i] = a[i] % p2;
        A[2][i] = a[i] % p3;
    }
    memset(A[0]+n, 0, (L-n)*sizeof(limb_t));
    memset(A[1]+n, 0, (L-n)*sizeof(limb_t));
    memset(A[2]+n, 0, (L-n)*sizeof(limb_t));

    for (size_t i=0;i<m;++i) {
        B[0][i] = b[i] % p1;
        B[1][i] = b[i] % p2;
        B[2][i] = b[i] % p3;
    }
    memset(B[0]+m, 0, (L-m)*sizeof(limb_t));
    memset(B[1]+m, 0, (L-m)*sizeof(limb_t));
    memset(B[2]+m, 0, (L-m)*sizeof(limb_t));

    lmpn_ntt_dif(A[0], L, p1, wk1);
    lmpn_ntt_dif(A[1], L, p2, wk2);
    lmpn_ntt_dif(A[2], L, p3, wk3);

    lmpn_ntt_dif(B[0], L, p1, wk1);
    lmpn_ntt_dif(B[1], L, p2, wk2);
    lmpn_ntt_dif(B[2], L, p3, wk3);

    for (size_t i=0;i<L;++i) {
        A[0][i] = limb_mulm(A[0][i], B[0][i], p1);
        A[1][i] = limb_mulm(A[1][i], B[1][i], p2);
        A[2][i] = limb_mulm(A[2][i], B[2][i], p3);
    }

    free(B[0]); free(B[1]); free(B[2]);

    lmpn_ntt_dit(A[0], L, p1, wki1);
    lmpn_ntt_dit(A[1], L, p2, wki2);
    lmpn_ntt_dit(A[2], L, p3, wki3);

    // Garner: c = x1 + x2*p1 + x3*p1*p2
    // c = a1 (mod p1), c = a2 (mod p2), c = a3 (mod p3)
    // x1 = a1, x2 = (a2 - x1) * p1^{-1} (mod p2)
    // x3 = ((a3-x1)*p1^{-1} - x2)*p2^{-1} (mod p3)
    // c: limb_t[3], a1: limb_t, a2: limb_t, a3: limb_t
    const limb_t i12 = limb_invm_p(p1, p2), i13 = limb_invm_p(p1, p3), i23 = limb_invm_p(p2, p3);
    const limb_t i12s = limb_div(i12, 0, p2), i13s = limb_div(i13, 0, p3), i23s = limb_div(i23, 0, p3);
    limb_t p12[2];
    limb_mul(p12+1, p12, p1, p2);

    #define eval_c(c, a1, a2, a3) do { \
        limb_t a2ma1 = (a2 < a1) ? (a2 - a1 + p2) : (a2 - a1); \
        limb_t a3ma1 = (a3 < a1) ? (a3 - a1 + p3) : (a3 - a1); \
        limb_t x2 = a2ma1 * i12 - limb_mul_hi(a2ma1, i12s) * p2; \
        if (x2 >= p2) x2 -= p2; \
        limb_t help = a3ma1 * i13 - limb_mul_hi(a3ma1, i13s) * p3; \
        if (help >= x2) help -= x2; \
        else help += p3 - x2; \
        limb_t x3 = help * i23 - limb_mul_hi(help, i23s) * p3; \
        if (x3 >= p3) x3 -= p3; \
        limb_mul(c+1, c, x2, p1); \
        c[2] = lmpn_addmul_1(c, p12, 2, x3); \
        lmpn_add_1(c, c, 3, a1); \
    } while(0)

    limb_t tmp[3] = {0, 0, 0};
    // i=0
    eval_c(c, A[0][0], A[1][0], A[2][0]);
    // i+2 < l
    for (size_t i=1;i+2<l;++i) {
        eval_c(tmp, A[0][i], A[1][i], A[2][i]);
        c[i+2] = tmp[2] + lmpn_add_n(c+i, c+i, tmp, 2); // 如果 c[i+2] 不会溢出就没事
    }
    
    // i=l-2
    eval_c(tmp, A[0][l-2], A[1][l-2], A[2][l-2]);
    assert(tmp[2] == 0);
    lmpn_add_n(c+l-2, c+l-2, tmp, 2);

    // i=l-1
    eval_c(tmp, A[0][l-1], A[1][l-1], A[2][l-1]);
    assert(tmp[1] == 0 && tmp[2] == 0);
    c[l-1] += tmp[0];

    free(A[0]); free(A[1]); free(A[2]);
    return 0;
}

// API

int lmpn_mul_n(limb_t *c, const limb_t *a, const limb_t *b, size_t n) {
    //basecase(c, a, n, b, n);
    //return;
    if (n < MUL_TOOM22_THRESHOLD) {basecase(c, a, n, b, n); return 0;}
    else if (n < MUL_TOOM33_THRESHOLD) return toom22(c, a, n, b, n);
    else if (n < MUL_TOOM44_THRESHOLD) return toom33(c, a, n, b, n);
    else if (n < MUL_NTT_THRESHOLD) return toom44(c, a, n, b, n);
    else return ntt(c, a, n, b, n);
}

// ---------- 辅助：分块处理极端不平衡 ----------
static int lmpn_mul_unbalanced(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    // 前置条件：n >= 4m
    // 策略：切 2m 大小的块，与 b 做 toom42，累加到 c
    size_t chunk = 2 * m;

    limb_t *tmp = lmpn_alloc(n - chunk + m);
    if (!tmp) return 1;

    int r = lmpn_mul(tmp, a + chunk, n - chunk, b, m);
    unlikely(r) goto cleanup;
    memset(c, 0, (n + m) * sizeof(limb_t));
    r = toom42(c, a, chunk, b, m);
    unlikely(r) goto cleanup;
    lmpn_add(c + chunk, c + chunk, n - chunk + m, tmp, n - chunk + m);

    cleanup:
    free(tmp);
    return r;
}

// ---------- 辅助：m 很小、n 很大的分块 ----------
static int lmpn_mul_small_b(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    // 前置条件：m < MUL_TOOM22_THRESHOLD 且 n > MUL_BASECASE_MAX_UN
    limb_t *tp = lmpn_alloc(m);
    unlikely(!tp) return 1;

    basecase(c, a, MUL_BASECASE_MAX_UN, b, m);
    limb_t *prodp = c + MUL_BASECASE_MAX_UN;
    memcpy(tp, prodp, m * sizeof(limb_t));
    const limb_t *up = a + MUL_BASECASE_MAX_UN;
    size_t un = n - MUL_BASECASE_MAX_UN;

    while (un > MUL_BASECASE_MAX_UN) {
        basecase(prodp, up, MUL_BASECASE_MAX_UN, b, m);
        limb_t cy = lmpn_add_n(prodp, prodp, tp, m);
        lmpn_add_1(prodp + m, prodp + m, MUL_BASECASE_MAX_UN, cy);
        prodp += MUL_BASECASE_MAX_UN;
        memcpy(tp, prodp, m * sizeof(limb_t));
        up += MUL_BASECASE_MAX_UN;
        un -= MUL_BASECASE_MAX_UN;
    }

    if (un > m) basecase(prodp, up, un, b, m);
    else basecase(prodp, b, m, up, un);

    limb_t cy = lmpn_add_n(prodp, prodp, tp, m);
    lmpn_add_1(prodp + m, prodp + m, un, cy);

    free(tp);
    return 0;
}

int lmpn_mul(limb_t *c, const limb_t *a, size_t n, const limb_t *b, size_t m) {
    assert(n >= m);

    //basecase(c, a, n, b, m);
    //return;

    ////printf("mul %llu %llu\n", n, m);

    // 1. 极小规模
    if (n < MUL_TOOM22_THRESHOLD) {
        basecase(c, a, n, b, m);
        return 0;
    }

    // 2. 等长
    if (n == m) return lmpn_mul_n(c, a, b, n);

    // 3. m 很小，n 很大
    if (m < MUL_TOOM22_THRESHOLD) {
        if (n <= MUL_BASECASE_MAX_UN) basecase(c, a, n, b, m);
        else return lmpn_mul_small_b(c, a, n, b, m);
        return 0;
    }

    // 4. 极端不平衡：n >= 4m，任何 Toom 都无法覆盖
    if (n >= 4 * m) return lmpn_mul_unbalanced(c, a, n, b, m);

    // 5. 按 m 分层，层内按 n/m 比例选算法
    if (m < MUL_TOOM33_THRESHOLD) {
        // ToomX2 变体：toom22 / toom32 / toom42
        if (4 * n < 5 * m) return toom22(c, a, n, b, m);
        else if (4 * n < 7 * m) return toom32(c, a, n, b, m);
        else if (n*2 > 3*m+5 && n < 4*m-15) return toom42(c, a, n, b, m);
        else return lmpn_mul_unbalanced(c, a, n, b, m);
    } else if (m < MUL_TOOM44_THRESHOLD) {
        // ToomX3 变体：toom33 / toom43 / toom32 / toom42
        if (6*n < 7*m) return toom33(c, a, n, b, m);
        else if (2 * n < 3 * m) {
            if (n*3 > 4*m+6 && n < 2*m) return toom43(c, a, n, b, m);
            else return toom32(c, a, n, b, m);
        }
        else if (4*n < 7*m) return toom32(c, a, n, b, m);
        else if (n*2 > 3*m+5 && n < 4*m-15) return toom42(c, a, n, b, m);
        else return lmpn_mul_unbalanced(c, a, n, b, m);
    } else if (m < MUL_NTT_THRESHOLD) {
        // 大 m：toom44 / toom43 / toom32 / toom42
        if (3*n <= 4*m - 12) return toom44(c, a, n, b, m);
        else if (2*n < 3*m) {
            if (n*3 > 4*m+6 && n < 2*m) return toom43(c, a, n, b, m);
            else return toom32(c, a, n, b, m);
        } else if (4*n < 7*m) return toom32(c, a, n, b, m);
        else if (n*2 > 3*m+5 && n < 4*m-15) return toom42(c, a, n, b, m);
        else return lmpn_mul_unbalanced(c, a, n, b, m);
    } else {
        // m >= MUL_NTT_THRESHOLD，n >= m
        // 此时 n 和 m 都很大，NTT 是合适的

        if (n >= 8 * m) {
            // 前置条件：n >= 8*m，m >= MUL_NTT_THRESHOLD
            assert(n >= 8 * m);

            const size_t chunk = 3 * m;
            // 临时缓冲：需容纳 max(chunk + m, n_final + m)
            // 循环退出后 n_final < 3.5*m，所以 n_final + m < 4.5*m
            limb_t *tmp = lmpn_alloc(9 * m / 2 + 2);
            unlikely(!tmp) return 1;

            // ---- 第一块：直接写入 c ----
            int r = ntt(c, a, chunk, b, m);
            unlikely(r) goto cleanup;
            a += chunk;
            n -= chunk;
            c += chunk;

            // ---- 中间块循环：剩余 n >= 3.5*m ----
            while (n >= 7 * m / 2) {
                r = ntt(tmp, a, chunk, b, m);
                unlikely(r) goto cleanup;
                a += chunk;
                n -= chunk;

                // c[0..m-1] += tmp[0..m-1]（与前一块重叠的 vn 个 limb）
                limb_t cy = lmpn_add_n(c, c, tmp, m);
                // c[m..chunk+m-1] = tmp[m..chunk+m-1]（没有重叠，直接覆盖）
                memcpy(c + m, tmp + m, chunk * sizeof(limb_t));
                // 进位传播
                lmpn_add_1(c + m, c + m, chunk, cy);

                c += chunk;
            }

            // ---- 剩余部分：n < 3.5*m ----
            if (n > 0) {
                if (n < m) r = lmpn_mul(tmp, b, m, a, n);   // 保证 n >= m 的契约
                else       r = lmpn_mul(tmp, a, n, b, m);
                unlikely(r) goto cleanup;

                limb_t cy = lmpn_add_n(c, c, tmp, m);
                memcpy(c + m, tmp + m, n * sizeof(limb_t));
                lmpn_add_1(c + m, c + m, n, cy);
            }

        cleanup:
            free(tmp);
            return r;
        } else return ntt(c, a, n, b, m);
    }
}