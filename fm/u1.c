/* Further Maths Unit 1: Further Pure A */
#include "../common/core.h"
#include "../common/util.h"
#include <string.h>

/* ======================= COMPLEX NUMBERS ======================= */
void c_arith(void) {
    cx a = in_cx("z1"), b = in_cx("z2");
    outf(HEAD "Sum and difference");
    outf("z1 + z2 = %s", fc(cadd(a, b)));
    outf("z1 - z2 = %s", fc(csub(a, b)));
    outf(HEAD "Product");
    outf("(%s)(%s)", fc(a), fc(b));
    outf("= %s%s%s%s", fx(a.re * b.re), fcoef(a.re * b.im, "i", 0), fcoef(a.im * b.re, "i", 0), fcoef(a.im * b.im, "i" S_SQ, 0));
    outf("i" S_SQ " = -1, so");
    outf(ANS "z1z2 = %s", fc(cmul(a, b)));
    outf(HEAD "Quotient");
    if (b.re == 0 && b.im == 0) outf("z2 = 0, can't divide");
    else {
        cx num = cmul(a, cconj(b));
        double den = b.re * b.re + b.im * b.im;
        outf("Multiply top and bottom by z2* = %s", fc(cconj(b)));
        outf("Top: %s", fc(num));
        outf("Bottom: |z2|" S_SQ " = %s", fx(den));
        outf(ANS "z1/z2 = %s", fc(cdiv(a, b)));
    }
    outf(HEAD "Other");
    outf("z1* = %s,  z2* = %s", fc(cconj(a)), fc(cconj(b)));
    outf("|z1| = %s,  |z2| = %s", fxd(cabs_(a)), fxd(cabs_(b)));
    outf("arg z1 = %s", fang(carg_(a)));
    outf("arg z2 = %s", fang(carg_(b)));
}

static void c_modarg_work(cx z) {
    double r = cabs_(z), t = carg_(z);
    outf("r = " S_SQRT "(%s" S_SQ " + %s" S_SQ ") = %s", fxp(z.re), fxp(z.im), fxd(r));
    if (z.re != 0 && z.im != 0) {
        double base = atan(fabs(z.im / z.re));
        outf("Base angle = tan^-1(|b/a|) = tan^-1(%s) = %s", fx(fabs(z.im / z.re)), fang(base));
        const char *q = z.re > 0 ? (z.im > 0 ? "1st" : "4th") : (z.im > 0 ? "2nd" : "3rd");
        outf("z is in the %s quadrant", q);
        if (z.re < 0 && z.im > 0) outf(S_TH " = " S_PI " - base");
        else if (z.re < 0 && z.im < 0) outf(S_TH " = -(" S_PI " - base)");
        else if (z.im < 0) outf(S_TH " = -base");
    }
    outf(ANS "arg z = %s", fang(t));
    outf(ANS "z = %s(cos %s + i sin %s)", fx(r), fx(t), fx(t));
    outf("  = %se^(i%s)", fx(r), fxp(t));
}

static void c_tomodarg(void) {
    cx z = in_cx("z (a + bi)");
    if (z.re == 0 && z.im == 0) fail("arg 0 is undefined");
    c_modarg_work(z);
}

static void c_fromodarg(void) {
    double r = in_real("Modulus r");
    double t = in_real("Argument " S_TH " (rad)");
    outf("z = r cos" S_TH " + i r sin" S_TH);
    outf("Re = %s cos(%s) = %s", fx(r), fx(t), fx(r * cos(t)));
    outf("Im = %s sin(%s) = %s", fx(r), fx(t), fx(r * sin(t)));
    outf(ANS "z = %s", fc(cpolar(r, t)));
}

void c_quad(void) {
    in_note("az" S_SQ " + bz + c = 0 (complex coefficients ok)");
    cx a = in_cx("a"), b = in_cx("b"), c = in_cx("c");
    if (a.re == 0 && a.im == 0) fail("a can't be 0");
    cx disc = csub(cmul(b, b), cmul(R(4), cmul(a, c)));
    outf("z = (-b " S_PM " " S_SQRT "(b" S_SQ " - 4ac)) / 2a");
    outf("b" S_SQ " - 4ac = %s", fc(disc));
    if (disc.im == 0 && disc.re < 0) outf(S_SQRT "(%s) = %si", fx(disc.re), fxd(sqrt(-disc.re)));
    else outf(S_SQRT "(b" S_SQ "-4ac) = %s", fc(csqrt_(disc)));
    cx s = csqrt_(disc);
    cx r1 = cdiv(cadd(cneg(b), s), cmul(R(2), a)), r2 = cdiv(csub(cneg(b), s), cmul(R(2), a));
    cx rr[2] = {r1, r2};
    clean_roots(rr, 2);
    outf(ANS "z = %s", fc(rr[0]));
    outf(ANS "z = %s", fc(rr[1]));
}

static void out_roots_factored(const double *a, int deg) {
    cx r[8];
    int n = proots_r(a, deg, r);
    outf(HEAD "Roots");
    for (int i = 0; i < n; i++) outf(ANS "z = %s", fc(r[i]));
    outf(HEAD "Factors");
    for (int i = 0; i < n; i++) {
        if (r[i].im == 0) outf("(z - %s)", fx(r[i].re));
        else if (r[i].im > 0) {
            double q[3] = {r[i].re * r[i].re + r[i].im * r[i].im, -2 * r[i].re, 1};
            outf("(z - %s)(z - %s)", fcp(r[i]), fcp(cconj(r[i])));
            outf("  = %s", fpoly(q, 2, "z"));
        }
    }
}

static void c_poly(void) {
    static const char *o[] = {"Cubic", "Quartic"};
    int k = in_choice("Degree", o, 2);
    int deg = 3 + k;
    double a[5];
    in_note(deg == 3 ? "az" S_CU " + bz" S_SQ " + cz + d = 0" : "az^4 + bz" S_CU " + cz" S_SQ " + dz + e = 0");
    const char *lab[] = {"a", "b", "c", "d", "e"};
    for (int i = 0; i <= deg; i++) a[deg - i] = in_real(lab[i]);
    if (a[deg] == 0) fail("Leading coefficient can't be 0");
    outf("Solving %s = 0", fpoly(a, deg, "z"));
    /* look for an obvious real root first for working */
    for (int t = -10; t <= 10; t++) {
        if (fabs(peval_r(a, deg, t)) < 1e-9) {
            outf("Factor theorem: f(%d) = 0, so (z - %d) is a factor", t, t);
            double b[2] = {-t, 1}, q[5];
            pdiv(a, deg, b, 1, q, NULL);
            outf("f(z) = (z - %d)(%s)", t, fpoly(q, deg - 1, "z"));
            break;
        }
    }
    out_roots_factored(a, deg);
}

