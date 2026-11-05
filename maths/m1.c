/* A Level Maths Unit 1: Pure Mathematics A (AS) */
#include "../common/core.h"
#include "../common/util.h"
#include "../common/shared.h"
#include "../common/stats.h"
#include <string.h>

/* ======================= ALGEBRA ======================= */
static void a_quadratic(void) {
    in_note("ax" S_SQ " + bx + c");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    if (a == 0) fail("a can't be 0");
    double q[3] = {c, b, a};
    double D = b * b - 4 * a * c;
    outf("f(x) = %s", fpoly(q, 2, "x"));
    outf("Discriminant b" S_SQ " - 4ac = %s", fx(D));
    outf("%s", D > 1e-12 ? "> 0: two distinct real roots" : D > -1e-12 ? "= 0: one repeated root" : "< 0: no real roots");
    if (D >= -1e-12) {
        double s = sqrt(fmax(D, 0));
        outf("x = (-b " S_PM " " S_SQRT "(b" S_SQ " - 4ac))/2a");
        outf(ANS "x = %s,  x = %s", fx((-b + s) / (2 * a)), fx((-b - s) / (2 * a)));
        if (D > 1e-12) outf("Factorised: %s", pfactor(q, 2, "x"));
    }
    double h = -b / (2 * a), k = c - b * b / (4 * a);
    outf(HEAD "Completed square");
    outf(ANS "%s%s" S_SQ "%s", a == 1 ? "" : fx(a), fshift("x", h), fcoef(k, "", 0));
    outf("Vertex (turning point): (%s, %s), %s", fx(h), fx(k), a > 0 ? "minimum" : "maximum");
    outf("Line of symmetry x = %s, y-intercept %s", fx(h), fx(c));
}

static void a_ineq(void) {
    in_note("ax" S_SQ " + bx + c  (compared with 0)");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    static const char *o[] = {"> 0", S_GE " 0", "< 0", S_LE " 0"};
    int k = in_choice("Inequality", o, 4);
    if (a == 0) fail("a can't be 0");
    double D = b * b - 4 * a * c;
    double q[3] = {c, b, a};
    outf("Solve %s %s", fpoly(q, 2, "x"), o[k]);
    const char *st = (k == 1 || k == 3) ? S_LE : "<";
    const char *gt = (k == 1 || k == 3) ? S_GE : ">";
    int want_pos = (k <= 1) ^ (a < 0);
    if (D <= 0) {
        if (D < -1e-12 || (k == 0 || k == 2)) {
            int all = want_pos;
            if (D > -1e-12 && (k == 0 || k == 2)) { outf("Repeated root x = %s", fx(-b / (2 * a))); outf(ANS "%s", all ? "All x except the root" : "No solutions"); return; }
            outf("No real roots: graph always %s the axis", a > 0 ? "above" : "below");
            outf(ANS "%s", all ? "All real x" : "No solutions");
            return;
        }
        outf(ANS "%s", want_pos ? "All real x" : "Only x = root");
        return;
    }
    double r1 = (-b - sqrt(D)) / (2 * a), r2 = (-b + sqrt(D)) / (2 * a);
    if (r1 > r2) { double t = r1; r1 = r2; r2 = t; }
    outf("Critical values x = %s, %s", fx(r1), fx(r2));
    outf("Sketch: %s parabola", a > 0 ? "U-shaped" : "n-shaped");
    if (want_pos) outf(ANS "x %s %s  or  x %s %s", st, fx(r1), gt, fx(r2));
    else outf(ANS "%s %s x %s %s", fx(r1), st, st, fx(r2));
}

