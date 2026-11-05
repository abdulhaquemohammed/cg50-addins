/* Further Maths Unit 4: Further Pure B */
#include "../common/core.h"
#include "../common/util.h"
#include <string.h>
#include <stdlib.h>

static double binom(int n, int k) {
    double r = 1;
    for (int i = 1; i <= k; i++) r = r * (n - k + i) / i;
    return r;
}

/* "e^(kx)" */
static const char *fexp(double k, const char *v) {
    char *o = sbuf();
    if (fabs(k) < 1e-12) { o[0] = 0; return o; }
    strcpy(o, "e^");
    if (fabs(k - 1) < 1e-12) { strcat(o, v); return o; }
    strcat(o, "(");
    if (fabs(k + 1) < 1e-12) strcat(o, "-");
    else strcat(o, fxp(k));
    strcat(o, v);
    strcat(o, ")");
    return o;
}
/* "cos 3x" */
static const char *fexpi(double t) {
    char *o = sbuf();
    strcpy(o, t < 0 ? "e^(-i" : "e^(i");
    strcat(o, fx(fabs(t)));
    strcat(o, ")");
    if (fabs(t) < 1e-12) strcpy(o, "");
    return o;
}
static const char *ftrig(const char *fn, double k, const char *v) {
    char *o = sbuf();
    strcpy(o, fn);
    strcat(o, " ");
    if (fabs(k - 1) > 1e-12) strcat(o, fxp(k));
    strcat(o, v);
    return o;
}

/* ======================= COMPLEX: DE MOIVRE ======================= */
static void d_power(void) {
    cx z = in_cx("z");
    int n = in_int("Power n", -100, 100);
    double r = cabs_(z), t = carg_(z);
    if (r == 0) fail("z = 0");
    outf("r = |z| = %s,  " S_TH " = arg z = %s", fxd(r), fx(t));
    outf("z = %s(cos %s + i sin %s)", fx(r), fx(t), fx(t));
    outf("de Moivre: z^n = r^n(cos n" S_TH " + i sin n" S_TH ")");
    double rn = pow(r, n), tn = n * t;
    outf("r^%d = %s,  n" S_TH " = %s", n, fxd(rn), fx(tn));
    double tp = atan2(sin(tn), cos(tn));
    if (fabs(tp - tn) > 1e-9) outf("Principal argument: %s", fang(tp));
    outf(ANS "z^%d = %s(cos %s + i sin %s)", n, fx(rn), fx(tp), fx(tp));
    outf(ANS "    = %s", fc(cpolar(rn, tp)));
}

static void d_roots(void) {
    cx z = in_cx("w (solve z^n = w)");
    int n = in_int("n", 2, 12);
    double r = cabs_(z), t = carg_(z);
    if (r == 0) { outf(ANS "z = 0 (repeated)"); return; }
    outf("w = %s%s", fx(r), fexpi(t));
    outf("z = %s^(1/%d) e^(i(%s + 2k" S_PI ")/%d)", fx(r), n, fx(t), n);
    double rr = pow(r, 1.0 / n);
    outf("Modulus of each root = %s", fxd(rr));
    for (int k = 0; k < n; k++) {
        double a = (t + 2 * k * M_PI) / n;
        double ap = atan2(sin(a), cos(a));
        outf(ANS "k=%d: %s%s = %s", k, fx(rr), fexpi(ap), fc(cpolar(rr, ap)));
    }
    outf("Roots form a regular %d-gon centred at O", n);
    outf("Sum of roots = 0");
}

/* polynomial helpers in one variable (ascending), small degrees */
static void padd(double *a, const double *b, int n, double k) { for (int i = 0; i <= n; i++) a[i] += k * b[i]; }

static void d_multiple(void) {
    static const char *o[] = {"cos n" S_TH " in powers of cos" S_TH, "sin n" S_TH " in terms of sin" S_TH " and cos" S_TH,
                              "tan n" S_TH " in powers of tan" S_TH};
    int k = in_choice("Express", o, 3);
    int n = in_int("n", 2, 9);
    outf("(cos" S_TH " + i sin" S_TH ")^%d = cos %d" S_TH " + i sin %d" S_TH, n, n, n);
    outf("Expand LHS with binomial, c = cos" S_TH ", s = sin" S_TH ":");
    /* list terms */
    for (int j = 0; j <= n; j++) {
        double co = binom(n, j);
        const char *ip = (j % 4 == 0) ? "" : (j % 4 == 1) ? "i" : (j % 4 == 2) ? "-" : "-i";
        char *t = sbuf();
        strcpy(t, "  ");
        if (ip[0] == '-') strcat(t, "-");
        if (co != 1) strcat(t, fd(co));
        if (strchr(ip, 'i')) strcat(t, "i");
        if (n - j > 0) { strcat(t, "c"); if (n - j > 1) strcat(t, sup(n - j)); }
        if (j > 0) { strcat(t, "s"); if (j > 1) strcat(t, sup(j)); }
        out_line(t);
    }
    if (k == 0 || k == 1) {
        /* real part: sum over even j of (-1)^(j/2) C(n,j) c^(n-j) s^j, s^2 = 1 - c^2 */
        double P[12] = {0};
        int odd = (k == 1);
        for (int j = odd; j <= n; j += 2) {
            double sg = ((j / 2) % 2) ? -1 : 1;
            double term[12] = {0};
            term[n - j] = sg * binom(n, j);
            /* multiply by (1 - c^2)^(j/2) */
            int h = j / 2;
            for (int m = 0; m < h; m++) {
                double t2[12] = {0};
                for (int i = 0; i <= 10; i++) { t2[i] += term[i]; if (i + 2 <= 11) t2[i + 2] -= term[i]; }
                memcpy(term, t2, sizeof term);
            }
            padd(P, term, 11, 1);
        }
        if (k == 0) {
            outf("Real parts, then use s" S_SQ " = 1 - c" S_SQ ":");
            outf(ANS "cos %d" S_TH " = %s", n, fpoly(P, n, "c"));
        } else {
            outf("Imaginary parts, then s" S_SQ " = 1 - c" S_SQ ":");
            outf(ANS "sin %d" S_TH " = s(%s)", n, fpoly(P, n, "c"));
            if (n % 2 == 1) {
                /* odd n: in powers of s using c^2 = 1 - s^2 */
                double Q[12] = {0};
                for (int j = 1; j <= n; j += 2) {
                    double sg = ((j / 2) % 2) ? -1 : 1;
                    double term[12] = {0};
                    term[j] = sg * binom(n, j);
                    int h = (n - j) / 2;
                    for (int m = 0; m < h; m++) {
                        double t2[12] = {0};
                        for (int i = 0; i <= 10; i++) { t2[i] += term[i]; if (i + 2 <= 11) t2[i + 2] -= term[i]; }
                        memcpy(term, t2, sizeof term);
                    }
                    padd(Q, term, 11, 1);
                }
                outf("Odd n, so also with c" S_SQ " = 1 - s" S_SQ ":");
                outf(ANS "sin %d" S_TH " = %s", n, fpoly(Q, n, "s"));
            }
        }
        outf("(c = cos" S_TH ", s = sin" S_TH ")");
    } else {
        /* tan: divide by c^n */
        double N_[12] = {0}, D[12] = {0};
        for (int j = 0; j <= n; j++) {
            double sg = ((j / 2) % 2) ? -1 : 1;
            if (j % 2) N_[j] = sg * binom(n, j); else D[j] = sg * binom(n, j);
        }
        outf("tan n" S_TH " = Im/Re, divide top and bottom by c^%d:", n);
        outf(ANS "tan %d" S_TH " = (%s)", n, fpoly(N_, n, "t"));
        outf(ANS "          / (%s)", fpoly(D, n, "t"));
        outf("(t = tan" S_TH ")");
    }
}

static void d_powers(void) {
    static const char *o[] = {"cos^n " S_TH, "sin^n " S_TH};
    int k = in_choice("Express", o, 2);
    int n = in_int("n", 2, 9);
    const char *zz = k == 0 ? "z + 1/z" : "z - 1/z";
    outf("z = e^(i" S_TH "): %s = %s", zz, k == 0 ? "2cos" S_TH : "2i sin" S_TH);
    outf("z^k %s z^-k = %s", k == 0 ? "+" : "-", k == 0 ? "2cos k" S_TH : "2i sin k" S_TH);
    outf("(%s)^%d = %s", zz, n, k == 0 ? "2^n cos^n" S_TH : "(2i)^n sin^n" S_TH);
    char *ex = sbuf();
    for (int j = 0; j <= n; j++) {
        double c = binom(n, j) * ((k == 1 && j % 2) ? -1 : 1);
        int p = n - 2 * j;
        char t[24];
        t[0] = 0;
        if (p != 0) { strcpy(t, "z"); if (p != 1) { strcat(t, "^"); strcat(t, fd(p)); } }
        strcat(ex, fcoef(c, t, j == 0));
    }
    outf("Expand: %s", ex);
    /* pair terms */
    char *res = sbuf();
    int first = 1;
    double scale = pow(2, n - 1);
    for (int j = 0; j < (n + 1) / 2; j++) {
        int p = n - 2 * j;
        double c = binom(n, j) * ((k == 1 && j % 2) ? -1 : 1);
        char t[24];
        if (k == 0 || n % 2 == 0) { strcpy(t, "cos "); }
        else strcpy(t, "sin ");
        if (p != 1) strcat(t, fd(p));
        strcat(t, S_TH);
        double sgn = 1;
        if (k == 1) sgn = (n % 2 == 0) ? (((n / 2) % 2) ? -1 : 1) : ((((n - 1) / 2) % 2) ? -1 : 1);
        strcat(res, fcoef(c * sgn, t, first));
        first = 0;
    }
    if (n % 2 == 0) {
        double c = binom(n, n / 2) * ((k == 1 && (n / 2) % 2) ? -1 : 1);
        double sgn = (k == 1) ? (((n / 2) % 2) ? -1 : 1) : 1;
        strcat(res, fcoef(c * sgn / 2, "", 0));
    }
    outf("Pair z^k with z^-k and divide by %s:", k == 0 ? "2^n" : "(2i)^n");
    outf(ANS "%s^%d " S_TH " = (1/%s)[%s]", k == 0 ? "cos" : "sin", n, fd(scale), res);
}