static void c_givenroot(void) {
    static const char *o[] = {"Cubic", "Quartic"};
    int k = in_choice("Degree", o, 2);
    int deg = 3 + k;
    double a[5];
    in_note("Real coefficients. Enter the known complex root too.");
    const char *lab[] = {"a (z^n coeff)", "b", "c", "d", "e"};
    for (int i = 0; i <= deg; i++) a[deg - i] = in_real(lab[i]);
    cx r = in_cx("Known root");
    if (a[deg] == 0) fail("Leading coefficient can't be 0");
    cx v = R(0);
    for (int i = deg; i >= 0; i--) v = cadd(cmul(v, r), R(a[i]));
    if (cabs_(v) > 1e-6 * (1 + fabs(a[deg]))) outf(ANS "Warning: f(root) = %s, not 0", fc(v));
    outf("f(z) = %s", fpoly(a, deg, "z"));
    double qf[3];
    int dq;
    if (fabs(r.im) > 1e-12) {
        outf("Real coefficients, so the conjugate %s is also a root", fc(cconj(r)));
        qf[0] = r.re * r.re + r.im * r.im; qf[1] = -2 * r.re; qf[2] = 1;
        outf("Sum of pair = %s, product = %s", fx(2 * r.re), fx(qf[0]));
        outf("Quadratic factor: %s", fpoly(qf, 2, "z"));
        dq = 2;
    } else {
        qf[0] = -r.re; qf[1] = 1;
        outf("Factor: (z - %s)", fx(r.re));
        dq = 1;
    }
    double q[5], rem[3];
    pdiv(a, deg, qf, dq, q, rem);
    outf("Divide: f(z) = (%s)(%s)", fpoly(qf, dq, "z"), fpoly(q, deg - dq, "z"));
    out_roots_factored(a, deg);
}

static void c_loci(void) {
    static const char *o[] = {"|z - a| = r", "|z - a| = |z - b|", "|z - a| = k|z - b|", "arg(z - a) = " S_TH};
    int k = in_choice("Locus type", o, 4);
    if (k == 0) {
        cx a = in_cx("a");
        double r = in_pos("r");
        outf("Circle, centre (%s, %s), radius %s", fx(a.re), fx(a.im), fx(r));
        outf(ANS "%s" S_SQ " + %s" S_SQ " = %s", fshift("x", a.re), fshift("y", a.im), fx(r * r));
        outf("Let z = x + iy: |(x - %s) + i(y - %s)| = %s", fx(a.re), fx(a.im), fx(r));
        outf("|z-a| < r: inside, |z-a| > r: outside");
    } else if (k == 1) {
        cx a = in_cx("a"), b = in_cx("b");
        outf("Perpendicular bisector of (%s, %s) and (%s, %s)", fx(a.re), fx(a.im), fx(b.re), fx(b.im));
        outf("%s" S_SQ " + %s" S_SQ " = %s" S_SQ " + %s" S_SQ, fshift("x", a.re), fshift("y", a.im), fshift("x", b.re), fshift("y", b.im));
        double A = 2 * (b.re - a.re), B = 2 * (b.im - a.im), Cc = b.re * b.re + b.im * b.im - a.re * a.re - a.im * a.im;
        double v[3] = {Cc, A, B};
        outf("Expand and cancel x" S_SQ ", y" S_SQ ":");
        outf(ANS "%s%s = %s", fcoef(A, "x", 1), fabs(B) > 1e-12 ? fcoef(B, "y", fabs(A) < 1e-12) : "", fx(Cc));
        if (fabs(B) > 1e-12) outf(ANS "y = %sx%s", fx(-A / B), fcoef(Cc / B, "", 0));
        (void)v;
    } else if (k == 2) {
        cx a = in_cx("a"), b = in_cx("b");
        double kk = in_pos("k");
        if (fabs(kk - 1) < 1e-12) fail("k = 1 is a perpendicular bisector (use that option)");
        double k2 = kk * kk;
        outf("%s" S_SQ " + %s" S_SQ " = %s[%s" S_SQ " + %s" S_SQ "]", fshift("x", a.re), fshift("y", a.im), fx(k2), fshift("x", b.re), fshift("y", b.im));
        double A = 1 - k2;
        double D = -2 * (a.re - k2 * b.re), E = -2 * (a.im - k2 * b.im);
        double F = a.re * a.re + a.im * a.im - k2 * (b.re * b.re + b.im * b.im);
        outf("%s%s%s%s%s = 0", fcoef(A, "x" S_SQ, 1), fcoef(A, "y" S_SQ, 0), fcoef(D, "x", 0), fcoef(E, "y", 0), fcoef(F, "", 0));
        D /= A; E /= A; F /= A;
        outf("Divide by %s: x" S_SQ " + y" S_SQ "%s%s%s = 0", fx(A), fcoef(D, "x", 0), fcoef(E, "y", 0), fcoef(F, "", 0));
        double cx_ = -D / 2, cy = -E / 2, r2 = cx_ * cx_ + cy * cy - F;
        outf("Complete the square:");
        outf(ANS "%s" S_SQ " + %s" S_SQ " = %s", fshift("x", cx_), fshift("y", cy), fx(r2));
        outf(ANS "Circle centre (%s, %s), radius %s", fx(cx_), fx(cy), fxd(sqrt(r2)));
    } else {
        cx a = in_cx("a");
        double t = in_real(S_TH " (rad)");
        outf("Half-line from (%s, %s) at angle %s", fx(a.re), fx(a.im), fang(t));
        outf("(the point a itself is not included)");
        if (fabs(cos(t)) < 1e-12) outf(ANS "x = %s, y %s %s", fx(a.re), sin(t) > 0 ? ">" : "<", fx(a.im));
        else {
            double m = tan(t);
            outf(ANS "y - %s = %s(x - %s)", fx(a.im), fx(m), fx(a.re));
            outf(ANS "with x %s %s", cos(t) > 0 ? ">" : "<", fx(a.re));
            outf("y = %sx%s", fx(m), fcoef(a.im - m * a.re, "", 0));
        }
    }
}

/* circle through 3 complex points; returns 0 if collinear */
static int circ3(cx p1, cx p2, cx p3, cx *c, double *r) {
    double ax = p1.re, ay = p1.im, bx = p2.re, by = p2.im, cxx = p3.re, cy = p3.im;
    double d = 2 * (ax * (by - cy) + bx * (cy - ay) + cxx * (ay - by));
    if (fabs(d) < 1e-9 * (1 + fabs(ax) + fabs(bx) + fabs(cxx) + fabs(ay) + fabs(by) + fabs(cy))) return 0;
    double a2 = ax * ax + ay * ay, b2 = bx * bx + by * by, c2 = cxx * cxx + cy * cy;
    c->re = (a2 * (by - cy) + b2 * (cy - ay) + c2 * (ay - by)) / d;
    c->im = (a2 * (cxx - bx) + b2 * (ax - cxx) + c2 * (bx - ax)) / d;
    *r = cabs_(csub(p1, *c));
    return 1;
}