static void a_simul(void) {
    static const char *o[] = {"Two linear", "Linear and quadratic (line + curve)"};
    int k = in_choice("Type", o, 2);
    if (k == 0) {
        double e1[3], e2[3];
        in_vec("ax + by = c: a, b, c", e1, 3);
        in_vec("dx + ey = f: d, e, f", e2, 3);
        double D = e1[0] * e2[1] - e1[1] * e2[0];
        if (fabs(D) < 1e-12) fail("Parallel lines: no unique solution");
        double x = (e1[2] * e2[1] - e1[1] * e2[2]) / D, y = (e1[0] * e2[2] - e1[2] * e2[0]) / D;
        outf(ANS "x = %s,  y = %s", fx(x), fx(y));
        return;
    }
    in_note("Line y = mx + c,  curve y = px" S_SQ " + qx + r");
    double m = in_real("m"), c = in_real("c (line)");
    double p = in_real("p"), q = in_real("q"), r = in_real("r");
    outf("Substitute: %sx + %s = %s", fx(m), fx(c), fpoly((double[]){r, q, p}, 2, "x"));
    double Q[3] = {r - c, q - m, p};
    outf("%s = 0", fpoly(Q, 2, "x"));
    cx rt[2];
    int n = proots_r(Q, fabs(p) > 1e-12 ? 2 : 1, rt);
    double D = Q[1] * Q[1] - 4 * Q[2] * Q[0];
    if (fabs(p) > 1e-12) outf("Discriminant = %s: %s", fx(D), D > 1e-12 ? "meets twice" : D > -1e-12 ? "line is a tangent" : "no intersection");
    for (int i = 0; i < n; i++) if (rt[i].im == 0) outf(ANS "x = %s, y = %s", fx(rt[i].re), fx(m * rt[i].re + c));
}

static void a_factor(void) {
    in_note("Polynomial: enter coefficients from highest power");
    double c[10];
    int n = in_list("Coefficients (e.g. 2,-3,-11,6)", c, 9);
    int d = n - 1;
    double a[10];
    for (int i = 0; i <= d; i++) a[i] = c[d - i];
    if (d < 1) fail("Need degree " S_GE " 1");
    outf("f(x) = %s", fpoly(a, d, "x"));
    for (int t = -6; t <= 6; t++) {
        double v = peval_r(a, d, t);
        if (fabs(v) < 1e-9) outf("f(%d) = 0, so (x %s %d) is a factor", t, t < 0 ? "+" : "-", t < 0 ? -t : t);
    }
    outf(ANS "f(x) = %s", pfactor(a, d, "x"));
    cx r[9];
    int nr = proots_r(a, d, r);
    char *s = sbuf();
    for (int i = 0; i < nr; i++) if (r[i].im == 0) { if (s[0]) strcat(s, ", "); strcat(s, fx(r[i].re)); }
    outf(ANS "Real roots: %s", s[0] ? s : "none");
    static const char *o[] = {"Done", "Remainder when divided by (ax + b)"};
    if (in_choice("More?", o, 2)) {
        double L[2];
        in_vec("a, b of (ax + b)", L, 2);
        double root = -L[1] / L[0];
        outf("Remainder theorem: f(%s)", fx(root));
        outf(ANS "Remainder = %s", fx(peval_r(a, d, root)));
        double b[2] = {L[1], L[0]}, q[10], rem[2];
        pdiv(a, d, b, 1, q, rem);
        outf("Quotient = %s", fpoly(q, d - 1, "x"));
    }
}

static const calc_t alg_c[] = {
    {"Quadratic: roots, discriminant, vertex", a_quadratic},
    {"Quadratic inequality", a_ineq},
    {"Simultaneous equations", a_simul},
    {"Factor & remainder theorem", a_factor},
};

/* ======================= COORDINATE GEOMETRY ======================= */
static const char *line_eq(double m, double c) {
    char *o = sbuf();
    strcpy(o, "y = ");
    if (fabs(m) > 1e-12) strcat(o, fcoef(m, "x", 1));
    if (fabs(c) > 1e-12 || fabs(m) < 1e-12) strcat(o, fabs(m) > 1e-12 ? fcoef(c, "", 0) : fx(c));
    return o;
}

static void g_line(void) {
    double A[2], B[2];
    in_vec("Point A (x, y)", A, 2);
    in_vec("Point B (x, y)", B, 2);
    double dx = B[0] - A[0], dy = B[1] - A[1];
    outf("Midpoint = (%s, %s)", fx((A[0] + B[0]) / 2), fx((A[1] + B[1]) / 2));
    outf("Distance = " S_SQRT "(%s" S_SQ " + %s" S_SQ ") = %s", fxp(dx), fxp(dy), fxd(hypot(dx, dy)));
    if (fabs(dx) < 1e-12) { outf(ANS "Vertical line x = %s", fx(A[0])); return; }
    double m = dy / dx, c = A[1] - m * A[0];
    outf("Gradient m = %s/%s = %s", fx(dy), fx(dx), fx(m));
    outf("y - %s = %s(x - %s)", fxp(A[1]), fx(m), fxp(A[0]));
    outf(ANS "%s", line_eq(m, c));
    double s[3] = {c, m, -1};
    if (pintscale(s, 2)) outf(ANS "%s%s%s = 0", fcoef(s[1], "x", 1), fcoef(s[2], "y", 0), fcoef(s[0], "", 0));
    if (fabs(m) > 1e-12) {
        double mp = -1 / m, mx = (A[0] + B[0]) / 2, my = (A[1] + B[1]) / 2;
        outf("Perpendicular gradient = -1/m = %s", fx(mp));
        outf("Perpendicular bisector: %s", line_eq(mp, my - mp * mx));
    }
}