static const calc_t demoivre_c[] = {
    {"Powers z^n (de Moivre)", d_power},
    {"nth roots / roots of unity", d_roots},
    {"cos n" S_TH ", sin n" S_TH ", tan n" S_TH, d_multiple},
    {"cos^n " S_TH ", sin^n " S_TH " in multiple angles", d_powers},
};

/* ======================= FURTHER TRIG ======================= */
static int deg_mode(void) {
    static const char *o[] = {"Radians", "Degrees"};
    return in_choice("Angles in", o, 2);
}

static void list_range(double (*gen)(int, void *), void *ctx, double lo, double hi, int deg, const char *var) {
    double sols[64];
    int ns = 0;
    for (int n = -400; n <= 400 && ns < 60; n++) {
        double x = gen(n, ctx);
        if (x != x) continue;
        if (x >= lo - 1e-9 && x <= hi + 1e-9) {
            int dup = 0;
            for (int i = 0; i < ns; i++) if (fabs(sols[i] - x) < 1e-9) dup = 1;
            if (!dup) sols[ns++] = x;
        }
    }
    for (int i = 0; i < ns; i++)
        for (int j = i + 1; j < ns; j++) if (sols[j] < sols[i]) { double t = sols[i]; sols[i] = sols[j]; sols[j] = t; }
    if (!ns) { outf(ANS "No solutions in range"); return; }
    char *o = sbuf();
    for (int i = 0; i < ns; i++) {
        const char *v = deg ? fd(sols[i]) : fx(sols[i]);
        if (strlen(o) + strlen(v) > 150) { outf(ANS "%s = %s", var, o); o = sbuf(); }
        else if (o[0]) strcat(o, ", ");
        strcat(o, v);
    }
    if (o[0]) outf(ANS "%s = %s", var, o);
}

typedef struct { int fn; double a, b, al, U; } gctx;
static double gen_basic(int n, void *c_) {
    gctx *c = c_;
    double h = c->U / 2; /* pi or 180 */
    double y;
    if (c->fn == 0) y = n * h + ((n % 2) ? -c->al : c->al);
    else if (c->fn == 1) { int m = n / 2; y = 2 * m * h + ((n % 2) ? -c->al : c->al); if (n < 0 && (n % 2)) y = 2 * ((n - 1) / 2) * h - c->al; }
    else y = n * h + c->al;
    return (y - c->b) / c->a;
}

void t_general(void) {
    int deg = deg_mode();
    static const char *o[] = {"sin(ax + b) = k", "cos(ax + b) = k", "tan(ax + b) = k"};
    int f = in_choice("Equation", o, 3);
    double a = in_real("a"), b = in_real(deg ? "b (degrees)" : "b"), k = in_real("k");
    if (a == 0) fail("a can't be 0");
    double U = deg ? 360 : 2 * M_PI;
    if (f < 2 && fabs(k) > 1) fail("|k| > 1: no solutions");
    double al = f == 0 ? asin(k) : f == 1 ? acos(k) : atan(k);
    if (deg) al *= 180 / M_PI;
    const char *fn = f == 0 ? "sin" : f == 1 ? "cos" : "tan";
    const char *P = deg ? "180" : S_PI;
    outf("Principal value: %s^-1(%s) = %s%s", fn, fx(k), deg ? fd(al) : fx(al), deg ? S_DEG : "");
    const char *A = deg ? fd(al) : fx(al);
    if (f == 0) outf("ax + b = n%s + (-1)^n(%s)", P, A);
    else if (f == 1) outf("ax + b = 2n%s " S_PM " %s", P, A);
    else outf("ax + b = n%s + %s", P, A);
    char bs[48];
    strcpy(bs, deg ? fd(b) : fx(b));
    if (f == 0) outf(ANS "x = [n%s + (-1)^n(%s) - %s] / %s", P, A, bs, fx(a));
    else if (f == 1) outf(ANS "x = [2n%s " S_PM " %s - %s] / %s", P, A, bs, fx(a));
    else outf(ANS "x = [n%s + %s - %s] / %s", P, A, bs, fx(a));
    outf("(n is any integer)");
    double lo = in_real(deg ? "Range: from (deg)" : "Range: from");
    double hi = in_real(deg ? "Range: to (deg)" : "Range: to");
    gctx c = {f, a, b, al, U};
    list_range(gen_basic, &c, lo, hi, deg, "x");
}

typedef struct { double al, beta, U; } rctx;
static double gen_r(int n, void *c_) {
    rctx *c = c_;
    int m = n / 2;
    if (n < 0 && (n % 2)) m = (n - 1) / 2;
    return m * c->U + ((n % 2) ? -c->beta : c->beta) + c->al;
}

void t_rform(void) {
    int deg = deg_mode();
    in_note("a cos x + b sin x = c");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    double R_ = sqrt(a * a + b * b);
    if (R_ == 0) fail("a and b can't both be 0");
    double al = atan2(b, a);
    double k = deg ? 180 / M_PI : 1;
    outf("Write as R cos(x - " S_AL ")");
    outf("R cos " S_AL " = %s,  R sin " S_AL " = %s", fx(a), fx(b));
    outf("R = " S_SQRT "(%s" S_SQ " + %s" S_SQ ") = %s", fxp(a), fxp(b), fxd(R_));
    outf("tan " S_AL " = %s/%s, " S_AL " = %s", fxp(b), fxp(a), deg ? fdeg(al) : fx(al));
    outf(ANS "%s cos(x - %s) = %s", fx(R_), deg ? fdeg(al) : fx(al), fx(c));
    outf("Equivalently R sin(x + " S_BE "), " S_BE " = %s", deg ? fdeg(atan2(a, b)) : fx(atan2(a, b)));
    outf("Max value %s, min value -%s", fx(R_), fx(R_));
    if (fabs(c) > R_ + 1e-12) { outf(ANS "|c| > R: no solutions"); return; }
    double beta = acos(fmax(-1, fmin(1, c / R_)));
    outf("cos(x - " S_AL ") = %s,  so x - " S_AL " = %s", fx(c / R_), deg ? fdeg(beta) : fx(beta));
    if (deg) outf(ANS "x = 360n " S_PM " %s + %s", fd(beta * k), fd(al * k));
    else outf(ANS "x = 2n" S_PI " " S_PM " %s + %s", fx(beta), fx(al));
    double lo = in_real(deg ? "Range: from (deg)" : "Range: from");
    double hi = in_real(deg ? "Range: to (deg)" : "Range: to");
    rctx cc = {al * k, beta * k, deg ? 360 : 2 * M_PI};
    list_range(gen_r, &cc, lo, hi, deg, "x");
}

typedef struct { double t[2]; int nt; double U; int pi_sol; } tctx;
static double gen_t(int n, void *c_) {
    tctx *c = c_;
    int which = abs(n) % 3;
    int m = n / 3;
    if (which == 2) return c->pi_sol ? c->U / 2 + m * c->U : NAN;
    if (which >= c->nt) return NAN;
    double base = 2 * atan(c->t[which]);
    if (c->U == 360) base *= 180 / M_PI;
    return base + m * c->U;
}

static void t_tform(void) {
    int deg = deg_mode();
    in_note("a cos x + b sin x = c, using t = tan(x/2)");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    outf("sin x = 2t/(1+t" S_SQ "), cos x = (1-t" S_SQ ")/(1+t" S_SQ ")");
    outf("%s(1 - t" S_SQ ") + %s(2t) = %s(1 + t" S_SQ ")", fx(a), fxp(b), fxp(c));
    double q[3] = {c - a, -2 * b, a + c};
    outf("%s = 0", fpoly(q, 2, "t"));
    tctx tc = {{0, 0}, 0, deg ? 360 : 2 * M_PI, 0};
    if (fabs(q[2]) < 1e-12) {
        if (fabs(q[1]) > 1e-12) { tc.t[0] = -q[0] / q[1]; tc.nt = 1; }
        tc.pi_sol = 1;
        outf("t" S_SQ " term vanishes: x = %s is also a solution", deg ? "180" S_DEG : S_PI);
    } else {
        double disc = q[1] * q[1] - 4 * q[2] * q[0];
        if (disc < -1e-12) outf("Discriminant < 0: no real t");
        else {
            tc.t[0] = (-q[1] + sqrt(fmax(0, disc))) / (2 * q[2]);
            tc.t[1] = (-q[1] - sqrt(fmax(0, disc))) / (2 * q[2]);
            tc.nt = disc < 1e-12 ? 1 : 2;
        }
    }
    for (int i = 0; i < tc.nt; i++) {
        double x = 2 * atan(tc.t[i]);
        outf(ANS "t = %s:  x/2 = %s, x = %s", fx(tc.t[i]), deg ? fdeg(x / 2) : fx(x / 2), deg ? fdeg(x) : fx(x));
    }
    outf("General: x = 2tan^-1(t) + %s", deg ? "360n" : "2n" S_PI);
    double lo = in_real(deg ? "Range: from (deg)" : "Range: from");
    double hi = in_real(deg ? "Range: to (deg)" : "Range: to");
    list_range(gen_t, &tc, lo, hi, deg, "x");
}

