/* linear algebra + polynomial roots */
#include "core.h"
#include <string.h>

double det2(const double m[2][2]) { return m[0][0] * m[1][1] - m[0][1] * m[1][0]; }
double det3(const double m[3][3]) {
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1]) - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
           m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}
int inv3(const double m[3][3], double o[3][3]) {
    double d = det3(m);
    if (fabs(d) < 1e-12) return 0;
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int r0 = (j + 1) % 3, r1 = (j + 2) % 3, c0 = (i + 1) % 3, c1 = (i + 2) % 3;
            o[i][j] = (m[r0][c0] * m[r1][c1] - m[r0][c1] * m[r1][c0]) / d;
        }
    return 1;
}
void matmul(const double *a, const double *b, double *o, int n, int k, int m) {
    double t[64];
    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++) {
            double s = 0;
            for (int l = 0; l < k; l++) s += a[i * k + l] * b[l * m + j];
            t[i * m + j] = s;
        }
    memcpy(o, t, sizeof(double) * n * m);
}
int rank_n(const double *a, int r, int c) {
    double m[64];
    memcpy(m, a, sizeof(double) * r * c);
    int rank = 0;
    for (int col = 0; col < c && rank < r; col++) {
        int piv = -1;
        double best = 1e-9;
        for (int i = rank; i < r; i++) if (fabs(m[i * c + col]) > best) { best = fabs(m[i * c + col]); piv = i; }
        if (piv < 0) continue;
        for (int j = 0; j < c; j++) { double t = m[rank * c + j]; m[rank * c + j] = m[piv * c + j]; m[piv * c + j] = t; }
        for (int i = 0; i < r; i++) {
            if (i == rank) continue;
            double f = m[i * c + col] / m[rank * c + col];
            for (int j = 0; j < c; j++) m[i * c + j] -= f * m[rank * c + j];
        }
        rank++;
    }
    return rank;
}
void cross3(const double *a, const double *b, double *o) {
    double t[3] = {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
    memcpy(o, t, sizeof t);
}
double dot3(const double *a, const double *b) { return a[0] * b[0] + a[1] * b[1] + a[2] * b[2]; }
double norm3(const double *a) { return sqrt(dot3(a, a)); }

/* Aberth / Durand-Kerner on monic, then Newton polish */
static cx peval(const cx *a, int d, cx z) {
    cx r = a[d];
    for (int k = d - 1; k >= 0; k--) r = cadd(cmul(r, z), a[k]);
    return r;
}
int proots(const cx *a0, int deg, cx *roots) {
    cx a[12];
    while (deg > 0 && cabs_(a0[deg]) < 1e-14) deg--;
    if (deg <= 0) return 0;
    int nz = 0;
    /* strip zero roots */
    while (nz < deg && cabs_(a0[nz]) < 1e-14) roots[nz++] = R(0);
    int d = deg - nz;
    for (int k = 0; k <= d; k++) a[k] = cdiv(a0[k + nz], a0[deg]);
    if (d == 1) { roots[nz] = cneg(a[0]); return deg; }
    if (d == 2) {
        cx disc = csub(cmul(a[1], a[1]), cmul(R(4), a[0]));
        cx s = csqrt_(disc);
        roots[nz] = cmul(R(0.5), cadd(cneg(a[1]), s));
        roots[nz + 1] = cmul(R(0.5), csub(cneg(a[1]), s));
        clean_roots(roots + nz, 2);
        return deg;
    }
    double rad = 0;
    for (int k = 0; k < d; k++) if (cabs_(a[k]) > rad) rad = cabs_(a[k]);
    rad = 1 + rad;
    cx z[12];
    for (int k = 0; k < d; k++) z[k] = cpolar(rad * 0.7, 2 * M_PI * k / d + 0.4);
    for (int it = 0; it < 500; it++) {
        double maxd = 0;
        for (int k = 0; k < d; k++) {
            cx num = peval(a, d, z[k]);
            cx den = R(1);
            for (int j = 0; j < d; j++) if (j != k) den = cmul(den, csub(z[k], z[j]));
            cx dz = cdiv(num, den);
            if (dz.re != dz.re) dz = R(1e-3);
            z[k] = csub(z[k], dz);
            if (cabs_(dz) > maxd) maxd = cabs_(dz);
        }
        if (maxd < 1e-15) break;
    }
    /* Newton polish on original */
    for (int k = 0; k < d; k++) {
        for (int it = 0; it < 6; it++) {
            cx p = a[d], dp = R(0);
            for (int j = d - 1; j >= 0; j--) { dp = cadd(cmul(dp, z[k]), p); p = cadd(cmul(p, z[k]), a[j]); }
            if (cabs_(dp) < 1e-14) break;
            cx nz2 = csub(z[k], cdiv(p, dp));
            if (nz2.re != nz2.re) break;
            z[k] = nz2;
        }
        roots[nz + k] = z[k];
    }
    clean_roots(roots + nz, d);
    return deg;
}

int proots_r(const double *a, int deg, cx *roots) {
    cx c[12];
    for (int k = 0; k <= deg; k++) c[k] = R(a[k]);
    int n = proots(c, deg, roots);
    /* enforce conjugate symmetry & sort: real first ascending, then by imag */
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            cx A = roots[i], B = roots[j];
            int sw = 0;
            if ((A.im != 0) && (B.im == 0)) sw = 1;
            else if ((A.im == 0) == (B.im == 0)) {
                if (A.re > B.re + 1e-9) sw = 1;
                else if (fabs(A.re - B.re) <= 1e-9 && A.im < B.im) sw = 1;
            }
            if (sw) { roots[i] = B; roots[j] = A; }
        }
    return n;
}

void clean_roots(cx *r, int n) {
    for (int i = 0; i < n; i++) {
        double s = 1 + cabs_(r[i]);
        if (fabs(r[i].im) < 1e-9 * s) r[i].im = 0;
        if (fabs(r[i].re) < 1e-9 * s) r[i].re = 0;
        /* snap near-nice values */
        double rr = floor(r[i].re * 1e6 + 0.5) / 1e6;
        if (fabs(rr - r[i].re) < 1e-9 * s) r[i].re = rr;
        double ii = floor(r[i].im * 1e6 + 0.5) / 1e6;
        if (fabs(ii - r[i].im) < 1e-9 * s) r[i].im = ii;
    }
}

int lsolve(double *A, double *b, int n) {
    for (int c = 0; c < n; c++) {
        int piv = c;
        for (int r = c + 1; r < n; r++) if (fabs(A[r * n + c]) > fabs(A[piv * n + c])) piv = r;
        if (fabs(A[piv * n + c]) < 1e-12) return 0;
        if (piv != c) {
            for (int j = 0; j < n; j++) { double t = A[c * n + j]; A[c * n + j] = A[piv * n + j]; A[piv * n + j] = t; }
            double t = b[c]; b[c] = b[piv]; b[piv] = t;
        }
        for (int r = 0; r < n; r++) {
            if (r == c) continue;
            double f = A[r * n + c] / A[c * n + c];
            for (int j = c; j < n; j++) A[r * n + j] -= f * A[c * n + j];
            b[r] -= f * b[c];
        }
    }
    for (int i = 0; i < n; i++) b[i] /= A[i * n + i];
    return 1;
}