static void g_meet(void) {
    double l1[2], l2[2];
    in_vec("Line 1: m, c (y = mx + c)", l1, 2);
    in_vec("Line 2: m, c", l2, 2);
    if (fabs(l1[0] - l2[0]) < 1e-12) { outf(ANS "Parallel: %s", fabs(l1[1] - l2[1]) < 1e-12 ? "same line" : "no intersection"); return; }
    double x = (l2[1] - l1[1]) / (l1[0] - l2[0]);
    outf("%sx + %s = %sx + %s", fx(l1[0]), fx(l1[1]), fx(l2[0]), fx(l2[1]));
    outf(ANS "(%s, %s)", fx(x), fx(l1[0] * x + l1[1]));
    if (fabs(l1[0] * l2[0] + 1) < 1e-12) outf("m1" S_TIMES "m2 = -1: perpendicular");
}

static void g_circle(void) {
    static const char *o[] = {"From x" S_SQ "+y" S_SQ "+ax+by+c = 0", "Centre and a point", "Through 3 points", "Ends of a diameter"};
    int k = in_choice("Circle given by", o, 4);
    double cx_, cy, r2;
    if (k == 0) {
        double v[3];
        in_vec("a, b, c", v, 3);
        cx_ = -v[0] / 2; cy = -v[1] / 2; r2 = cx_ * cx_ + cy * cy - v[2];
        outf("Complete the square in x and y:");
        outf("%s" S_SQ " - %s + %s" S_SQ " - %s%s = 0", fshift("x", cx_), fx(cx_ * cx_), fshift("y", cy), fx(cy * cy), fcoef(v[2], "", 0));
    } else if (k == 1) {
        double C[2], P[2];
        in_vec("Centre (a, b)", C, 2);
        in_vec("Point on circle", P, 2);
        cx_ = C[0]; cy = C[1]; r2 = (P[0] - cx_) * (P[0] - cx_) + (P[1] - cy) * (P[1] - cy);
    } else if (k == 3) {
        double P[2], Q[2];
        in_vec("End A", P, 2);
        in_vec("End B", Q, 2);
        cx_ = (P[0] + Q[0]) / 2; cy = (P[1] + Q[1]) / 2;
        r2 = (P[0] - cx_) * (P[0] - cx_) + (P[1] - cy) * (P[1] - cy);
        outf("Centre = midpoint of AB");
    } else {
        double P[3][2];
        in_vec("Point 1", P[0], 2);
        in_vec("Point 2", P[1], 2);
        in_vec("Point 3", P[2], 2);
        double M[9], b[3];
        for (int i = 0; i < 3; i++) { M[i * 3] = P[i][0]; M[i * 3 + 1] = P[i][1]; M[i * 3 + 2] = 1; b[i] = -(P[i][0] * P[i][0] + P[i][1] * P[i][1]); }
        if (!lsolve(M, b, 3)) fail("Points are collinear");
        cx_ = -b[0] / 2; cy = -b[1] / 2; r2 = cx_ * cx_ + cy * cy - b[2];
        outf("Solve for a, b, c in x" S_SQ "+y" S_SQ "+ax+by+c = 0:");
        outf("a = %s, b = %s, c = %s", fx(b[0]), fx(b[1]), fx(b[2]));
    }
    if (r2 <= 0) fail("Not a real circle (r" S_SQ " " S_LE " 0)");
    outf(ANS "Centre (%s, %s), radius %s", fx(cx_), fx(cy), fxd(sqrt(r2)));
    outf(ANS "%s" S_SQ " + %s" S_SQ " = %s", fshift("x", cx_), fshift("y", cy), fx(r2));
    static const char *m[] = {"Done", "Tangent at a point", "Where a line y = mx + c meets it"};
    int t = in_choice("More?", m, 3);
    if (t == 1) {
        double P[2];
        in_vec("Point on circle", P, 2);
        double dx = P[0] - cx_, dy = P[1] - cy;
        if (fabs(dx * dx + dy * dy - r2) > 1e-6 * (1 + r2)) outf("Warning: point isn't on the circle");
        if (fabs(dx) < 1e-12) { outf(ANS "Tangent: y = %s", fx(P[1])); return; }
        double mr = dy / dx;
        outf("Radius gradient = %s", fx(mr));
        if (fabs(dy) < 1e-12) { outf(ANS "Tangent: x = %s", fx(P[0])); return; }
        double mt = -1 / mr;
        outf("Tangent gradient = -1/%s = %s", fxp(mr), fx(mt));
        outf(ANS "Tangent: %s", line_eq(mt, P[1] - mt * P[0]));
    } else if (t == 2) {
        double m_ = in_real("m"), c = in_real("c");
        double A = 1 + m_ * m_, B = -2 * cx_ + 2 * m_ * (c - cy), C_ = cx_ * cx_ + (c - cy) * (c - cy) - r2;
        double q[3] = {C_, B, A};
        outf("Substitute: %s = 0", fpoly(q, 2, "x"));
        double D = B * B - 4 * A * C_;
        outf("Discriminant = %s: %s", fx(D), D > 1e-12 ? "two points" : D > -1e-12 ? "tangent" : "doesn't meet");
        if (D >= -1e-12) {
            double x1 = (-B + sqrt(fmax(D, 0))) / (2 * A), x2 = (-B - sqrt(fmax(D, 0))) / (2 * A);
            outf(ANS "(%s, %s)", fx(x1), fx(m_ * x1 + c));
            if (D > 1e-12) outf(ANS "(%s, %s)", fx(x2), fx(m_ * x2 + c));
        }
    }
}