static void t_sumprod(void) {
    static const char *o[] = {"sin Px + sin Qx", "sin Px - sin Qx", "cos Px + cos Qx", "cos Px - cos Qx"};
    int k = in_choice("Expression", o, 4);
    double P = in_real("P"), Q = in_real("Q");
    double s = (P + Q) / 2, d = (P - Q) / 2;
    const char *A = ftrig("", s, "x") + 1, *B = ftrig("", d, "x") + 1;
    const char *f1[] = {"2sin", "2cos", "2cos", "-2sin"};
    const char *f2[] = {"cos", "sin", "cos", "sin"};
    outf("Factor formula with (P+Q)/2 = %s, (P-Q)/2 = %s", fx(s), fx(d));
    outf(ANS "= %s(%s) %s(%s)", f1[k], A, f2[k], B);
    outf("Setting = 0: each factor = 0");
    outf("%s(%s) = 0  or  %s(%s) = 0", f1[k] + (f1[k][0] == '-' ? 2 : 1), A, f2[k], B);
}

static const calc_t trig_c[] = {
    {"General solution sin/cos/tan", t_general},
    {"a cos x + b sin x = c (R-form)", t_rform},
    {"a cos x + b sin x = c (t-formulae)", t_tform},
    {"Sum to product", t_sumprod},
};

/* ======================= 3x3 MATRICES ======================= */
static void x_det(void) {
    double m[9];
    in_mat("M", m, 3, 3);
    out_mat("M", m, 3, 3);
    double c1 = m[4] * m[8] - m[5] * m[7], c2 = m[3] * m[8] - m[5] * m[6], c3 = m[3] * m[7] - m[4] * m[6];
    outf("Expand along row 1:");
    outf("%s[(%s)(%s) - (%s)(%s)]", fx(m[0]), fx(m[4]), fx(m[8]), fx(m[5]), fx(m[7]));
    outf("- %s[(%s)(%s) - (%s)(%s)]", fxp(m[1]), fx(m[3]), fx(m[8]), fx(m[5]), fx(m[6]));
    outf("+ %s[(%s)(%s) - (%s)(%s)]", fxp(m[2]), fx(m[3]), fx(m[7]), fx(m[4]), fx(m[6]));
    outf("= %s(%s) - %s(%s) + %s(%s)", fx(m[0]), fx(c1), fxp(m[1]), fx(c2), fxp(m[2]), fx(c3));
    double d = m[0] * c1 - m[1] * c2 + m[2] * c3;
    outf(ANS "det M = %s", fx(d));
    outf("Volume scale factor = %s", fx(fabs(d)));
    if (fabs(d) < 1e-12) outf("Singular (no inverse)");
}

static void x_inv(void) {
    double m[9], mi[9], co[9];
    in_mat("M", m, 3, 3);
    out_mat("M", m, 3, 3);
    double d = det3((const double(*)[3])m);
    outf("det M = %s", fx(d));
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++) {
            int r0 = (i + 1) % 3, r1 = (i + 2) % 3, c0 = (j + 1) % 3, c1 = (j + 2) % 3;
            if (r0 > r1) { int t = r0; r0 = r1; r1 = t; }
            if (c0 > c1) { int t = c0; c0 = c1; c1 = t; }
            mi[i * 3 + j] = m[r0 * 3 + c0] * m[r1 * 3 + c1] - m[r0 * 3 + c1] * m[r1 * 3 + c0];
            co[i * 3 + j] = ((i + j) % 2 ? -1 : 1) * mi[i * 3 + j];
        }
    outf(HEAD "Matrix of minors");
    out_mat(NULL, mi, 3, 3);
    outf(HEAD "Cofactors (sign pattern + - +)");
    out_mat(NULL, co, 3, 3);
    double adj[9];
    for (int i = 0; i < 3; i++) for (int j = 0; j < 3; j++) adj[i * 3 + j] = co[j * 3 + i];
    outf(HEAD "Adjugate (transpose)");
    out_mat(NULL, adj, 3, 3);
    if (fabs(d) < 1e-12) { outf(ANS "det = 0: no inverse"); return; }
    outf(ANS "M^-1 = (1/%s) " S_TIMES " adj M", fx(d));
    for (int i = 0; i < 9; i++) adj[i] /= d;
    out_mat(NULL, adj, 3, 3);
}

static void x_solve(void) {
    in_note("Each row: a, b, c, d for ax + by + cz = d");
    double r[12];
    in_mat("Eq", r, 3, 4);
    double A[9], B[3];
    for (int i = 0; i < 3; i++) { A[i * 3] = r[i * 4]; A[i * 3 + 1] = r[i * 4 + 1]; A[i * 3 + 2] = r[i * 4 + 2]; B[i] = r[i * 4 + 3]; }
    double d = det3((const double(*)[3])A);
    outf("det(coefficients) = %s", fx(d));
    if (fabs(d) > 1e-12) {
        double inv[3][3], X[3];
        inv3((const double(*)[3])A, inv);
        matmul(&inv[0][0], B, X, 3, 3, 1);
        outf("Non-zero, so unique solution: X = M^-1 B");
        outf(ANS "x = %s, y = %s, z = %s", fx(X[0]), fx(X[1]), fx(X[2]));
        outf("Three planes meet at a single point");
        return;
    }
    int ra = rank_n(A, 3, 3), rb = rank_n(r, 3, 4);
    outf("det = 0: no unique solution");
    outf("rank(M) = %d, rank(M|B) = %d", ra, rb);
    int np = 0;
    for (int i = 0; i < 3; i++)
        for (int j = i + 1; j < 3; j++) {
            double c[3];
            cross3(A + 3 * i, A + 3 * j, c);
            if (norm3(c) < 1e-9) np++;
        }
    if (ra == rb) {
        if (ra == 1) { outf(ANS "Consistent: all three planes are the same plane"); return; }
        outf(ANS "Consistent: infinitely many solutions");
        outf(ANS "Planes form a sheaf (meet in a common line)");
        /* line from two independent rows */
        int i1 = 0, i2 = 1;
        double c[3];
        for (int i = 0; i < 3; i++) for (int j = i + 1; j < 3; j++) { cross3(A + 3 * i, A + 3 * j, c); if (norm3(c) > 1e-9) { i1 = i; i2 = j; goto found; } }
    found:
        cross3(A + 3 * i1, A + 3 * i2, c);
        int z = 0;
        for (int i = 1; i < 3; i++) if (fabs(c[i]) > fabs(c[z])) z = i;
        int a1 = (z + 1) % 3, a2 = (z + 2) % 3;
        double dd = A[i1 * 3 + a1] * A[i2 * 3 + a2] - A[i1 * 3 + a2] * A[i2 * 3 + a1];
        double p[3] = {0};
        p[a1] = (B[i1] * A[i2 * 3 + a2] - B[i2] * A[i1 * 3 + a2]) / dd;
        p[a2] = (A[i1 * 3 + a1] * B[i2] - A[i2 * 3 + a1] * B[i1]) / dd;
        double s[3];
        memcpy(s, c, sizeof s);
        if (pintscale(s, 2)) memcpy(c, s, sizeof c);
        out_line3("Line", p, c);
        return;
    }
    outf(ANS "Inconsistent: no solutions");
    if (ra == 1) outf(ANS "Planes are parallel (not all the same)");
    else if (np == 0) outf(ANS "Planes form a triangular prism");
    else outf(ANS "Two planes are parallel, the third cuts both");
}

static void x_detk(void) {
    in_note("Entries can contain k, e.g. 2, k, 1");
    node *e[9];
    for (int i = 0; i < 3; i++) {
        char lab[16] = "Row ";
        strcat(lab, fd(i + 1));
        in_vecfn(lab, e + 3 * i, 3, 'k');
    }
    /* evaluate det at k = -3..3 and interpolate (degree <= 6) */
    double ys[7];
    for (int t = 0; t < 7; t++) {
        double m[9];
        for (int i = 0; i < 9; i++) m[i] = ex_evalr(e[i], 'k', t - 3);
        ys[t] = det3((const double(*)[3])m);
    }
    /* Newton divided differences -> monomial coefficients */
    double P[7] = {0};
    double A[49], b[7];
    for (int t = 0; t < 7; t++) { double x = t - 3, p = 1; for (int j = 0; j < 7; j++) { A[t * 7 + j] = p; p *= x; } b[t] = ys[t]; }
    lsolve(A, b, 7);
    for (int i = 0; i < 7; i++) P[i] = fabs(b[i]) < 1e-8 ? 0 : b[i];
    int dg = 6;
    while (dg > 0 && P[dg] == 0) dg--;
    outf("Expand the determinant along row 1:");
    outf(ANS "det = %s", fpoly(P, dg, "k"));
    if (dg == 0) { outf("Constant: %s", P[0] == 0 ? "always singular" : "never singular"); return; }
    cx r[8];
    int n = proots_r(P, dg, r);
    outf("Singular when det = 0:");
    int nr = 0;
    for (int i = 0; i < n; i++) if (r[i].im == 0) { outf(ANS "k = %s", fx(r[i].re)); nr++; }
    if (!nr) outf(ANS "No real k: never singular");
    outf("For other k the matrix is non-singular");
}