static void c_transform(void) {
    static const char *o[] = {"Image of a point, any w = f(z)", "Image of circle / line, w = (az+b)/(cz+d)"};
    int k = in_choice("Type", o, 2);
    if (k == 0) {
        node *f = in_fn("w = f(z)", 'z');
        cx z = in_cx("Point z");
        ex_setvar('z', z);
        cx w = ex_eval(f);
        outf("w = %s with z = %s", ex_src(f), fc(z));
        outf(ANS "w = %s", fc(w));
        outf("u = %s, v = %s", fx(w.re), fx(w.im));
        return;
    }
    in_note("w = (az + b)/(cz + d)");
    cx a = in_cx("a"), b = in_cx("b"), c = in_cx("c"), d = in_cx("d");
    static const char *lo[] = {"Circle |z - z0| = r", "Line through 2 points", "Line ax + by = c"};
    int L = in_choice("Locus of z", lo, 3);
    cx base, dir;
    int circle = 0;
    double rad = 0;
    if (L == 0) {
        base = in_cx("z0");
        rad = in_pos("r");
        circle = 1;
        dir = R(0);
    } else if (L == 1) {
        base = in_cx("Point 1");
        dir = csub(in_cx("Point 2"), base);
        if (cabs_(dir) < 1e-12) fail("Points must be different");
    } else {
        double v[3];
        in_vec("a, b, c", v, 3);
        if (fabs(v[0]) < 1e-12 && fabs(v[1]) < 1e-12) fail("Not a line");
        base = fabs(v[1]) > 1e-12 ? C(0, v[2] / v[1]) : C(v[2] / v[0], 0);
        dir = C(-v[1], v[0]);
    }
    outf("Map 3 points of the locus (avoiding cz + d = 0):");
    static const double ts[] = {0, M_PI / 2, M_PI, 3 * M_PI / 2, M_PI / 4, 3 * M_PI / 4, 5 * M_PI / 4};
    static const double tl[] = {0, 1, -1, 2, -2, 3, 0.5};
    cx W[3];
    int np = 0;
    for (int i = 0; i < 7 && np < 3; i++) {
        cx z = circle ? cadd(base, cpolar(rad, ts[i])) : cadd(base, cmul(R(tl[i]), dir));
        cx den = cadd(cmul(c, z), d);
        if (cabs_(den) < 1e-6) { outf("  %s " S_ARR " " S_INF " (pole)", fc(z)); continue; }
        W[np] = cdiv(cadd(cmul(a, z), b), den);
        outf("  %s " S_ARR " %s", fc(z), fc(W[np]));
        np++;
    }
    cx cc;
    double rr;
    if (circ3(W[0], W[1], W[2], &cc, &rr)) {
        outf(ANS "Image: circle centre %s, radius %s", fc(cc), fxd(rr));
        outf(ANS "|w - %s| = %s", fcp(cc), fx(rr));
        outf("%s" S_SQ " + %s" S_SQ " = %s", fshift("u", cc.re), fshift("v", cc.im), fx(rr * rr));
    } else {
        cx dir = csub(W[1], W[0]);
        double A = dir.im, B = -dir.re, Cc = A * W[0].re + B * W[0].im;
        double s[3] = {Cc, A, B};
        double kk = fabs(A) > 1e-12 ? A : B;
        for (int i = 0; i < 3; i++) s[i] /= kk;
        pintscale(s, 2);
        if (fabs(B) > 1e-12 && s[2] * B < 0) { s[0] = -s[0]; s[1] = -s[1]; s[2] = -s[2]; }
        outf(ANS "Image: straight line");
        outf(ANS "%s%s = %s", fcoef(s[1], "u", 1), fcoef(s[2], "v", fabs(s[1]) < 1e-12), fx(s[0]));
    }
}

static const calc_t complex_c[] = {
    {"Arithmetic (+ - " S_TIMES " /)", c_arith},
    {"a + bi " S_ARR " modulus-argument", c_tomodarg},
    {"Modulus-argument " S_ARR " a + bi", c_fromodarg},
    {"Solve quadratic", c_quad},
    {"Solve cubic / quartic", c_poly},
    {"Cubic/quartic given a root", c_givenroot},
    {"Loci", c_loci},
    {"Transformations w = f(z)", c_transform},
};

/* ======================= MATRICES ======================= */
static int in_size(void) {
    static const char *o[] = {"2" S_TIMES "2", "3" S_TIMES "3"};
    return in_choice("Size", o, 2) + 2;
}

static void m_det_inv2(void) {
    double m[4];
    in_mat("M", m, 2, 2);
    double a = m[0], b = m[1], c = m[2], d = m[3];
    out_mat("M", m, 2, 2);
    double det = a * d - b * c;
    outf("det M = ad - bc = (%s)(%s) - (%s)(%s)", fx(a), fx(d), fx(b), fx(c));
    outf(ANS "det M = %s", fx(det));
    if (fabs(det) < 1e-12) { outf(ANS "Singular: no inverse"); return; }
    outf("M^-1 = (1/det)[d  -b; -c  a]");
    double adj[4] = {d, -b, -c, a};
    out_mat("adj", adj, 2, 2);
    double inv[4] = {d / det, -b / det, -c / det, a / det};
    outf(ANS "M^-1 =");
    out_mat(NULL, inv, 2, 2);
    outf("Area scale factor = |det| = %s", fx(fabs(det)));
}

static void m_mult(void) {
    int n = in_size();
    double A[9], B[9], P[9];
    in_note("For transformation A then B, the matrix is BA");
    in_mat("A", A, n, n);
    in_mat("B", B, n, n);
    matmul(A, B, P, n, n, n);
    outf(HEAD "AB");
    out_mat(NULL, P, n, n);
    matmul(B, A, P, n, n, n);
    outf(HEAD "BA  (A first, then B)");
    out_mat(NULL, P, n, n);
    double d1 = n == 2 ? A[0] * A[3] - A[1] * A[2] : det3((const double(*)[3])A);
    double d2 = n == 2 ? B[0] * B[3] - B[1] * B[2] : det3((const double(*)[3])B);
    outf("det A = %s, det B = %s, det(AB) = %s", fx(d1), fx(d2), fx(d1 * d2));
}

static void m_identify(void) {
    double m[4];
    in_mat("M", m, 2, 2);
    double a = m[0], b = m[1], c = m[2], d = m[3];
    double det = a * d - b * c;
    out_mat("M", m, 2, 2);
    outf("det = %s", fx(det));
    outf("Image of i: (%s, %s)   Image of j: (%s, %s)", fx(a), fx(c), fx(b), fx(d));
    double e = 1e-9;
    if (fabs(b) < e && fabs(c) < e) {
        if (fabs(a - d) < e) {
            if (fabs(a - 1) < e) outf(ANS "Identity");
            else if (fabs(a + 1) < e) outf(ANS "Rotation 180" S_DEG " about O (enlargement -1)");
            else outf(ANS "Enlargement, scale factor %s, centre O", fx(a));
        } else if (fabs(a - 1) < e && fabs(d + 1) < e) outf(ANS "Reflection in the x-axis");
        else if (fabs(a + 1) < e && fabs(d - 1) < e) outf(ANS "Reflection in the y-axis");
        else {
            outf(ANS "Stretch: factor %s parallel to x-axis", fx(a));
            outf(ANS "and factor %s parallel to y-axis", fx(d));
        }
    } else if (fabs(a - d) < e && fabs(b + c) < e) {
        double k = sqrt(a * a + c * c), t = atan2(c, a);
        if (fabs(k - 1) < e) outf(ANS "Rotation %s anticlockwise about O", fang(t));
        else {
            outf(ANS "Rotation %s anticlockwise about O", fang(t));
            outf(ANS "combined with enlargement scale factor %s", fxd(k));
        }
        outf("cos " S_TH " = %s, sin " S_TH " = %s", fx(a / k), fx(c / k));
    } else if (fabs(a + d) < e && fabs(b - c) < e && fabs(a * a + b * b - 1) < 1e-9) {
        double t = atan2(b, a);
        outf("Form [cos2" S_TH "  sin2" S_TH "; sin2" S_TH "  -cos2" S_TH "]");
        outf("2" S_TH " = %s, " S_TH " = %s", fang(t), fang(t / 2));
        if (fabs(cos(t / 2)) < e) outf(ANS "Reflection in the y-axis (x = 0)");
        else outf(ANS "Reflection in y = %s", fcoef(tan(t / 2), "x", 1));
    } else if (fabs(a - 1) < e && fabs(d - 1) < e && fabs(c) < e) {
        outf(ANS "Shear parallel to x-axis: (x, y) " S_ARR " (x + %sy, y)", fx(b));
        outf("x-axis is invariant (fixed points)");
    } else if (fabs(a - 1) < e && fabs(d - 1) < e && fabs(b) < e) {
        outf(ANS "Shear parallel to y-axis: (x, y) " S_ARR " (x, y + %sx)", fx(c));
        outf("y-axis is invariant (fixed points)");
    } else {
        outf(ANS "Not a single standard transformation");
        outf("Area scale factor %s%s", fx(fabs(det)), det < 0 ? ", orientation reversed" : "");
        outf("Try 'Invariant lines' to describe it");
    }
}