static const calc_t geo_c[] = {
    {"Line through 2 points", g_line},
    {"Where two lines meet", g_meet},
    {"Circles", g_circle},
};

/* ======================= BINOMIAL EXPANSION ======================= */
static double nCr(int n, int r) { double v = 1; for (int i = 1; i <= r; i++) v = v * (n - r + i) / i; return v; }

static void b_expand(void) {
    in_note("(a + bx)^n, n a positive whole number");
    double a = in_real("a"), b = in_real("b");
    int n = in_int("n", 1, 20);
    static const char *o[] = {"Full expansion", "First few terms", "Coefficient of x^r"};
    int k = in_choice("Find", o, 3);
    if (k == 2) {
        int r = in_int("r", 0, n);
        double c = nCr(n, r) * pow(a, n - r) * pow(b, r);
        outf("Term = nCr " S_TIMES " a^(n-r) " S_TIMES " (bx)^r");
        outf("= %s " S_TIMES " %s^%d " S_TIMES " %s^%d", fd(nCr(n, r)), fxp(a), n - r, fxp(b), r);
        outf(ANS "Coefficient of x^%d = %s", r, fx(c));
        return;
    }
    int upto = k == 0 ? n : in_int("Number of terms", 1, n + 1) - 1;
    for (int r = 0; r <= upto; r++) {
        double c = nCr(n, r) * pow(a, n - r) * pow(b, r);
        char t[16] = "";
        if (r) { strcpy(t, "x"); if (r > 1) strcat(t, sup(r)); if (r > 3) { strcpy(t, "x^"); strcat(t, fd(r)); } }
        outf("%sC%d " S_TIMES " %s^%d " S_TIMES " %s^%d = %s", fd(n), r, fxp(a), n - r, fxp(b), r, fcoef(c, t, 1));
    }
    double P[22] = {0};
    for (int r = 0; r <= upto; r++) P[r] = nCr(n, r) * pow(a, n - r) * pow(b, r);
    if (n <= 9 || upto <= 6) outf(ANS "%s%s", fpoly(P, upto, "x"), upto < n ? " + ..." : "");
}

static const calc_t bin_c[] = {
    {"Expand (a + bx)^n", b_expand},
};