static const calc_t mat3_c[] = {
    {"3" S_TIMES "3 determinant", x_det},
    {"3" S_TIMES "3 inverse (minors, cofactors)", x_inv},
    {"3 equations (unique/sheaf/prism)", x_solve},
    {"Determinant with unknown k", x_detk},
};

/* ======================= MACLAURIN & PARTIAL FRACTIONS ======================= */
static void s_maclaurin(void) {
    node *f = in_fn("f(x)", 'x');
    int n = in_int("Up to power x^n, n =", 1, 8);
    ser s;
    const char *err;
    if (!ex_series(f, 'x', 0, &s, &err)) fail(err ? err : "Can't expand at x = 0");
    double fact = 1;
    outf("f^(r)(0) values:");
    for (int k = 0; k <= n; k++) {
        if (k) fact *= k;
        char nm[16] = "f";
        if (k == 1) strcat(nm, "'");
        else if (k == 2) strcat(nm, "''");
        else if (k == 3) strcat(nm, "'''");
        else if (k) { strcat(nm, "^("); strcat(nm, fd(k)); strcat(nm, ")"); }
        outf("  %s(0) = %s", nm, fx(s.c[k] * fact));
    }
    outf("f(x) = f(0) + f'(0)x + f''(0)x" S_SQ "/2! + ...");
    char *o = sbuf();
    int first = 1;
    for (int k = 0; k <= n; k++) {
        if (fabs(s.c[k]) < 1e-12) continue;
        char t[16] = "";
        if (k) { strcpy(t, "x"); if (k > 1) strcat(t, sup(k)); }
        if (k > 3) { strcpy(t, "x^"); strcat(t, fd(k)); }
        const char *term = fcoef(s.c[k], t, first);
        if (strlen(o) + strlen(term) > 150) { outf(ANS "%s", o); o = sbuf(); first = 0; }
        strcat(o, term);
        first = 0;
    }
    outf(ANS "f(x) " S_APX " %s", first ? "0" : o);
    double v = 0, p = 1;
    for (int k = 0; k <= n; k++) { v += s.c[k] * p; p *= 0.1; }
    outf("Check x = 0.1: series %s, f = %s", fd(v), fd(ex_evalr(f, 'x', 0.1)));
}

/* " + 11/(8(x + 2))" */
static const char *pfterm(double c, const char *den, int first, const char *pw) {
    char *o = sbuf();
    if (first) { if (c < 0) strcpy(o, "-"); }
    else strcpy(o, c < 0 ? " - " : " + ");
    long p, q;
    if (ratapprox(fabs(c), 1000, &p, &q)) {
        strcat(o, fd(p));
        strcat(o, "/");
        if (q != 1) { strcat(o, "("); strcat(o, fd(q)); }
        strcat(o, "("); strcat(o, den); strcat(o, ")"); strcat(o, pw);
        if (q != 1) strcat(o, ")");
    } else {
        strcat(o, fd(fabs(c))); strcat(o, "/("); strcat(o, den); strcat(o, ")"); strcat(o, pw);
    }
    return o;
}

void s_partial(void) {
    static const char *o[] = {"(ax+b)(cx+d)", "(ax+b)(cx+d)(ex+f)", "(ax+b)(cx+d)" S_SQ, "(ax+b)(cx" S_SQ "+d)"};
    int k = in_choice("Denominator", o, 4);
    double nm[3];
    in_vec("Numerator p, q, r (px" S_SQ "+qx+r)", nm, 3);
    double L[3][2];
    int nl = k == 1 ? 3 : 2;
    in_vec("a, b (ax + b)", L[0], 2);
    double Qd[2];
    if (k == 3) in_vec("c, d (cx" S_SQ " + d)", Qd, 2);
    else in_vec("c, d (cx + d)", L[1], 2);
    if (k == 1) in_vec("e, f (ex + f)", L[2], 2);
    if (k == 2) { L[2][0] = L[1][0]; L[2][1] = L[1][1]; nl = 3; }
    /* denominator polynomial */
    double D[4] = {L[0][1], L[0][0]};
    int dd = 1;
    if (k == 3) { double q[3] = {Qd[1], 0, Qd[0]}; pmul(D, 1, q, 2, D); dd = 3; }
    else for (int i = 1; i < nl; i++) { double l[2] = {L[i][1], L[i][0]}; pmul(D, dd, l, 1, D); dd++; }
    double N_[4] = {nm[2], nm[1], nm[0], 0};
    int dn = 2;
    while (dn > 0 && N_[dn] == 0) dn--;
    outf("Denominator = %s", fpoly(D, dd, "x"));
    double quot[4] = {0};
    int dq = -1;
    if (dn >= dd) {
        double rem[4];
        pdiv(N_, dn, D, dd, quot, rem);
        dq = dn - dd;
        outf("Improper: divide first. Quotient = %s", fpoly(quot, dq, "x"));
        for (int i = 0; i < 4; i++) N_[i] = i < dd ? rem[i] : 0;
        dn = dd - 1;
    }
    /* unknowns */
    int nu = 3;
    if (k == 0) nu = 2;
    const char *form;
    if (k == 0) form = "A/(ax+b) + B/(cx+d)";
    else if (k == 1) form = "A/(ax+b) + B/(cx+d) + C/(ex+f)";
    else if (k == 2) form = "A/(ax+b) + B/(cx+d) + C/(cx+d)" S_SQ;
    else form = "A/(ax+b) + (Bx+C)/(cx" S_SQ "+d)";
    outf("Form: %s", form);
    /* basis numerators: N = sum u_i * basis_i(x) */
    double M[9], rhs[3];
    double xs[3] = {0.37, -1.29, 2.71};
    for (int t = 0; t < nu; t++) {
        double x = xs[t];
        double l0 = L[0][0] * x + L[0][1], l1 = (k == 3) ? 0 : L[1][0] * x + L[1][1];
        double Dv = peval_r(D, dd, x);
        double b[3];
        if (k == 0) { b[0] = Dv / l0; b[1] = Dv / l1; }
        else if (k == 1) { double l2 = L[2][0] * x + L[2][1]; b[0] = Dv / l0; b[1] = Dv / l1; b[2] = Dv / l2; }
        else if (k == 2) { b[0] = Dv / l0; b[1] = Dv / l1; b[2] = Dv / (l1 * l1); }
        else { double q = Qd[0] * x * x + Qd[1]; b[0] = Dv / l0; b[1] = x * Dv / q; b[2] = Dv / q; }
        for (int i = 0; i < nu; i++) M[t * nu + i] = b[i];
        rhs[t] = peval_r(N_, dn, x);
    }
    if (!lsolve(M, rhs, nu)) fail("Repeated or zero factor: check inputs");
    outf("Multiply through by the denominator, then");
    outf("substitute roots / compare coefficients:");
    /* cover-up working for linear factors */
    for (int i = 0; i < (k == 2 ? 1 : k == 3 ? 1 : nl); i++) {
        double root = -L[i][1] / L[i][0];
        outf("  x = %s: gives %s = %s", fx(root), i == 0 ? "A" : i == 1 ? "B" : "C", fx(rhs[i]));
    }
    if (k == 2) outf("  x = %s: gives C = %s, then compare for B", fx(-L[1][1] / L[1][0]), fx(rhs[2]));
    if (k == 3) outf("  compare x" S_SQ " and constant terms for B, C");
    outf(ANS "A = %s, B = %s%s%s", fx(rhs[0]), fx(rhs[1]), nu > 2 ? ", C = " : "", nu > 2 ? fx(rhs[2]) : "");
    char *res = sbuf();
    if (dq >= 0) strcat(res, fpoly(quot, dq, "x"));
    char t1[40], t2[40], t3[40];
    strcpy(t1, fpoly((double[]){L[0][1], L[0][0]}, 1, "x"));
    if (k != 3) strcpy(t2, fpoly((double[]){L[1][1], L[1][0]}, 1, "x"));
    else strcpy(t2, fpoly((double[]){Qd[1], 0, Qd[0]}, 2, "x"));
    if (k == 1) strcpy(t3, fpoly((double[]){L[2][1], L[2][0]}, 1, "x"));
    outf(ANS "= %s%s", res, pfterm(rhs[0], t1, res[0] == 0, ""));
    if (k == 3) outf(ANS "  + (%s)/(%s)", fpoly((double[]){rhs[2], rhs[1]}, 1, "x"), t2);
    else outf(ANS "  %s", pfterm(rhs[1], t2, 0, ""));
    if (k == 1) outf(ANS "  %s", pfterm(rhs[2], t3, 0, ""));
    if (k == 2) outf(ANS "  %s", pfterm(rhs[2], t2, 0, S_SQ));
}

static const calc_t series_c[] = {
    {"Maclaurin series of f(x)", s_maclaurin},
    {"Partial fractions", s_partial},
};

/* ======================= CALCULUS ======================= */
static double in_lim(const char *l) {
    in_note("Use " S_INF " (F6 page 3) for infinity");
    cx v = in_cx(l);
    return v.re;
}