static void m_build(void) {
    static const char *o[] = {"2D rotation (anticlockwise)", "2D reflection in y = mx", "2D reflection in y = x tan" S_TH,
                              "2D enlargement", "2D stretch", "3D rotation about x-axis", "3D rotation about y-axis",
                              "3D rotation about z-axis", "3D reflection in plane"};
    int k = in_choice("Transformation", o, 9);
    double m[9];
    if (k == 0) {
        double t = in_real(S_TH " (rad)");
        double c = cos(t), s = sin(t);
        outf("[cos" S_TH "  -sin" S_TH "; sin" S_TH "  cos" S_TH "]");
        double r[4] = {c, -s, s, c};
        out_mat("M", r, 2, 2);
    } else if (k == 1 || k == 2) {
        double t;
        if (k == 1) { double mm = in_real("m"); t = atan(mm); outf(S_TH " = tan^-1(%s) = %s", fx(mm), fang(t)); }
        else t = in_real(S_TH " (rad)");
        outf("[cos2" S_TH "  sin2" S_TH "; sin2" S_TH "  -cos2" S_TH "]");
        double r[4] = {cos(2 * t), sin(2 * t), sin(2 * t), -cos(2 * t)};
        out_mat("M", r, 2, 2);
    } else if (k == 3) {
        double s = in_real("Scale factor");
        double r[4] = {s, 0, 0, s};
        out_mat("M", r, 2, 2);
    } else if (k == 4) {
        double p = in_real("Factor parallel to x-axis"), q = in_real("Factor parallel to y-axis");
        double r[4] = {p, 0, 0, q};
        out_mat("M", r, 2, 2);
    } else if (k <= 7) {
        double t = in_real(S_TH " (rad)");
        double c = cos(t), s = sin(t);
        double rx[9] = {1, 0, 0, 0, c, -s, 0, s, c};
        double ry[9] = {c, 0, s, 0, 1, 0, -s, 0, c};
        double rz[9] = {c, -s, 0, s, c, 0, 0, 0, 1};
        memcpy(m, k == 5 ? rx : k == 6 ? ry : rz, sizeof m);
        outf("Anticlockwise looking from the positive axis");
        out_mat("M", m, 3, 3);
    } else {
        static const char *pl[] = {"x = 0 (yz-plane)", "y = 0 (xz-plane)", "z = 0 (xy-plane)", "y = x", "z = x", "z = y"};
        int p = in_choice("Plane", pl, 6);
        double I[6][9] = {{-1, 0, 0, 0, 1, 0, 0, 0, 1}, {1, 0, 0, 0, -1, 0, 0, 0, 1}, {1, 0, 0, 0, 1, 0, 0, 0, -1},
                          {0, 1, 0, 1, 0, 0, 0, 0, 1}, {0, 0, 1, 0, 1, 0, 1, 0, 0}, {1, 0, 0, 0, 0, 1, 0, 1, 0}};
        out_mat("M", I[p], 3, 3);
    }
}

static void m_invpoints(void) {
    int n = in_size();
    double m[9];
    in_mat("M", m, n, n);
    outf("Invariant points satisfy Mx = x, i.e. (M - I)x = 0");
    double a[9];
    memcpy(a, m, sizeof a);
    for (int i = 0; i < n; i++) a[i * n + i] -= 1;
    out_mat("M - I", a, n, n);
    int rk = rank_n(a, n, n);
    if (rk == n) { outf("det(M - I) " S_NE " 0"); outf(ANS "Only invariant point: the origin"); return; }
    if (rk == 0) { outf(ANS "M = I: every point is invariant"); return; }
    if (n == 2) {
        int r = fabs(a[0]) + fabs(a[1]) > 1e-12 ? 0 : 2;
        outf("det(M - I) = 0, equations are multiples of:");
        outf("%s%s = 0", fcoef(a[r], "x", 1), fcoef(a[r + 1], "y", fabs(a[r]) < 1e-12));
        if (fabs(a[r + 1]) > 1e-12) outf(ANS "Line of invariant points: y = %sx", fx(-a[r] / a[r + 1]));
        else outf(ANS "Line of invariant points: x = 0");
        return;
    }
    /* 3x3 */
    if (rk == 2) {
        double d[3] = {0};
        for (int i = 0; i < 3 && norm3(d) < 1e-9; i++)
            for (int j = i + 1; j < 3 && norm3(d) < 1e-9; j++) cross3(a + 3 * i, a + 3 * j, d);
        double s[3];
        memcpy(s, d, sizeof s);
        if (pintscale(s, 2)) memcpy(d, s, sizeof d);
        outf("Rank 2: solutions form a line through O");
        outf("Direction = cross product of two rows");
        outf(ANS "Line of invariant points: r = " S_LA "%s", fvec(d, 3));
    } else {
        int r = 0;
        while (norm3(a + 3 * r) < 1e-12) r++;
        outf("Rank 1: every equation is a multiple of one");
        double z = 0;
        outf(ANS "Plane of invariant points: %s", fplane(a + 3 * r, z));
    }
}

static void m_invlines(void) {
    double m[4];
    in_mat("M", m, 2, 2);
    double a = m[0], b = m[1], c = m[2], d = m[3];
    out_mat("M", m, 2, 2);
    outf("Try y = mx + c: (x, mx + c) maps to");
    outf("X = %s%s,  Y = %s%s", fcoef(a, "x", 1), fcoef(b, "(mx + c)", 0), fcoef(c, "x", 1), fcoef(d, "(mx + c)", 0));
    outf("Need Y = mX + c for all x:");
    outf("x terms: %s%s = m(%s%s)", fx(c), fcoef(d, "m", 0), fx(a), fcoef(b, "m", 0));
    double q[3] = {-c, a - d, b};
    outf("%s = 0", fpoly(q, 2, "m"));
    int found = 0;
    if (fabs(b) < 1e-12) {
        outf("b = 0, so also check vertical lines x = k:");
        outf(ANS "x = 0 is invariant%s", fabs(a - 1) < 1e-12 ? "; so is every x = k" : "");
        if (fabs(a - d) > 1e-12) {
            double mm = c / (a - d);
            outf("Linear: m = %s", fx(mm));
            double t = d - b * mm - 1;
            if (fabs(t) < 1e-12) outf(ANS "y = %sx + c is invariant for all c", fx(mm));
            else outf(ANS "y = %sx (through O)", fx(mm));
            found = 1;
        } else if (fabs(c) < 1e-12) {
            outf(ANS "Every line through O is invariant"); found = 1;
        }
    } else {
        double disc = (a - d) * (a - d) + 4 * b * c;
        outf("Discriminant = %s", fx(disc));
        if (disc < -1e-12) { outf(ANS "No real m: no invariant lines (except none)"); return; }
        double ms[2] = {(-(a - d) + sqrt(fmax(disc, 0))) / (2 * b), (-(a - d) - sqrt(fmax(disc, 0))) / (2 * b)};
        int nm = disc < 1e-12 ? 1 : 2;
        for (int i = 0; i < nm; i++) {
            double mm = ms[i];
            outf("m = %s:", fx(mm));
            double t = d - b * mm - 1;
            outf("  constant terms: c(d - bm - 1) = c(%s) = 0", fx(t));
            if (fabs(t) < 1e-9) outf(ANS "y = %s + c is invariant for ALL c", fcoef(mm, "x", 1));
            else outf(ANS "y = %s (c = 0 only)", fcoef(mm, "x", 1));
            found = 1;
        }
    }
    if (!found) outf(ANS "No invariant lines");
    outf("Check 'Invariant points' for lines of fixed points");
}