/* ======================= TRIGONOMETRY ======================= */
static void t_triangle(void) {
    static const char *o[] = {"SSS (3 sides)", "SAS (2 sides + included angle)", "ASA / AAS (2 angles + side)", "SSA (2 sides + non-included angle)"};
    int k = in_choice("Known", o, 4);
    double a, b, c, A, B, C_;
    const double D = M_PI / 180;
    if (k == 0) {
        a = in_pos("a"), b = in_pos("b"), c = in_pos("c");
        if (a + b <= c || a + c <= b || b + c <= a) fail("Not a triangle");
        A = acos((b * b + c * c - a * a) / (2 * b * c));
        outf("cos A = (b" S_SQ " + c" S_SQ " - a" S_SQ ")/2bc = %s", fx((b * b + c * c - a * a) / (2 * b * c)));
        B = acos((a * a + c * c - b * b) / (2 * a * c));
        C_ = M_PI - A - B;
    } else if (k == 1) {
        a = in_pos("a"), b = in_pos("b"), C_ = in_pos("Angle C (deg)") * D;
        c = sqrt(a * a + b * b - 2 * a * b * cos(C_));
        outf("c" S_SQ " = a" S_SQ " + b" S_SQ " - 2ab cos C = %s", fd(c * c));
        A = asin(fmin(1, a * sin(C_) / c));
        if (a * a > b * b + c * c) A = M_PI - A;
        B = M_PI - A - C_;
    } else if (k == 2) {
        A = in_pos("Angle A (deg)") * D, B = in_pos("Angle B (deg)") * D;
        a = in_pos("Side a (opposite A)");
        C_ = M_PI - A - B;
        if (C_ <= 0) fail("Angles too big");
        outf("C = 180 - A - B = %s", fdeg(C_));
        outf("Sine rule: a/sin A = b/sin B = c/sin C");
        b = a * sin(B) / sin(A); c = a * sin(C_) / sin(A);
    } else {
        a = in_pos("Side a"), b = in_pos("Side b"), A = in_pos("Angle A (opposite a, deg)") * D;
        double sB = b * sin(A) / a;
        outf("sin B = b sin A / a = %s", fd(sB));
        if (sB > 1 + 1e-12) fail("sin B > 1: no triangle");
        double B1 = asin(fmin(1, sB)), B2 = M_PI - B1;
        int two = (B2 + A < M_PI - 1e-9) && fabs(B1 - B2) > 1e-9;
        for (int i = 0; i < (two ? 2 : 1); i++) {
            B = i ? B2 : B1;
            C_ = M_PI - A - B;
            c = a * sin(C_) / sin(A);
            outf(ANS "%s: B = %s, C = %s, c = %s", two ? (i ? "Case 2" : "Case 1") : "Solution", fdeg(B), fdeg(C_), fd(c));
            outf("   Area = %s", fd(0.5 * a * b * sin(C_)));
        }
        if (two) outf("Ambiguous case: two triangles");
        return;
    }
    outf(ANS "a = %s, b = %s, c = %s", fd(a), fd(b), fd(c));
    outf(ANS "A = %s, B = %s, C = %s", fdeg(A), fdeg(B), fdeg(C_));
    outf(ANS "Area = (1/2)ab sin C = %s", fd(0.5 * a * b * sin(C_)));
}

static void t_quadtrig(void) {
    in_note("a f" S_SQ " + b f + c = 0 where f = sin x, cos x or tan x");
    static const char *o[] = {"sin x", "cos x", "tan x"};
    int f = in_choice("f", o, 3);
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    double lo = in_real("x from (deg)"), hi = in_real("x to (deg)");
    double q[3] = {c, b, a};
    cx r[2];
    int n = proots_r(q, a != 0 ? 2 : 1, r);
    outf("Solve %s = 0 as a quadratic in %s", fpoly(q, 2, f == 0 ? "s" : f == 1 ? "c" : "t"), o[f]);
    char *all = sbuf();
    for (int i = 0; i < n; i++) {
        if (r[i].im != 0) continue;
        double v = r[i].re;
        outf("%s = %s%s", o[f], fx(v), (f < 2 && fabs(v) > 1) ? " (no solutions)" : "");
        if (f < 2 && fabs(v) > 1) continue;
        double p = (f == 0 ? asin(v) : f == 1 ? acos(v) : atan(v)) * 180 / M_PI;
        for (int k = -10; k <= 10; k++) {
            double xs[2];
            int m = 0;
            if (f == 0) { xs[m++] = p + 360 * k; xs[m++] = 180 - p + 360 * k; }
            else if (f == 1) { xs[m++] = p + 360 * k; xs[m++] = -p + 360 * k; }
            else xs[m++] = p + 180 * k;
            for (int j = 0; j < m; j++)
                if (xs[j] >= lo - 1e-9 && xs[j] <= hi + 1e-9 && !strstr(all, fd(xs[j]))) {
                    if (all[0]) strcat(all, ", ");
                    strcat(all, fd(xs[j]));
                }
        }
    }
    outf(ANS "x = %s", all[0] ? all : "no solutions in range");
}