void k_integral(void) {
    node *f = in_fn("f(x)", 'x');
    double a = in_lim("Lower limit"), b = in_lim("Upper limit");
    int ok;
    double v = integrate(f, 'x', a, b, &ok);
    if (isinf(a) || isinf(b)) outf("Improper: take the limit as the bound " S_ARR " " S_INF);
    outf(S_INT " from %s to %s of %s dx", fx(a), fx(b), ex_src(f));
    if (!ok) outf(ANS "Doesn't converge (or numerically unstable)");
    else outf(ANS "= %s", fxd(v));
}

static void k_mean(void) {
    node *f = in_fn("f(x)", 'x');
    double a = in_real("a"), b = in_real("b");
    if (a == b) fail("a = b");
    int ok;
    double v = integrate(f, 'x', a, b, &ok);
    outf("Mean value = (1/(b - a)) " S_INT " f(x) dx");
    outf(S_INT " = %s", fxd(v));
    outf(ANS "Mean = %s/%s = %s", fx(v), fxp(b - a), fxd(v / (b - a)));
}

static void k_volume(void) {
    static const char *o[] = {"About x-axis: V = " S_PI S_INT "y" S_SQ " dx", "About y-axis: V = " S_PI S_INT "x" S_SQ " dy"};
    int k = in_choice("Rotation", o, 2);
    node *f;
    if (k == 0) f = in_fn("y = f(x)", 'x');
    else f = in_fn("x = g(y)", 'y');
    char v = k == 0 ? 'x' : 'y';
    double a = in_real("Lower limit"), b = in_real("Upper limit");
    /* integrate f^2 via composed node: build string */
    char s[240];
    strcpy(s, "(");
    strcat(s, ex_src(f));
    strcat(s, ")^2");
    const char *err;
    node *g = ex_parse(s, &err);
    if (!g) fail("Expression too long");
    int ok;
    double I = integrate(g, v, a, b, &ok);
    outf("V = " S_PI S_INT " (%s)" S_SQ " d%c from %s to %s", ex_src(f), v, fx(a), fx(b));
    outf(S_INT " = %s", fxd(I));
    outf(ANS "V = " S_PI " " S_TIMES " %s = %s", fx(I), fxd(I * M_PI));
}

static const char *inv_coef(double d) {
    char *o = sbuf();
    if (fabs(d - 1) < 1e-12) return o;
    strcpy(o, "(1/");
    strcat(o, fx(d));
    strcat(o, ") ");
    return o;
}

static void k_standard(void) {
    static const char *o[] = {"1/(px" S_SQ " + qx + r)", "1/" S_SQRT "(px" S_SQ " + qx + r)"};
    int k = in_choice("Integrand", o, 2);
    double p = in_real("p"), q = in_real("q"), r = in_real("r");
    if (p == 0) fail("p can't be 0");
    double P = fabs(p), h = q / (2 * p), K = r / p - h * h, Kp = -K;
    char u[32];
    strcpy(u, fshift("x", -h));
    outf("Complete the square:");
    if (p > 0) outf("%s = %s[%s" S_SQ "%s]", fpoly((double[]){r, q, p}, 2, "x"), p == 1 ? "" : fx(p), u, fcoef(K, "", 0));
    else outf("%s = %s[%s - %s" S_SQ "]", fpoly((double[]){r, q, p}, 2, "x"), P == 1 ? "" : fx(P), fx(Kp), u);
    int typ; /* 0 atan,1 ln-diff (p>0,K<0),2 recip,3 ln(a+u)/(a-u) (p<0),4 asinh,5 acosh,6 ln|u|,7 asin */
    if (k == 0) {
        if (p > 0 && K > 0) { typ = 0; outf(ANS "= %stan^-1(%s/%s) + c", inv_coef(p * sqrt(K)), u, fx(sqrt(K))); }
        else if (p > 0 && K < 0) { typ = 1; outf(ANS "= %sln|(%s - %s)/(%s + %s)| + c", inv_coef(2 * p * sqrt(-K)), u, fx(sqrt(-K)), u, fx(sqrt(-K))); }
        else if (p > 0 || fabs(K) < 1e-12) { typ = 2; outf(ANS "= -1/(%s%s) + c", fx(p), u); }
        else if (Kp > 0) { typ = 3; outf(ANS "= %sln|(%s + %s)/(%s - %s)| + c", inv_coef(2 * P * sqrt(Kp)), fx(sqrt(Kp)), u, fx(sqrt(Kp)), u); }
        else { typ = 0; outf("Take out -1: = -" S_INT "1/(%s[%s" S_SQ " + %s])", inv_coef(P), u, fx(-Kp)); outf(ANS "= -%stan^-1(%s/%s) + c", fx(P * sqrt(-Kp)), u, fx(sqrt(-Kp))); }
    } else {
        if (p > 0 && K > 0) { typ = 4; outf(ANS "= %ssinh^-1(%s/%s) + c", inv_coef(sqrt(p)), u, fx(sqrt(K))); }
        else if (p > 0 && K < 0) { typ = 5; outf(ANS "= %scosh^-1(%s/%s) + c", inv_coef(sqrt(p)), u, fx(sqrt(-K))); }
        else if (p > 0) { typ = 6; outf(ANS "= %sln|%s| + c", inv_coef(sqrt(p)), u); }
        else if (Kp > 0) { typ = 7; outf(ANS "= %ssin^-1(%s/%s) + c", inv_coef(sqrt(P)), u, fx(sqrt(Kp))); }
        else { outf(ANS "Square root of a negative: no real integral"); return; }
        if (typ == 4 || typ == 5) outf("  = %sln(%s + " S_SQRT "(%s" S_SQ "%s)) + c", inv_coef(sqrt(p)), u, u, fcoef(K, "", 0));
    }
    static const char *lo[] = {"Indefinite only", "Evaluate between limits"};
    if (in_choice("Limits?", lo, 2) == 0) return;
    double a = in_real("Lower limit"), b = in_real("Upper limit");
    double F[2];
    double xs[2] = {a, b};
    for (int i = 0; i < 2; i++) {
        double U = xs[i] + h;
        switch (typ) {
        case 0: F[i] = p > 0 ? atan(U / sqrt(K)) / (p * sqrt(K)) : -atan(U / sqrt(-Kp)) / (P * sqrt(-Kp)); break;
        case 1: { double m = sqrt(-K); F[i] = log(fabs((U - m) / (U + m))) / (2 * p * m); break; }
        case 2: F[i] = -1 / (p * U); break;
        case 3: { double m = sqrt(Kp); F[i] = log(fabs((m + U) / (m - U))) / (2 * P * m); break; }
        case 4: F[i] = asinh(U / sqrt(K)) / sqrt(p); break;
        case 5: F[i] = log(fabs(U) + sqrt(U * U + K)) / sqrt(p); break;
        case 6: F[i] = log(fabs(U)) / sqrt(p); break;
        default: F[i] = asin(U / sqrt(Kp)) / sqrt(P);
        }
    }
    outf("F(%s) = %s,  F(%s) = %s", fx(b), fxd(F[1]), fx(a), fxd(F[0]));
    outf(ANS "= %s", fxd(F[1] - F[0]));
}

void k_deriv(void) {
    node *f = in_fn("f(x)", 'x');
    double x = in_real("At x =");
    ser s;
    const char *err;
    if (ex_series(f, 'x', x, &s, &err)) {
        outf("f(%s) = %s", fx(x), fxd(s.c[0]));
        outf(ANS "f'(%s) = %s", fx(x), fxd(s.c[1]));
        outf(ANS "f''(%s) = %s", fx(x), fxd(2 * s.c[2]));
    } else {
        outf(ANS "f'(%s) " S_APX " %s", fx(x), fd(nderiv(f, 'x', x)));
    }
}

static void k_ref(void) {
    outf(HEAD "Derivatives");
    outf("d/dx sin^-1 x = 1/" S_SQRT "(1 - x" S_SQ ")");
    outf("d/dx cos^-1 x = -1/" S_SQRT "(1 - x" S_SQ ")");
    outf("d/dx tan^-1 x = 1/(1 + x" S_SQ ")");
    outf("d/dx sinh x = cosh x,  d/dx cosh x = sinh x");
    outf("d/dx tanh x = sech" S_SQ "x");
    outf("d/dx sinh^-1 x = 1/" S_SQRT "(x" S_SQ " + 1)");
    outf("d/dx cosh^-1 x = 1/" S_SQRT "(x" S_SQ " - 1)");
    outf("d/dx tanh^-1 x = 1/(1 - x" S_SQ ")");
    outf(HEAD "Integrals");
    outf(S_INT "1/(a" S_SQ " + x" S_SQ ") = (1/a)tan^-1(x/a)");
    outf(S_INT "1/" S_SQRT "(a" S_SQ " - x" S_SQ ") = sin^-1(x/a)");
    outf(S_INT "1/" S_SQRT "(x" S_SQ " + a" S_SQ ") = sinh^-1(x/a)");
    outf(S_INT "1/" S_SQRT "(x" S_SQ " - a" S_SQ ") = cosh^-1(x/a)");
    outf(S_INT "1/(a" S_SQ " - x" S_SQ ") = (1/2a)ln|(a+x)/(a-x)|");
    outf(HEAD "Log forms");
    outf("sinh^-1 x = ln(x + " S_SQRT "(x" S_SQ " + 1))");
    outf("cosh^-1 x = ln(x + " S_SQRT "(x" S_SQ " - 1)), x " S_GE " 1");
    outf("tanh^-1 x = (1/2)ln((1 + x)/(1 - x)), |x| < 1");
}