static void m_image(void) {
    static const char *o[] = {"2D point", "2D line y = mx + c", "3D point"};
    int k = in_choice("Image of", o, 3);
    if (k == 0 || k == 1) {
        double m[4];
        in_mat("M", m, 2, 2);
        if (k == 0) {
            double p[2];
            in_vec("Point (x, y)", p, 2);
            double r[2] = {m[0] * p[0] + m[1] * p[1], m[2] * p[0] + m[3] * p[1]};
            outf("(%s)(%s) + (%s)(%s) = %s", fx(m[0]), fx(p[0]), fx(m[1]), fx(p[1]), fx(r[0]));
            outf("(%s)(%s) + (%s)(%s) = %s", fx(m[2]), fx(p[0]), fx(m[3]), fx(p[1]), fx(r[1]));
            outf(ANS "Image = %s", fvec(r, 2));
            return;
        }
        double mm = in_real("m"), c = in_real("c");
        outf("General point (t, %st%s) maps to", fx(mm), fcoef(c, "", 0));
        double X1 = m[0] + m[1] * mm, X0 = m[1] * c, Y1 = m[2] + m[3] * mm, Y0 = m[3] * c;
        outf("X = %s,  Y = %s", fpoly((double[]){X0, X1}, 1, "t"), fpoly((double[]){Y0, Y1}, 1, "t"));
        if (fabs(X1) < 1e-12 && fabs(Y1) < 1e-12) { outf(ANS "Line collapses to the point (%s, %s)", fx(X0), fx(Y0)); return; }
        if (fabs(X1) < 1e-12) { outf(ANS "Image: x = %s", fx(X0)); return; }
        outf("Eliminate t = (X - %s)/%s", fx(X0), fx(X1));
        double M2 = Y1 / X1, C2 = Y0 - M2 * X0;
        outf(ANS "Image: y = %sx%s", fx(M2), fcoef(C2, "", 0));
    } else {
        double m[9], p[3], r[3];
        in_mat("M", m, 3, 3);
        in_vec("Point", p, 3);
        matmul(m, p, r, 3, 3, 1);
        outf(ANS "Image = %s", fvec(r, 3));
    }
}

static void m_power(void) {
    int n = in_size();
    double m[9], p[9];
    in_mat("M", m, n, n);
    int k = in_int("Power n", 1, 50);
    memcpy(p, m, sizeof p);
    outf("Use the pattern for proof by induction:");
    for (int i = 1; i <= k; i++) {
        if (i > 1) matmul(p, m, p, n, n, n);
        if (i <= 4 || i == k) {
            char nm[12] = "M^";
            strcat(nm, fd(i));
            out_mat(nm, p, n, n);
        } else if (i == 5) outf("...");
    }
}

static const calc_t matrix_c[] = {
    {"2" S_TIMES "2 determinant & inverse", m_det_inv2},
    {"Multiply / successive transformations", m_mult},
    {"Identify a 2" S_TIMES "2 transformation", m_identify},
    {"Matrix for a transformation", m_build},
    {"Invariant points", m_invpoints},
    {"Invariant lines (2" S_TIMES "2)", m_invlines},
    {"Image of point / line", m_image},
    {"Powers M^n (induction)", m_power},
};

/* ======================= ROOTS & COEFFICIENTS ======================= */
static int read_poly(double *a) {
    static const char *o[] = {"Quadratic", "Cubic", "Quartic"};
    int deg = in_choice("Degree", o, 3) + 2;
    const char *lab[] = {"a", "b", "c", "d", "e"};
    if (deg == 2) in_note("az" S_SQ " + bz + c = 0");
    if (deg == 3) in_note("az" S_CU " + bz" S_SQ " + cz + d = 0");
    if (deg == 4) in_note("az^4 + bz" S_CU " + cz" S_SQ " + dz + e = 0");
    for (int i = 0; i <= deg; i++) a[deg - i] = in_real(lab[i]);
    if (a[deg] == 0) fail("Leading coefficient can't be 0");
    return deg;
}

static void r_sums(void) {
    double a[5];
    int deg = read_poly(a);
    double A = a[deg];
    double s1 = -a[deg - 1] / A, s2 = a[deg - 2] / A;
    const char *R1 = deg == 2 ? S_AL "+" S_BE : S_SIG S_AL;
    outf(HEAD "Basic relations");
    outf("%s = -b/a = %s", R1, fx(s1));
    if (deg == 2) outf(S_AL S_BE " = c/a = %s", fx(s2));
    else outf(S_SIG S_AL S_BE " = c/a = %s", fx(s2));
    double s3 = 0, s4 = 0;
    if (deg >= 3) { s3 = -a[deg - 3] / A; outf("%s = -d/a = %s", deg == 3 ? S_AL S_BE S_GA : S_SIG S_AL S_BE S_GA, fx(s3)); }
    if (deg == 4) { s4 = a[0] / A; outf(S_AL S_BE S_GA S_DE " = e/a = %s", fx(s4)); }
    outf(HEAD "Derived");
    double sq = s1 * s1 - 2 * s2;
    outf(S_SIG S_AL S_SQ " = (" S_SIG S_AL ")" S_SQ " - 2" S_SIG S_AL S_BE " = %s", fx(sq));
    double prod = deg == 2 ? s2 : deg == 3 ? s3 : s4;
    double pen = deg == 2 ? s1 : deg == 3 ? s2 : s3;
    if (fabs(prod) > 1e-12) outf(S_SIG "1/" S_AL " = %s/%s = %s", fxp(pen), fxp(prod), fx(pen / prod));
    double cube;
    if (deg == 2) cube = s1 * s1 * s1 - 3 * s1 * s2;
    else cube = s1 * s1 * s1 - 3 * s1 * s2 + 3 * s3;
    outf(S_SIG S_AL S_CU " = %s", fx(cube));
    if (deg == 2) outf("  = (" S_AL "+" S_BE ")" S_CU " - 3" S_AL S_BE "(" S_AL "+" S_BE ")");
    else outf("  = (" S_SIG S_AL ")" S_CU " - 3" S_SIG S_AL S_SIG S_AL S_BE " + 3" S_AL S_BE S_GA);
    /* sum of 1/alpha^2 */
    if (fabs(prod) > 1e-12) {
        cx r[4];
        int n = proots_r(a, deg, r);
        cx s = R(0);
        for (int i = 0; i < n; i++) s = cadd(s, cdiv(R(1), cmul(r[i], r[i])));
        outf(S_SIG "1/" S_AL S_SQ " = %s", fx(s.re));
    }
}