static void t_arc(void) {
    double r = in_pos("Radius r"), t = in_pos("Angle " S_TH " (radians)");
    outf(ANS "Arc length r" S_TH " = %s", fxd(r * t));
    outf(ANS "Sector area (1/2)r" S_SQ S_TH " = %s", fxd(0.5 * r * r * t));
    outf("Segment area (1/2)r" S_SQ "(" S_TH " - sin" S_TH ") = %s", fxd(0.5 * r * r * (t - sin(t))));
    outf("Perimeter of sector = %s", fxd(2 * r + r * t));
}

static const calc_t trig_c[] = {
    {"Solve a triangle (sine/cosine rule)", t_triangle},
    {"Trig equations in a range", t_general},
    {"Quadratic in sin/cos/tan", t_quadtrig},
    {"Arc length & sector area", t_arc},
};

/* ======================= EXPONENTIALS & LOGS ======================= */
static void e_solve(void) {
    in_note("a" S_TIMES "b^(px + q) = c");
    double a = in_real("a"), b = in_pos("b (base)"), p = in_real("p"), q = in_real("q"), c = in_real("c");
    if (c / a <= 0) fail("c/a must be positive");
    outf("b^(px + q) = %s", fx(c / a));
    outf("Take logs: (px + q) log b = log(%s)", fx(c / a));
    double v = log(c / a) / log(b);
    outf("px + q = %s", fxd(v));
    outf(ANS "x = %s", fxd((v - q) / p));
}

static void e_model(void) {
    static const char *o[] = {"y = ab^x (log y vs x)", "y = ax^n (log y vs log x)", "y = Ae^(kx)"};
    int k = in_choice("Model", o, 3);
    double x[2], y[2];
    in_vec("Point 1 (x, y)", x, 2);
    in_vec("Point 2 (x, y)", y, 2);
    double x1 = x[0], y1 = x[1], x2 = y[0], y2 = y[1];
    if (y1 <= 0 || y2 <= 0) fail("y must be positive");
    if (k == 0 || k == 2) {
        double m = (log10(y2) - log10(y1)) / (x2 - x1), c = log10(y1) - m * x1;
        outf("log y = x log b + log a: straight line");
        outf("gradient = %s, intercept = %s", fd(m), fd(c));
        double a = pow(10, c), b = pow(10, m);
        if (k == 0) outf(ANS "a = %s, b = %s", fd(a), fd(b));
        else outf(ANS "A = %s, k = ln b = %s", fd(a), fd(log(b)));
    } else {
        if (x1 <= 0 || x2 <= 0) fail("x must be positive");
        double n = (log10(y2) - log10(y1)) / (log10(x2) - log10(x1)), c = log10(y1) - n * log10(x1);
        outf("log y = n log x + log a");
        outf(ANS "n = %s, a = %s", fd(n), fd(pow(10, c)));
    }
}

static void e_laws(void) {
    double a = in_pos("Value inside log"), b = in_pos("Base");
    outf(ANS "log_%s(%s) = %s", fx(b), fx(a), fxd(log(a) / log(b)));
    outf("ln %s = %s,  log10 %s = %s", fx(a), fd(log(a)), fx(a), fd(log10(a)));
    outf("Change of base: log_b a = ln a / ln b");
}

static const calc_t exp_c[] = {
    {"Solve a" S_TIMES "b^(px+q) = c", e_solve},
    {"Fit y = ab^x or y = ax^n", e_model},
    {"Evaluate a logarithm", e_laws},
};

/* ======================= DIFFERENTIATION ======================= */
static void dd_derive(void) {
    node *f = in_fn("y = f(x)", 'x');
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate that");
    node *d2 = ex_diff(d, 'x');
    outf(ANS "dy/dx = %s", ex_str(d));
    if (d2) outf(ANS "d" S_SQ "y/dx" S_SQ " = %s", ex_str(d2));
    static const char *o[] = {"Done", "Gradient, tangent & normal at x"};
    if (in_choice("More?", o, 2)) {
        double x = in_real("x =");
        double y = ex_evalr(f, 'x', x), m = ex_evalr(d, 'x', x);
        outf("y = %s, gradient = %s", fx(y), fx(m));
        outf(ANS "Tangent: %s", line_eq(m, y - m * x));
        if (fabs(m) > 1e-12) outf(ANS "Normal: %s", line_eq(-1 / m, y + x / m));
        else outf(ANS "Normal: x = %s", fx(x));
    }
}