static const calc_t calc_c[] = {
    {"Definite / improper integral", k_integral},
    {"Mean value of f(x)", k_mean},
    {"Volume of revolution", k_volume},
    {S_INT "1/(quadratic), " S_INT "1/" S_SQRT "(quadratic)", k_standard},
    {"Derivative at a point", k_deriv},
    {"Standard results (reference)", k_ref},
};

/* ======================= POLAR ======================= */
void p_convert(void) {
    static const char *o[] = {"Polar (r, " S_TH ") " S_ARR " Cartesian", "Cartesian " S_ARR " polar"};
    int k = in_choice("Convert", o, 2);
    if (k == 0) {
        double r = in_real("r"), t = in_real(S_TH);
        outf("x = r cos " S_TH " = %s", fx(r * cos(t)));
        outf("y = r sin " S_TH " = %s", fx(r * sin(t)));
        outf(ANS "(%s, %s)", fx(r * cos(t)), fx(r * sin(t)));
    } else {
        double x = in_real("x"), y = in_real("y");
        double r = hypot(x, y), t = atan2(y, x);
        outf("r = " S_SQRT "(x" S_SQ " + y" S_SQ ") = %s", fxd(r));
        outf(S_TH " = %s (check quadrant)", fang(t));
        outf(ANS "(%s, %s)", fx(r), fx(t));
        if (t < 0) outf("or with 0 " S_LE " " S_TH " < 2" S_PI ": " S_TH " = %s", fx(t + 2 * M_PI));
    }
}

static void p_area(void) {
    node *f = in_fn("r = f(" S_TH ")", 'T');
    double a = in_real(S_TH " from"), b = in_real(S_TH " to");
    char s[240];
    strcpy(s, "(");
    strcat(s, ex_src(f));
    strcat(s, ")^2");
    const char *err;
    node *g = ex_parse(s, &err);
    if (!g) fail("Expression too long");
    int ok;
    double I = integrate(g, 'T', a, b, &ok);
    outf("Area = (1/2) " S_INT " r" S_SQ " d" S_TH);
    outf(S_INT " (%s)" S_SQ " d" S_TH " = %s", ex_src(f), fxd(I));
    outf(ANS "Area = %s", fxd(I / 2));
}

static node *pf;
static int pmode;
static double polar_g(double t) {
    ser s;
    const char *err;
    double r, dr;
    if (ex_series(pf, 'T', t, &s, &err)) { r = s.c[0]; dr = s.c[1]; }
    else { r = ex_evalr(pf, 'T', t); dr = nderiv(pf, 'T', t); }
    return pmode == 0 ? dr * sin(t) + r * cos(t) : dr * cos(t) - r * sin(t);
}

static void p_tangent(void) {
    static const char *o[] = {"Parallel to initial line", "Perpendicular to initial line"};
    pmode = in_choice("Tangent", o, 2);
    pf = in_fn("r = f(" S_TH ")", 'T');
    double a = in_real(S_TH " from"), b = in_real(S_TH " to");
    if (pmode == 0) outf("Parallel: d(r sin" S_TH ")/d" S_TH " = 0");
    else outf("Perpendicular: d(r cos" S_TH ")/d" S_TH " = 0");
    outf("= (dr/d" S_TH ")%s %s r %s" S_TH, pmode == 0 ? "sin" S_TH : "cos" S_TH, pmode == 0 ? "+" : "-", pmode == 0 ? "cos" : "sin");
    double roots[30];
    int n = nsolve_fn(polar_g, a, b, roots, 30);
    if (!n) { outf(ANS "No points in range"); return; }
    for (int i = 0; i < n; i++) {
        double t = roots[i], r = ex_evalr(pf, 'T', t);
        outf(ANS S_TH " = %s, r = %s", fx(t), fx(r));
        outf("   (x, y) = (%s, %s)", fx(r * cos(t)), fx(r * sin(t)));
    }
}

static const calc_t polar_c[] = {
    {"Convert polar / Cartesian", p_convert},
    {"Area enclosed (1/2)" S_INT "r" S_SQ "d" S_TH, p_area},
    {"Tangents parallel/perp to initial line", p_tangent},
};

/* ======================= HYPERBOLIC ======================= */
static void h_eval(void) {
    double x = in_real("x");
    outf("sinh x = (e^x - e^-x)/2 = %s", fxd(sinh(x)));
    outf("cosh x = (e^x + e^-x)/2 = %s", fxd(cosh(x)));
    outf("tanh x = %s", fxd(tanh(x)));
    outf(HEAD "Inverses");
    outf("sinh^-1 x = ln(%s + " S_SQRT "(%s)) = %s", fx(x), fx(x * x + 1), fxd(asinh(x)));
    if (x >= 1) outf("cosh^-1 x = ln(%s + " S_SQRT "(%s)) = %s", fx(x), fx(x * x - 1), fxd(acosh(x)));
    else outf("cosh^-1 x undefined (x < 1)");
    if (fabs(x) < 1) outf("tanh^-1 x = (1/2)ln(%s) = %s", fx((1 + x) / (1 - x)), fxd(atanh(x)));
    else outf("tanh^-1 x undefined (|x| " S_GE " 1)");
}

static void h_solve(void) {
    in_note("a cosh x + b sinh x = c");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    outf("%s(e^x + e^-x)/2%s(e^x - e^-x)/2 = %s", fx(a), fcoef(b, "", 0), fx(c));
    outf("Multiply by 2e^x, let u = e^x:");
    double q[3] = {a - b, -2 * c, a + b};
    outf("%s = 0", fpoly(q, 2, "u"));
    double us[2];
    int nu = 0;
    if (fabs(q[2]) < 1e-12) { if (fabs(q[1]) > 1e-12) us[nu++] = -q[0] / q[1]; }
    else {
        double d = q[1] * q[1] - 4 * q[2] * q[0];
        outf("Discriminant = %s", fx(d));
        if (d >= -1e-12) {
            us[nu++] = (-q[1] + sqrt(fmax(0, d))) / (2 * q[2]);
            if (d > 1e-12) us[nu++] = (-q[1] - sqrt(fmax(0, d))) / (2 * q[2]);
        }
    }
    int any = 0;
    for (int i = 0; i < nu; i++) {
        if (us[i] <= 0) { outf("u = %s rejected (e^x > 0)", fx(us[i])); continue; }
        outf("u = %s", fx(us[i]));
        outf(ANS "x = ln(%s) = %s", fx(us[i]), fd(log(us[i])));
        any = 1;
    }
    if (!any) outf(ANS "No real solutions");
}

static void h_inv(void) {
    static const char *o[] = {"sinh x = k", "cosh x = k", "tanh x = k"};
    int f = in_choice("Solve", o, 3);
    double k = in_real("k");
    if (f == 0) {
        outf("x = sinh^-1 k = ln(k + " S_SQRT "(k" S_SQ " + 1))");
        outf(ANS "x = ln(%s + " S_SQRT "%s) = %s", fx(k), fxp(k * k + 1), fd(asinh(k)));
    } else if (f == 1) {
        if (k < 1) fail("cosh x " S_GE " 1: no solutions");
        outf("x = " S_PM "ln(k + " S_SQRT "(k" S_SQ " - 1))");
        outf(ANS "x = " S_PM "ln(%s + " S_SQRT "%s)", fx(k), fxp(k * k - 1));
        outf(ANS "  = " S_PM "%s", fd(acosh(k)));
    } else {
        if (fabs(k) >= 1) fail("|tanh x| < 1: no solutions");
        outf("x = (1/2)ln((1 + k)/(1 - k))");
        outf(ANS "x = (1/2)ln(%s) = %s", fx((1 + k) / (1 - k)), fxd(atanh(k)));
    }
}

static const calc_t hyp_c[] = {
    {"Evaluate sinh/cosh/tanh + inverses", h_eval},
    {"Solve sinh/cosh/tanh x = k", h_inv},
    {"Solve a cosh x + b sinh x = c", h_solve},
};

/* ======================= DIFFERENTIAL EQUATIONS ======================= */
typedef struct { int type; double m1, m2, p, q; char str[120]; } cfun; /* 0 distinct real, 1 repeated, 2 complex */

static cfun aux_solve(double a, double b, double c, const char *v, const char *dep) {
    cfun r;
    double q[3] = {c, b, a};
    outf("Auxiliary equation: %s = 0", fpoly(q, 2, "m"));
    double d = b * b - 4 * a * c;
    outf("Discriminant = %s", fx(d));
    if (d > 1e-12) {
        r.type = 0; r.m1 = (-b + sqrt(d)) / (2 * a); r.m2 = (-b - sqrt(d)) / (2 * a);
        outf("m = %s, %s (distinct real)", fx(r.m1), fx(r.m2));
        strcpy(r.str, "A"); strcat(r.str, fexp(r.m1, v)); strcat(r.str, " + B"); strcat(r.str, fexp(r.m2, v));
        outf(ANS "CF: %s = %s", dep, r.str);
    } else if (d > -1e-12) {
        r.type = 1; r.m1 = -b / (2 * a);
        outf("m = %s (repeated)", fx(r.m1));
        strcpy(r.str, "(A + B"); strcat(r.str, v); strcat(r.str, ")"); strcat(r.str, fexp(r.m1, v));
        outf(ANS "CF: %s = %s", dep, r.str);
    } else {
        r.type = 2; r.p = -b / (2 * a); r.q = sqrt(-d) / (2 * a);
        outf("m = %s " S_PM " %si (complex)", fx(r.p), fx(r.q));
        strcpy(r.str, fexp(r.p, v)); strcat(r.str, "(A "); strcat(r.str, ftrig("cos", r.q, v)); strcat(r.str, " + B "); strcat(r.str, ftrig("sin", r.q, v)); strcat(r.str, ")");
        outf(ANS "CF: %s = %s", dep, r.str);
    }
    return r;
}