static void r_transform(void) {
    double a[5];
    int deg = read_poly(a);
    static const char *o[] = {"p" S_AL " + q (e.g. 2" S_AL ", " S_AL "+1)", "1/" S_AL, S_AL S_SQ};
    int k = in_choice("New roots", o, 3);
    double b[9] = {0};
    int nd = deg;
    if (k == 0) {
        double p = in_real("p"), q = in_real("q");
        if (p == 0) fail("p can't be 0");
        outf("Let w = %s" S_AL "%s, so " S_AL " = (w%s)/%s", fx(p), fcoef(q, "", 0), fcoef(-q, "", 0), fx(p));
        outf("Substitute z = (w%s)/%s into the equation", fcoef(-q, "", 0), fx(p));
        /* b(w) = p^deg * a((w - q)/p) */
        double lin[2] = {-q / p, 1 / p}, acc[9] = {a[deg]};
        int ad = 0;
        for (int i = deg - 1; i >= 0; i--) {
            pmul(acc, ad, lin, 1, acc);
            ad++;
            acc[0] += a[i];
        }
        for (int i = 0; i <= deg; i++) b[i] = acc[i] * pow(p, deg);
    } else if (k == 1) {
        if (fabs(a[0]) < 1e-12) fail("0 is a root: 1/" S_AL " undefined");
        outf("Let w = 1/z, so z = 1/w");
        outf("Substitute and multiply by w^%d: coefficients reverse", deg);
        for (int i = 0; i <= deg; i++) b[i] = a[deg - i];
    } else {
        outf("Let w = z" S_SQ ", so z = " S_SQRT "w");
        outf("Put even powers on one side, odd on the other, then square");
        double E[5] = {0}, O[5] = {0};
        for (int i = 0; i <= deg; i++) { if (i % 2 == 0) E[i / 2] = a[i]; else O[i / 2] = a[i]; }
        int de = deg / 2, dodd = (deg - 1) / 2;
        outf("%s = -" S_SQRT "w(%s)", fpoly(E, de, "w"), fpoly(O, dodd, "w"));
        double E2[9], O2[9];
        pmul(E, de, E, de, E2);
        pmul(O, dodd, O, dodd, O2);
        for (int i = 0; i <= 2 * de; i++) b[i] += E2[i];
        for (int i = 0; i <= 2 * dodd; i++) b[i + 1] -= O2[i];
        nd = deg;
    }
    double s[9];
    memcpy(s, b, sizeof s);
    if (pintscale(s, nd)) memcpy(b, s, sizeof s);
    outf(ANS "%s = 0", fpoly(b, nd, "w"));
}

static const calc_t roots_c[] = {
    {"Sums & products of roots", r_sums},
    {"Equation with transformed roots", r_transform},
};

/* ======================= SERIES ======================= */
static void s_standard(void) {
    long a = in_int("From r =", -100000, 100000), b = in_int("To r =", -100000, 100000);
    if (b < a) fail("Upper limit must be " S_GE " lower");
    outf(HEAD "Formulae");
    outf(S_SIG "r = n(n+1)/2");
    outf(S_SIG "r" S_SQ " = n(n+1)(2n+1)/6");
    outf(S_SIG "r" S_CU " = n" S_SQ "(n+1)" S_SQ "/4");
    outf("Sum from %d to %d = S(%d) - S(%d)", (int)a, (int)b, (int)b, (int)(a - 1));
    double B = b, A = a - 1;
    double s1 = B * (B + 1) / 2 - A * (A + 1) / 2;
    double s2 = B * (B + 1) * (2 * B + 1) / 6 - A * (A + 1) * (2 * A + 1) / 6;
    double s3 = B * B * (B + 1) * (B + 1) / 4 - A * A * (A + 1) * (A + 1) / 4;
    outf(ANS S_SIG "r = %s - %s = %s", fd(B * (B + 1) / 2), fd(A * (A + 1) / 2), fd(s1));
    outf(ANS S_SIG "r" S_SQ " = %s - %s = %s", fd(B * (B + 1) * (2 * B + 1) / 6), fd(A * (A + 1) * (2 * A + 1) / 6), fd(s2));
    outf(ANS S_SIG "r" S_CU " = %s - %s = %s", fd(B * B * (B + 1) * (B + 1) / 4), fd(A * A * (A + 1) * (A + 1) / 4), fd(s3));
}

static void s_polysum(void) {
    in_note(S_SIG " (ar" S_CU " + br" S_SQ " + cr + d) from r = 1 to n");
    double a = in_real("a (r" S_CU ")"), b = in_real("b (r" S_SQ ")"), c = in_real("c (r)"), d = in_real("d (const)");
    /* result polynomial in n */
    double P[5] = {0};
    /* r^3: n^4/4 + n^3/2 + n^2/4 */
    P[4] += a / 4; P[3] += a / 2; P[2] += a / 4;
    P[3] += b / 3; P[2] += b / 2; P[1] += b / 6;
    P[2] += c / 2; P[1] += c / 2;
    P[1] += d;
    {
        char *t = sbuf();
        int first = 1;
        const double co[4] = {a, b, c, d};
        const char *tm[4] = {S_SIG "r" S_CU, S_SIG "r" S_SQ, S_SIG "r", "n"};
        const char *fm_[4] = {"n" S_SQ "(n+1)" S_SQ "/4", "n(n+1)(2n+1)/6", "n(n+1)/2", "n"};
        for (int i = 0; i < 4; i++) if (fabs(co[i]) > 1e-12) { strcat(t, fcoef(co[i], tm[i], first)); first = 0; }
        outf("= %s", first ? "0" : t);
        first = 1;
        for (int i = 0; i < 4; i++) if (fabs(co[i]) > 1e-12) {
            char u[48]; strcpy(u, i < 3 ? "[" : ""); strcat(u, fm_[i]); if (i < 3) strcat(u, "]");
            outf("%s%s", first ? "= " : "  ", fcoef(co[i], u, first)); first = 0;
        }
    }
    outf("Expand:");
    outf("= %s", fpoly(P, 4, "n"));
    outf(ANS "= %s", pfactor(P, 4, "n"));
}

void s_numeric(void) {
    node *f = in_fn("f(r)", 'r');
    int a = in_int("From r =", -1000000, 1000000), b = in_int("To r =", -1000000, 1000000);
    if (b < a) fail("Upper limit must be " S_GE " lower");
    if ((long)b - a > 200000) fail("Too many terms (max 200000)");
    double s = 0;
    for (long r = a; r <= b; r++) {
        double v = ex_evalr(f, 'r', r);
        if (r - a < 4) outf("r = %d: %s", (int)r, fx(v));
        s += v;
    }
    if (b - a >= 4) outf("...");
    outf(ANS "Sum = %s", fxd(s));
}