static void dd_stationary(void) {
    node *f = in_fn("y = f(x)", 'x');
    double lo = in_real("Search x from"), hi = in_real("to");
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate that");
    node *d2 = ex_diff(d, 'x');
    outf("dy/dx = %s = 0", ex_str(d));
    double r[20];
    int n = nsolve(d, 'x', lo, hi, r, 20);
    if (!n) { outf(ANS "No stationary points in range"); return; }
    for (int i = 0; i < n; i++) {
        double x = r[i], y = ex_evalr(f, 'x', x), s = d2 ? ex_evalr(d2, 'x', x) : nderiv(d, 'x', x);
        const char *nat = s > 1e-9 ? "minimum" : s < -1e-9 ? "maximum" : "check: d" S_SQ "y/dx" S_SQ " = 0";
        if (fabs(s) <= 1e-9) {
            double l = ex_evalr(d, 'x', x - 1e-4), rr = ex_evalr(d, 'x', x + 1e-4);
            if ((l > 0) == (rr > 0)) nat = "point of inflection";
            else nat = l < 0 ? "minimum" : "maximum";
        }
        outf(ANS "(%s, %s): %s", fx(x), fx(y), nat);
        outf("   d" S_SQ "y/dx" S_SQ " = %s", fx(s));
    }
}

static void dd_incr(void) {
    node *f = in_fn("y = f(x)", 'x');
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate that");
    double lo = in_real("x from"), hi = in_real("to");
    outf("dy/dx = %s", ex_str(d));
    double r[20];
    int n = nsolve(d, 'x', lo, hi, r, 20);
    double pts[22];
    pts[0] = lo;
    for (int i = 0; i < n; i++) pts[i + 1] = r[i];
    pts[n + 1] = hi;
    for (int i = 0; i <= n; i++) {
        double m = (pts[i] + pts[i + 1]) / 2, v = ex_evalr(d, 'x', m);
        outf(ANS "%s < x < %s: %s", fx(pts[i]), fx(pts[i + 1]), v > 0 ? "increasing" : "decreasing");
    }
}

static void dd_first(void) {
    in_note("Differentiation from first principles: (f(x+h) - f(x))/h");
    node *f = in_fn("f(x)", 'x');
    double x = in_real("At x =");
    for (int k = 1; k <= 5; k++) {
        double h = pow(10, -k);
        outf("h = %s: %s", fd(h), fdn((ex_evalr(f, 'x', x + h) - ex_evalr(f, 'x', x)) / h, 8));
    }
    node *d = ex_diff(f, 'x');
    if (d) outf(ANS "Limit = f'(%s) = %s", fx(x), fx(ex_evalr(d, 'x', x)));
}

static const calc_t diff_c[] = {
    {"Differentiate + tangent/normal", dd_derive},
    {"Stationary points & nature", dd_stationary},
    {"Increasing / decreasing", dd_incr},
    {"First principles (table)", dd_first},
};

/* ======================= INTEGRATION ======================= */
static int poly_of(node *f, double *c, int *deg) {
    ser s;
    const char *err;
    if (!ex_series(f, 'x', 0, &s, &err)) return 0;
    for (int k = 9; k <= SER_N; k++) if (fabs(s.c[k]) > 1e-12) return 0;
    for (int k = 0; k <= 8; k++) c[k] = s.c[k];
    for (double t = -2.3; t < 3; t += 1.7) if (fabs(peval_r(c, 8, t) - ex_evalr(f, 'x', t)) > 1e-8 * (1 + fabs(peval_r(c, 8, t)))) return 0;
    *deg = 8;
    while (*deg > 0 && fabs(c[*deg]) < 1e-12) (*deg)--;
    return 1;
}