/* basis function values for CF */
static void cf_basis(cfun *c, double x, double *y1, double *d1, double *y2, double *d2) {
    if (c->type == 0) { *y1 = exp(c->m1 * x); *d1 = c->m1 * *y1; *y2 = exp(c->m2 * x); *d2 = c->m2 * *y2; }
    else if (c->type == 1) { double e = exp(c->m1 * x); *y1 = e; *d1 = c->m1 * e; *y2 = x * e; *d2 = e + c->m1 * x * e; }
    else {
        double e = exp(c->p * x), co = cos(c->q * x), si = sin(c->q * x);
        *y1 = e * co; *d1 = e * (c->p * co - c->q * si);
        *y2 = e * si; *d2 = e * (c->p * si + c->q * co);
    }
}

static void de_second(void) {
    in_note("a y'' + b y' + c y = f(x)");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    if (a == 0) fail("a can't be 0");
    static const char *o[] = {"f(x) = 0", "Polynomial px" S_SQ " + qx + r", "p e^(kx)", "p cos kx + q sin kx"};
    int ft = in_choice("f(x)", o, 4);
    double P[3] = {0}, k = 0, p = 0, q = 0;
    if (ft == 1) { double v[3]; in_vec("p, q, r", v, 3); P[2] = v[0]; P[1] = v[1]; P[0] = v[2]; }
    if (ft == 2) { p = in_real("p"); k = in_real("k"); }
    if (ft == 3) { p = in_real("p (cos)"); q = in_real("q (sin)"); k = in_real("k"); }
    cfun cf = aux_solve(a, b, c, "x", "y");
    /* particular integral as function: store representation */
    double pc[5] = {0};
    int pdeg = -1;
    double lam = 0, mu = 0;
    int s = 0;
    char pi[160] = "";
    if (ft == 1) {
        s = fabs(c) > 1e-12 ? 0 : (fabs(b) > 1e-12 ? 1 : 2);
        int n = 2;
        while (n > 0 && P[n] == 0) n--;
        int nu = n + 1;
        outf("Try y = %s%s", s ? (s == 1 ? "x(" : "x" S_SQ "(") : "", n == 2 ? "ax" S_SQ " + bx + c" : n == 1 ? "ax + b" : "a");
        if (s) outf("(multiplied by x%s since %s)", s == 2 ? S_SQ : "", s == 1 ? "c = 0" : "b = c = 0");
        if (s) outf(")");
        /* unknown coefficients for x^(s..s+n) */
        double M[9] = {0}, rhs[3];
        for (int j = 0; j < nu; j++) {
            int pw = s + j;
            /* L[x^pw] = a pw(pw-1) x^(pw-2) + b pw x^(pw-1) + c x^pw ; match powers 0..n */
            double Lc[6] = {0};
            if (pw >= 2) Lc[pw - 2] += a * pw * (pw - 1);
            if (pw >= 1) Lc[pw - 1] += b * pw;
            Lc[pw] += c;
            for (int i = 0; i < nu; i++) M[i * nu + j] = Lc[i];
        }
        for (int i = 0; i < nu; i++) rhs[i] = P[i];
        if (!lsolve(M, rhs, nu)) fail("Couldn't solve for PI");
        for (int j = 0; j < nu; j++) pc[s + j] = rhs[j];
        pdeg = s + n;
        outf("Substitute and compare coefficients:");
        strcpy(pi, fpoly(pc, pdeg, "x"));
    } else if (ft == 2) {
        double ak = a * k * k + b * k + c;
        if (fabs(ak) > 1e-12) { s = 0; lam = p / ak; outf("Try y = " S_LA "%s", fexp(k, "x")); outf(S_LA "(%s) = %s", fx(ak), fx(p)); }
        else if (fabs(2 * a * k + b) > 1e-12) { s = 1; lam = p / (2 * a * k + b); outf("k is a root of the aux eq: try y = " S_LA "x%s", fexp(k, "x")); outf(S_LA "(%s) = %s", fx(2 * a * k + b), fx(p)); }
        else { s = 2; lam = p / (2 * a); outf("k is a repeated root: try y = " S_LA "x" S_SQ "%s", fexp(k, "x")); outf(S_LA "(%s) = %s", fx(2 * a), fx(p)); }
        outf(S_LA " = %s", fx(lam));
        strcpy(pi, fcoef(lam, s == 0 ? "" : s == 1 ? "x" : "x" S_SQ, 1));
        strcat(pi, fexp(k, "x"));
    } else if (ft == 3) {
        if (fabs(b) < 1e-12 && fabs(c - a * k * k) < 1e-12) {
            s = 1;
            outf("Resonance (ki is a root): try y = x(" S_LA "cos kx + " S_MU "sin kx)");
            mu = p / (2 * a * k); lam = -q / (2 * a * k);
        } else {
            double A_ = c - a * k * k, B_ = b * k;
            outf("Try y = " S_LA "%s + " S_MU "%s", ftrig("cos", k, "x"), ftrig("sin", k, "x"));
            outf("cos: %s" S_LA " + %s" S_MU " = %s", fx(A_), fxp(B_), fx(p));
            outf("sin: %s" S_LA " + %s" S_MU " = %s", fxp(-B_), fx(A_), fx(q));
            double det = A_ * A_ + B_ * B_;
            lam = (A_ * p - B_ * q) / det; mu = (B_ * p + A_ * q) / det;
        }
        outf(S_LA " = %s, " S_MU " = %s", fx(lam), fx(mu));
        char inner[120] = "";
        strcat(inner, fabs(lam) > 1e-12 ? fcoef(lam, ftrig("cos", k, "x"), 1) : "");
        if (fabs(mu) > 1e-12) strcat(inner, fcoef(mu, ftrig("sin", k, "x"), fabs(lam) < 1e-12));
        if (s) { strcpy(pi, "x("); strcat(pi, inner); strcat(pi, ")"); } else strcpy(pi, inner);
    }
    if (ft) outf(ANS "PI: y = %s", pi);
    if (ft) outf(ANS "GS: y = %s %s %s", cf.str, pi[0] == '-' ? "-" : "+", pi[0] == '-' ? pi + 1 : pi);
    else outf(ANS "GS: y = %s", cf.str);
    static const char *ic[] = {"No initial conditions", "Given y and y' at a point", "Given y at two points"};
    int ick = in_choice("Find A and B?", ic, 3);
    if (ick == 0) return;
    double x0, y0, x1 = 0, y1v = 0, dy0 = 0;
    x0 = in_real("x0");
    y0 = in_real("y(x0)");
    if (ick == 1) dy0 = in_real("y'(x0)");
    else { x1 = in_real("x1"); y1v = in_real("y(x1)"); }
    /* PI value and derivative at point */
    double piv[2], pid[2];
    double xs[2] = {x0, ick == 1 ? x0 : x1};
    for (int t = 0; t < 2; t++) {
        double x = xs[t];
        double v = 0, dv = 0;
        if (ft == 1) { v = peval_r(pc, pdeg, x); for (int i = 1; i <= pdeg; i++) dv += i * pc[i] * pow(x, i - 1); }
        if (ft == 2) {
            double e = exp(k * x), xs_ = s == 0 ? 1 : s == 1 ? x : x * x, dxs = s == 0 ? 0 : s == 1 ? 1 : 2 * x;
            v = lam * xs_ * e; dv = lam * (dxs * e + xs_ * k * e);
        }
        if (ft == 3) {
            double co = cos(k * x), si = sin(k * x);
            double g = lam * co + mu * si, dg = -lam * k * si + mu * k * co;
            if (s) { v = x * g; dv = g + x * dg; } else { v = g; dv = dg; }
        }
        piv[t] = v; pid[t] = dv;
    }
    double u1, du1, u2, du2;
    double M[4], r[2];
    cf_basis(&cf, x0, &u1, &du1, &u2, &du2);
    M[0] = u1; M[1] = u2; r[0] = y0 - piv[0];
    if (ick == 1) { M[2] = du1; M[3] = du2; r[1] = dy0 - pid[0]; }
    else { cf_basis(&cf, x1, &u1, &du1, &u2, &du2); M[2] = u1; M[3] = u2; r[1] = y1v - piv[1]; }
    outf("Substitute the conditions into y%s:", ick == 1 ? " and y'" : "");
    outf("%s%s = %s", fcoef(M[0], "A", 1), fcoef(M[1], "B", 0), fx(r[0]));
    outf("%s%s = %s", fcoef(M[2], "A", 1), fcoef(M[3], "B", 0), fx(r[1]));
    if (!lsolve(M, r, 2)) fail("Conditions don't determine A and B");
    outf(ANS "A = %s, B = %s", fx(r[0]), fx(r[1]));
}