static void s_diff(void) {
    in_note(S_SIG " 1/((r + p)(r + q)) from r = 1 to n, p < q integers");
    int p = in_int("p", -50, 50), q = in_int("q", -50, 50);
    if (q <= p) fail("Need q > p");
    if (p < 0 && -p >= 1) fail("Need r + p " S_NE " 0 for r " S_GE " 1 (p " S_GE " 0)");
    int k = q - p;
    outf("Partial fractions:");
    outf("1/(%s%s) = (1/%d)[1/%s - 1/%s]", fshift("r", -p), fshift("r", -q), k, fshift("r", -p), fshift("r", -q));
    outf("Write out terms; most cancel (telescoping)");
    outf("Survivors: + first %d terms, - last %d terms", k, k);
    char *s = sbuf();
    for (int j = p + 1; j <= q; j++) { if (j > p + 1) strcat(s, " + "); strcat(s, "1/"); strcat(s, fd(j)); }
    outf(ANS "S(n) = (1/%d)[%s", k, s);
    char *t = sbuf();
    for (int j = p + 1; j <= q; j++) {
        strcat(t, " - 1/(n+");
        strcat(t, fd(j));
        strcat(t, ")");
    }
    outf(ANS "   %s]", t);
    double H = 0;
    for (int j = p + 1; j <= q; j++) H += 1.0 / j;
    outf(ANS "Sum to " S_INF " = %s", fx(H / k));
    int n = in_int("Evaluate S(n) for n =", 1, 1000000);
    double v = H;
    for (int j = p + 1; j <= q; j++) v -= 1.0 / (n + j);
    outf(ANS "S(%d) = %s", n, fxd(v / k));
}

static const calc_t series_c[] = {
    {S_SIG "r, " S_SIG "r" S_SQ ", " S_SIG "r" S_CU " between limits", s_standard},
    {S_SIG " of cubic in r (formula in n)", s_polysum},
    {S_SIG " of any f(r) (numeric)", s_numeric},
    {"Method of differences 1/((r+p)(r+q))", s_diff},
};

/* ======================= INDUCTION ======================= */
static void i_series(void) {
    in_note("Checks " S_SIG "f(r) from 1 to n = g(n) for n = 1..8");
    node *f = in_fn("f(r)", 'r');
    node *g = in_fn("g(n)", 'n');
    double s = 0;
    int ok = 1;
    for (int n = 1; n <= 8; n++) {
        s += ex_evalr(f, 'r', n);
        double gv = ex_evalr(g, 'n', n);
        int m = fabs(s - gv) < 1e-9 * (1 + fabs(s));
        if (!m) ok = 0;
        outf("n=%d: " S_SIG " = %s, g = %s %s", n, fx(s), fx(gv), m ? "ok" : "X");
    }
    outf(ANS "%s", ok ? "Formula holds for n = 1..8" : "Formula FAILS (check it)");
    outf("Inductive step: S(k+1) = S(k) + f(k+1)");
}

static void i_divis(void) {
    node *f = in_fn("f(n)", 'n');
    long g = 0;
    int okall = 1;
    for (int n = 1; n <= 10; n++) {
        double v = ex_evalr(f, 'n', n);
        if (fabs(v) > 9e15 || v != floor(v)) { okall = 0; outf("n=%d: %s", n, fd(v)); continue; }
        g = lgcd(g, (long)fabs(v));
        outf("n=%d: f = %s", n, fd(v));
    }
    if (okall) outf(ANS "Largest divisor of all: %s", fd(g));
    outf("Inductive step tip: write f(k+1) - (const)f(k)");
    outf("and show it's a multiple of the divisor");
}

static const calc_t induct_c[] = {
    {"Check a series formula", i_series},
    {"Find divisibility of f(n)", i_divis},
};

/* ======================= VECTORS ======================= */
void v_line(void) {
    double a[3], d[3];
    in_line3("Line", a, d);
    out_line3("Line", a, d);
}

static void v_plane(void) {
    double n[3], d;
    in_plane3("Plane", n, &d);
    outf(ANS "n = %s", fvec(n, 3));
    outf(ANS "r" S_DOT "%s = %s", fvec(n, 3), fx(d));
    outf(ANS "%s", fplane(n, d));
    outf("Distance from O = |d|/|n| = %s", fxd(fabs(d) / norm3(n)));
}

void v_products(void) {
    double a[3], b[3], c[3];
    in_vec("a", a, 3);
    in_vec("b", b, 3);
    double dt = dot3(a, b);
    outf("a" S_DOT "b = (%s)(%s) + (%s)(%s) + (%s)(%s)", fx(a[0]), fx(b[0]), fx(a[1]), fx(b[1]), fx(a[2]), fx(b[2]));
    outf(ANS "a" S_DOT "b = %s", fx(dt));
    cross3(a, b, c);
    outf(ANS "a " S_TIMES " b = %s", fvec(c, 3));
    outf("|a| = %s, |b| = %s", fxd(norm3(a)), fxd(norm3(b)));
    if (norm3(a) > 0 && norm3(b) > 0) {
        double ct = dt / (norm3(a) * norm3(b));
        outf("cos " S_TH " = a" S_DOT "b/(|a||b|) = %s", fx(ct));
        outf(ANS S_TH " = %s", fang(acos(fmax(-1, fmin(1, ct)))));
        if (fabs(dt) < 1e-12) outf("a" S_DOT "b = 0: perpendicular");
    }
}

static void v_angle(void) {
    static const char *o[] = {"Two lines", "Line and plane", "Two planes"};
    int k = in_choice("Angle between", o, 3);
    double u[3], v[3], p[3], dd;
    if (k == 0) {
        in_line3("Line 1", p, u);
        in_line3("Line 2", p, v);
        outf("Use directions d1 = %s, d2 = %s", fvec(u, 3), fvec(v, 3));
    } else if (k == 1) {
        in_line3("Line", p, u);
        in_plane3("Plane", v, &dd);
        outf("Use direction d = %s, normal n = %s", fvec(u, 3), fvec(v, 3));
    } else {
        in_plane3("Plane 1", u, &dd);
        in_plane3("Plane 2", v, &dd);
        outf("Use normals n1 = %s, n2 = %s", fvec(u, 3), fvec(v, 3));
    }
    double dt = dot3(u, v), m = norm3(u) * norm3(v);
    outf("Dot product = %s", fx(dt));
    outf("Magnitudes: %s " S_TIMES " %s", fxd(norm3(u)), fxd(norm3(v)));
    if (k == 1) {
        double s = fabs(dt) / m;
        outf("sin " S_TH " = |d" S_DOT "n|/(|d||n|) = %s", fx(s));
        outf("(angle with normal is 90" S_DEG " - " S_TH ")");
        outf(ANS S_TH " = %s", fang(asin(fmin(1, s))));
    } else {
        double c = fabs(dt) / m;
        outf("cos " S_TH " = |%s|/(%s) = %s", fx(dt), fx(m), fx(c));
        outf(ANS "Acute angle " S_TH " = %s", fang(acos(fmin(1, c))));
    }
}

static void v_lineplane(void) {
    double a[3], d[3], n[3], dd;
    in_line3("Line", a, d);
    in_plane3("Plane", n, &dd);
    outf("Sub r = a + " S_LA "d into r" S_DOT "n = %s", fx(dd));
    double an = dot3(a, n), dn = dot3(d, n);
    outf("a" S_DOT "n + " S_LA "(d" S_DOT "n) = %s", fx(dd));
    outf("%s + %s" S_LA " = %s", fx(an), fx(dn), fx(dd));
    if (fabs(dn) < 1e-12) {
        if (fabs(an - dd) < 1e-9) outf(ANS "Line lies IN the plane");
        else {
            outf(ANS "Line is parallel to the plane (no intersection)");
            outf("Distance = %s", fxd(fabs(an - dd) / norm3(n)));
        }
        return;
    }
    double L = (dd - an) / dn;
    double p[3] = {a[0] + L * d[0], a[1] + L * d[1], a[2] + L * d[2]};
    outf(S_LA " = %s", fx(L));
    outf(ANS "Intersection: %s", fvec(p, 3));
}