static void in_indef(void) {
    node *f = in_fn("f(x)", 'x');
    double c[10], q[10] = {0};
    int d;
    if (poly_of(f, c, &d)) {
        for (int k = 0; k <= d; k++) q[k + 1] = c[k] / (k + 1);
        outf("Add 1 to each power, divide by the new power:");
        outf(ANS S_INT " f(x) dx = %s + c", fpoly(q, d + 1, "x"));
    } else {
        outf("Not a polynomial: only definite integrals here");
    }
    static const char *o[] = {"Done", "Evaluate between limits"};
    if (in_choice("More?", o, 2)) {
        double a = in_real("Lower limit"), b = in_real("Upper limit");
        int ok;
        double I = integrate(f, 'x', a, b, &ok);
        if (poly_of(f, c, &d)) outf("[F(x)] = %s - %s", fx(peval_r(q, d + 1, b)), fxp(peval_r(q, d + 1, a)));
        outf(ANS "= %s", fxd(I));
    }
}

static void in_area(void) {
    static const char *o[] = {"Between curve and x-axis", "Between two curves"};
    int k = in_choice("Area", o, 2);
    node *f = in_fn(k ? "Upper/first curve y = f(x)" : "y = f(x)", 'x');
    node *g = NULL;
    if (k) g = in_fn("Second curve y = g(x)", 'x');
    static const char *lo[] = {"Enter limits", "Use the intersections/roots"};
    int L = in_choice("Limits", lo, 2);
    double a, b;
    node *h = f;
    if (k) {
        char s[240];
        const char *err;
        strcpy(s, "("); strcat(s, ex_src(f)); strcat(s, ")-("); strcat(s, ex_src(g)); strcat(s, ")");
        h = ex_parse(s, &err);
        if (!h) fail("Too long");
    }
    double roots[20];
    int nr;
    if (L == 1) {
        double sl = in_real("Search from x ="), sh = in_real("to x =");
        nr = nsolve(h, 'x', sl, sh, roots, 20);
        if (nr < 2) fail("Need two crossing points in that range");
        a = roots[0]; b = roots[nr - 1];
        outf("Crossings at x = %s ... %s", fx(a), fx(b));
    } else { a = in_real("x from"), b = in_real("x to"); nr = nsolve(h, 'x', a, b, roots, 20); }
    double pts[24];
    int np = 0;
    pts[np++] = a;
    for (int i = 0; i < nr; i++) if (roots[i] > a + 1e-9 && roots[i] < b - 1e-9) pts[np++] = roots[i];
    pts[np++] = b;
    double tot = 0;
    for (int i = 0; i + 1 < np; i++) {
        int ok;
        double I = integrate(h, 'x', pts[i], pts[i + 1], &ok);
        outf(S_INT " from %s to %s = %s", fx(pts[i]), fx(pts[i + 1]), fxd(I));
        tot += fabs(I);
    }
    if (np > 2) outf("Parts below the axis are counted as positive");
    outf(ANS "Area = %s", fxd(tot));
}

static const calc_t int_c[] = {
    {"Integrate (indefinite / definite)", in_indef},
    {"Area under / between curves", in_area},
};

/* ======================= VECTORS ======================= */
static void vv_2d(void) {
    double a[2];
    in_vec("Vector (x, y)", a, 2);
    double m = hypot(a[0], a[1]);
    outf(ANS "|a| = %s", fxd(m));
    outf(ANS "Direction = %s from the positive x-axis", fdeg(atan2(a[1], a[0])));
    if (m > 0) { double u[2] = {a[0] / m, a[1] / m}; outf("Unit vector = %s", fvec(u, 2)); }
    static const char *o[] = {"Done", "Distance/vector between two points"};
    if (in_choice("More?", o, 2)) {
        double P[2], Q[2];
        in_vec("Point A", P, 2);
        in_vec("Point B", Q, 2);
        double d[2] = {Q[0] - P[0], Q[1] - P[1]};
        outf(ANS "AB = %s, |AB| = %s", fvec(d, 2), fxd(hypot(d[0], d[1])));
    }
}

static const calc_t vec_c[] = {
    {"Magnitude & direction (2D)", vv_2d},
};

const topic_t m_u1[] = {
    {"Algebra & polynomials", alg_c, NEL(alg_c)},
    {"Coordinate geometry & circles", geo_c, NEL(geo_c)},
    {"Binomial expansion", bin_c, NEL(bin_c)},
    {"Trigonometry", trig_c, NEL(trig_c)},
    {"Exponentials & logarithms", exp_c, NEL(exp_c)},
    {"Differentiation", diff_c, NEL(diff_c)},
    {"Integration", int_c, NEL(int_c)},
    {"Vectors", vec_c, NEL(vec_c)},
};
const int m_u1_n = NEL(m_u1);