static const char *ifactor(int t, double k, double a, double b) {
    char *o = sbuf();
    switch (t) {
    case 0: strcpy(o, fexp(k, "x")); break;
    case 1: strcpy(o, "x^"); strcat(o, fxp(k)); break;
    case 2: strcpy(o, "("); strcat(o, fpoly((double[]){b, a}, 1, "x")); strcat(o, ")^"); strcat(o, fxp(k / a)); break;
    case 3: strcpy(o, "sec^"); strcat(o, fxp(k)); strcat(o, " x"); break;
    case 4: strcpy(o, "sin^"); strcat(o, fxp(k)); strcat(o, " x"); break;
    case 5: strcpy(o, fexp(k / 2, "x" S_SQ)); break;
    }
    return o;
}

static void de_first(void) {
    in_note("dy/dx + P(x)y = Q(x)");
    static const char *o[] = {"P = k", "P = k/x", "P = k/(ax + b)", "P = k tan x", "P = k cot x", "P = kx"};
    int t = in_choice("P(x)", o, 6);
    double k = in_real("k"), a = 1, b = 0;
    if (t == 2) { a = in_real("a"); b = in_real("b"); }
    const char *ints[] = {"kx", "k ln x", "(k/a) ln(ax + b)", "k ln(sec x)", "k ln(sin x)", "kx" S_SQ "/2"};
    outf("IF = e^(" S_INT "P dx), " S_INT "P dx = %s", ints[t]);
    const char *IF = ifactor(t, k, a, b);
    outf(ANS "IF = %s", IF);
    outf("Multiply through: d/dx[%s " S_DOT " y] = %s " S_DOT " Q(x)", IF, IF);
    outf(ANS "%s " S_DOT " y = " S_INT " %s Q(x) dx", IF, IF);
    static const char *yn[] = {"No", "Yes, check a value numerically"};
    if (in_choice("Numerical check?", yn, 2) == 0) return;
    node *Q = in_fn("Q(x)", 'x');
    double x0 = in_real("x0"), y0 = in_real("y(x0)"), x1 = in_real("Find y at x =");
    /* RK4 on y' = Q - P y */
    int N = 2000;
    double h = (x1 - x0) / N, x = x0, y = y0;
    for (int i = 0; i < N; i++) {
        double kk[4];
        for (int j = 0; j < 4; j++) {
            double xx = x + (j == 0 ? 0 : j == 3 ? h : h / 2);
            double yy = y + (j == 0 ? 0 : j == 3 ? h * kk[2] : h / 2 * kk[j - 1]);
            double Pv;
            switch (t) {
            case 0: Pv = k; break;
            case 1: Pv = k / xx; break;
            case 2: Pv = k / (a * xx + b); break;
            case 3: Pv = k * tan(xx); break;
            case 4: Pv = k / tan(xx); break;
            default: Pv = k * xx;
            }
            kk[j] = ex_evalr(Q, 'x', xx) - Pv * yy;
        }
        y += h / 6 * (kk[0] + 2 * kk[1] + 2 * kk[2] + kk[3]);
        x += h;
    }
    outf(ANS "y(%s) " S_APX " %s", fx(x1), fdn(y, 8));
    outf("Use this to check your exact answer");
}

static void de_numeric(void) {
    in_note("dy/dx = f(x, y), solved numerically (RK4)");
    node *f = in_fn("f(x, y)", 'x');
    double x0 = in_real("x0"), y0 = in_real("y(x0)"), x1 = in_real("Find y at x =");
    int N = 2000;
    double h = (x1 - x0) / N, x = x0, y = y0;
    for (int i = 0; i < N; i++) {
        double k1, k2, k3, k4;
        ex_setvar('x', R(x)); ex_setvar('y', R(y)); k1 = ex_eval(f).re;
        ex_setvar('x', R(x + h / 2)); ex_setvar('y', R(y + h / 2 * k1)); k2 = ex_eval(f).re;
        ex_setvar('y', R(y + h / 2 * k2)); k3 = ex_eval(f).re;
        ex_setvar('x', R(x + h)); ex_setvar('y', R(y + h * k3)); k4 = ex_eval(f).re;
        y += h / 6 * (k1 + 2 * k2 + 2 * k3 + k4);
        x += h;
        if (N >= 10 && (i + 1) % (N / 4) == 0 && i + 1 < N) outf("x = %s: y " S_APX " %s", fd(x), fd(y));
    }
    outf(ANS "y(%s) " S_APX " %s", fx(x1), fdn(y, 8));
}

static void de_coupled(void) {
    in_note("dx/dt = ax + by,  dy/dt = cx + dy");
    double a = in_real("a"), b = in_real("b"), c = in_real("c"), d = in_real("d");
    int swap = 0;
    if (fabs(b) < 1e-12) {
        if (fabs(c) < 1e-12) {
            outf("Equations are independent:");
            outf(ANS "x = A%s,  y = B%s", fexp(a, "t"), fexp(d, "t"));
            return;
        }
        swap = 1;
        double t = a; a = d; d = t; t = b; b = c; c = t;
        outf("b = 0, so eliminate x instead (roles swapped)");
    }
    const char *X = swap ? "y" : "x", *Y = swap ? "x" : "y";
    outf("Differentiate the %s equation:", X);
    { char t1[8], t2[8]; strcpy(t1, X); strcat(t1, "'"); strcpy(t2, Y); strcat(t2, "'");
      outf("%s'' = %s%s", X, fcoef(a, t1, 1), fcoef(b, t2, 0));
      outf("Substitute %s' and %s = (%s%s)/%s:", Y, Y, t1, fcoef(-a, X, 0), fx(b)); }
    double B_ = -(a + d), C_ = a * d - b * c;
    { char t1[8]; strcpy(t1, X); strcat(t1, "'"); outf(ANS "%s''%s%s = 0", X, fcoef(B_, t1, 0), fcoef(C_, X, 0)); }
    cfun cf = aux_solve(1, B_, C_, "t", X);
    outf("then %s = (%s' %s)/%s", Y, X, fcoef(-a, X, 0) + 1, fx(b));
    if (cf.type == 0) {
        outf(ANS "%s = A%s + B%s", X, fexp(cf.m1, "t"), fexp(cf.m2, "t"));
        { char e1[40], e2[40]; strcpy(e1, "A"); strcat(e1, fexp(cf.m1, "t")); strcpy(e2, "B"); strcat(e2, fexp(cf.m2, "t"));
          outf(ANS "%s = %s%s", Y, fcoef((cf.m1 - a) / b, e1, 1), fcoef((cf.m2 - a) / b, e2, 0)); }
    } else if (cf.type == 1) {
        outf(ANS "%s = (A + Bt)%s", X, fexp(cf.m1, "t"));
        outf(ANS "%s = [%sA %sB %sBt]%s / 1", Y, fx((cf.m1 - a) / b), fcoef(1 / b, "", 0), fcoef((cf.m1 - a) / b, "", 0), fexp(cf.m1, "t"));
    } else {
        outf(ANS "%s = %s(A cos %st + B sin %st)", X, fexp(cf.p, "t"), fx(cf.q), fx(cf.q));
        outf("%s' = %s[(%sA + %sB)cos + (%sB - %sA)sin]", X, fexp(cf.p, "t"), fx(cf.p), fx(cf.q), fx(cf.p), fx(cf.q));
        outf(ANS "%s = %s[(%sA %sB)cos %st", Y, fexp(cf.p, "t"), fx((cf.p - a) / b), fcoef(cf.q / b, "", 0), fx(cf.q));
        outf(ANS "   + (%sB %sA)sin %st]", fx((cf.p - a) / b), fcoef(-cf.q / b, "", 0), fx(cf.q));
    }
    static const char *ic[] = {"No", "Yes"};
    if (in_choice("Use initial values at t = 0?", ic, 2) == 0) return;
    double x0 = in_real(swap ? "y(0)" : "x(0)"), y0 = in_real(swap ? "x(0)" : "y(0)");
    double u1, du1, u2, du2;
    cf_basis(&cf, 0, &u1, &du1, &u2, &du2);
    /* X(0) = A u1 + B u2 ; Y(0) = (X'(0) - a X(0))/b */
    double M[4] = {u1, u2, (du1 - a * u1) / b, (du2 - a * u2) / b}, r[2] = {x0, y0};
    if (!lsolve(M, r, 2)) fail("Can't solve for A, B");
    outf(ANS "A = %s, B = %s", fx(r[0]), fx(r[1]));
}

static const calc_t de_c[] = {
    {"2nd order: ay''+by'+cy = f(x)", de_second},
    {"1st order: integrating factor", de_first},
    {"Coupled first order system", de_coupled},
    {"dy/dx = f(x,y) numerically", de_numeric},
};

const topic_t fm_u4[] = {
    {"Complex: de Moivre & roots", demoivre_c, NEL(demoivre_c)},
    {"Further trigonometry", trig_c, NEL(trig_c)},
    {"3" S_TIMES "3 matrices & equations", mat3_c, NEL(mat3_c)},
    {"Maclaurin & partial fractions", series_c, NEL(series_c)},
    {"Calculus & integration", calc_c, NEL(calc_c)},
    {"Polar coordinates", polar_c, NEL(polar_c)},
    {"Hyperbolic functions", hyp_c, NEL(hyp_c)},
    {"Differential equations", de_c, NEL(de_c)},
};
const int fm_u4_n = NEL(fm_u4);