static void v_twolines(void) {
    double a1[3], d1[3], a2[3], d2[3];
    in_line3("Line 1", a1, d1);
    in_line3("Line 2", a2, d2);
    double c[3], w[3];
    cross3(d1, d2, c);
    for (int i = 0; i < 3; i++) w[i] = a2[i] - a1[i];
    outf("d1 " S_TIMES " d2 = %s", fvec(c, 3));
    if (norm3(c) < 1e-12) {
        double t[3];
        cross3(w, d1, t);
        if (norm3(t) < 1e-12) { outf(ANS "Lines are identical (coincident)"); return; }
        outf(ANS "Lines are parallel");
        outf("Distance = |(a2 - a1) " S_TIMES " d1| / |d1|");
        outf(ANS "Distance = %s", fxd(norm3(t) / norm3(d1)));
        return;
    }
    outf("Set %s + " S_LA "%s", fvec(a1, 3), fvec(d1, 3));
    outf("  = %s + " S_MU "%s", fvec(a2, 3), fvec(d2, 3));
    /* closest points: solve [d1.d1 -d1.d2; d1.d2 -d2.d2][l m] = [w.d1; w.d2] */
    double A = dot3(d1, d1), B = dot3(d1, d2), Cc = dot3(d2, d2), D = dot3(w, d1), E = dot3(w, d2);
    double det = -A * Cc + B * B;
    double l = (-D * Cc + B * E) / det, m = (A * E - B * D) / det;
    double P[3], Q[3];
    for (int i = 0; i < 3; i++) { P[i] = a1[i] + l * d1[i]; Q[i] = a2[i] + m * d2[i]; }
    double dist = fabs(dot3(w, c)) / norm3(c);
    if (dist < 1e-9) {
        outf("Solve two components, check the third:");
        outf(S_LA " = %s, " S_MU " = %s (consistent)", fx(l), fx(m));
        outf(ANS "Lines intersect at %s", fvec(P, 3));
        double ct = fabs(dot3(d1, d2)) / (norm3(d1) * norm3(d2));
        outf("Acute angle = %s", fang(acos(fmin(1, ct))));
    } else {
        outf("No solution for " S_LA ", " S_MU ": not parallel, so skew");
        outf(ANS "Lines are skew");
        outf("Shortest distance = |(a2 - a1)" S_DOT "(d1 " S_TIMES " d2)| / |d1 " S_TIMES " d2|");
        outf("= |%s" S_DOT "%s| / %s", fvec(w, 3), fvec(c, 3), fxd(norm3(c)));
        outf(ANS "Distance = %s", fxd(dist));
        outf("Closest points: " S_LA " = %s, " S_MU " = %s", fx(l), fx(m));
        outf("  %s and %s", fvec(P, 3), fvec(Q, 3));
    }
}

void v_dist(void) {
    static const char *o[] = {"Point to plane", "Point to line"};
    int k = in_choice("Distance", o, 2);
    double p[3];
    if (k == 0) {
        double n[3], d;
        in_vec("Point P", p, 3);
        in_plane3("Plane", n, &d);
        double num = dot3(p, n) - d;
        outf("Distance = |n" S_DOT "p - d| / |n|");
        outf("= |%s - %s| / %s", fx(dot3(p, n)), fx(d), fxd(norm3(n)));
        outf(ANS "Distance = %s", fxd(fabs(num) / norm3(n)));
        double t = num / dot3(n, n);
        double f[3] = {p[0] - t * n[0], p[1] - t * n[1], p[2] - t * n[2]};
        outf("Foot of perpendicular: %s", fvec(f, 3));
        double r[3] = {p[0] - 2 * t * n[0], p[1] - 2 * t * n[1], p[2] - 2 * t * n[2]};
        outf("Reflection of P in plane: %s", fvec(r, 3));
    } else {
        double a[3], d[3];
        in_vec("Point P", p, 3);
        in_line3("Line", a, d);
        double w[3] = {p[0] - a[0], p[1] - a[1], p[2] - a[2]};
        double l = dot3(w, d) / dot3(d, d);
        double f[3] = {a[0] + l * d[0], a[1] + l * d[1], a[2] + l * d[2]};
        outf("Foot F = a + " S_LA "d with PF" S_DOT "d = 0");
        outf(S_LA " = (p - a)" S_DOT "d / |d|" S_SQ " = %s", fx(l));
        outf("F = %s", fvec(f, 3));
        double pf[3] = {f[0] - p[0], f[1] - p[1], f[2] - p[2]};
        outf(ANS "Distance = |PF| = %s", fxd(norm3(pf)));
    }
}

static void v_twoplanes(void) {
    double n1[3], d1, n2[3], d2;
    in_plane3("Plane 1", n1, &d1);
    in_plane3("Plane 2", n2, &d2);
    double c[3];
    cross3(n1, n2, c);
    if (norm3(c) < 1e-12) {
        outf(ANS "Planes are parallel");
        double k = norm3(n2) / norm3(n1) * (dot3(n1, n2) < 0 ? -1 : 1);
        outf("Distance between = %s", fxd(fabs(d2 / k - d1) / norm3(n1)));
        return;
    }
    outf("Direction = n1 " S_TIMES " n2 = %s", fvec(c, 3));
    /* point: set the coordinate with largest |c| to 0 */
    int z = 0;
    for (int i = 1; i < 3; i++) if (fabs(c[i]) > fabs(c[z])) z = i;
    int i1 = (z + 1) % 3, i2 = (z + 2) % 3;
    double det = n1[i1] * n2[i2] - n1[i2] * n2[i1];
    double p[3] = {0};
    p[i1] = (d1 * n2[i2] - d2 * n1[i2]) / det;
    p[i2] = (n1[i1] * d2 - n2[i1] * d1) / det;
    static const char *nm = "xyz";
    outf("Put %c = 0 and solve: point %s", nm[z], fvec(p, 3));
    out_line3("Line of intersection", p, c);
    double ct = fabs(dot3(n1, n2)) / (norm3(n1) * norm3(n2));
    outf("Angle between planes = %s", fang(acos(fmin(1, ct))));
}

static const calc_t vector_c[] = {
    {"Equation of a line", v_line},
    {"Equation of a plane", v_plane},
    {"Scalar & vector product", v_products},
    {"Angle between lines / planes", v_angle},
    {"Line meets plane", v_lineplane},
    {"Two lines (meet / skew / distance)", v_twolines},
    {"Distance point to plane / line", v_dist},
    {"Two planes (line of intersection)", v_twoplanes},
};

const topic_t fm_u1[] = {
    {"Complex numbers", complex_c, NEL(complex_c)},
    {"Matrices & transformations", matrix_c, NEL(matrix_c)},
    {"Roots of polynomials", roots_c, NEL(roots_c)},
    {"Series & method of differences", series_c, NEL(series_c)},
    {"Proof by induction (checker)", induct_c, NEL(induct_c)},
    {"Vectors, lines & planes", vector_c, NEL(vector_c)},
};
const int fm_u1_n = NEL(fm_u1);
