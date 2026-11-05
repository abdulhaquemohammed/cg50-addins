/* A Level Maths Unit 3: Pure Mathematics B (A2) */
#include "../common/core.h"
#include "../common/util.h"
#include "../common/shared.h"
#include <string.h>

/* ======================= SEQUENCES & SERIES ======================= */
static void q_arith(void) {
    static const char *o[] = {"Know a and d", "Know two terms"};
    int k = in_choice("Arithmetic: given", o, 2);
    double a, d;
    if (k == 0) { a = in_real("First term a"); d = in_real("Common difference d"); }
    else {
        int p = in_int("Term number p", 1, 1000000); double up = in_real("u_p");
        int q = in_int("Term number q", 1, 1000000); double uq = in_real("u_q");
        if (p == q) fail("Need different terms");
        d = (uq - up) / (q - p); a = up - (p - 1) * d;
        outf("a + %dd = %s, a + %dd = %s", p - 1, fx(up), q - 1, fx(uq));
        outf(ANS "a = %s, d = %s", fx(a), fx(d));
    }
    outf(ANS "u_n = %s%s", fx(a - d), fcoef(d, "n", 0));
    int n = in_int("n", 1, 10000000);
    outf(ANS "u_%d = a + (n-1)d = %s", n, fx(a + (n - 1) * d));
    outf(ANS "S_%d = (n/2)(2a + (n-1)d) = %s", n, fx(n / 2.0 * (2 * a + (n - 1) * d)));
}

static void q_geo(void) {
    static const char *o[] = {"Know a and r", "Know two terms"};
    int k = in_choice("Geometric: given", o, 2);
    double a, r;
    if (k == 0) { a = in_real("First term a"); r = in_real("Common ratio r"); }
    else {
        int p = in_int("Term number p", 1, 10000); double up = in_real("u_p");
        int q = in_int("Term number q", 1, 10000); double uq = in_real("u_q");
        if (p == q || up == 0) fail("Need different, non-zero terms");
        double ratio = uq / up;
        int e = q - p;
        outf("r^%d = %s", e, fx(ratio));
        if (ratio < 0 && e % 2 == 0) fail("No real r");
        r = (ratio < 0 ? -1 : 1) * pow(fabs(ratio), 1.0 / e);
        if (e % 2 == 0) outf("(r could also be %s)", fx(-r));
        a = up / pow(r, p - 1);
        outf(ANS "a = %s, r = %s", fx(a), fx(r));
    }
    int n = in_int("n", 1, 100000);
    outf(ANS "u_%d = ar^(n-1) = %s", n, fxd(a * pow(r, n - 1)));
    double S = fabs(r - 1) < 1e-15 ? a * n : a * (1 - pow(r, n)) / (1 - r);
    outf(ANS "S_%d = a(1 - r^n)/(1 - r) = %s", n, fxd(S));
    if (fabs(r) < 1) outf(ANS "S_" S_INF " = a/(1 - r) = %s", fxd(a / (1 - r)));
    else outf("|r| " S_GE " 1: no sum to infinity");
    static const char *m[] = {"Done", "Smallest n with S_n > k (or u_n < k)"};
    if (in_choice("More?", m, 2)) {
        double K = in_real("k");
        for (int i = 1; i < 100000; i++) {
            double Si = fabs(r - 1) < 1e-15 ? a * i : a * (1 - pow(r, i)) / (1 - r);
            if (Si > K) { outf(ANS "S_n > %s first when n = %d", fx(K), i); break; }
            if (i == 99999) outf("Never exceeds k");
        }
        for (int i = 1; i < 100000; i++) if (a * pow(r, i - 1) < K) { outf(ANS "u_n < %s first when n = %d", fx(K), i); break; }
    }
}

static void q_recur(void) {
    node *f = in_fn("u_(n+1) = f(x), x = u_n", 'x');
    double u = in_real("u_1");
    int n = in_int("Terms", 1, 40);
    double s = 0;
    for (int i = 1; i <= n; i++) {
        outf("u_%d = %s", i, fxd(u));
        s += u;
        u = ex_evalr(f, 'x', u);
    }
    outf(ANS "Sum of %d terms = %s", n, fxd(s));
}

static void q_binom(void) {
    in_note("(a + bx)^n for any rational n");
    double a = in_real("a"), b = in_real("b"), n = in_real("n");
    int k = in_int("Number of terms", 2, 8);
    if (a <= 0 && n != floor(n)) fail("Need a > 0 for non-integer n");
    double an = pow(a, n);
    outf("= %s(1 + %sx)^%s", fx(an), fx(b / a), fx(n));
    double c = 1, P[10] = {0};
    for (int r = 0; r < k; r++) {
        P[r] = an * c * pow(b / a, r);
        c = c * (n - r) / (r + 1);
    }
    outf(ANS "%s + ...", fpoly(P, k - 1, "x"));
    if (n != floor(n) || n < 0) outf(ANS "Valid for |x| < %s", fx(fabs(a / b)));
}

static const calc_t seq_c[] = {
    {"Arithmetic sequences & series", q_arith},
    {"Geometric sequences & series", q_geo},
    {"Recurrence relation terms", q_recur},
    {"Binomial expansion, any n", q_binom},
};

/* ======================= TRIGONOMETRY ======================= */
static void t_eval(void) {
    double x = in_real("x (radians; use " S_PI ")");
    outf("sin x = %s, cos x = %s, tan x = %s", fx(sin(x)), fx(cos(x)), fx(tan(x)));
    outf("sec x = %s", fx(1 / cos(x)));
    outf("cosec x = %s", fx(1 / sin(x)));
    outf("cot x = %s", fx(cos(x) / sin(x)));
    outf("Degrees: %s", fdeg(x));
}

static void t_double(void) {
    in_note("Identities used: sin2x = 2sinx cosx, cos2x = 1 - 2sin" S_SQ "x etc.");
    node *f = in_fn("Equation f(x) = 0, f(x) =", 'x');
    double lo = in_real("x from (deg)"), hi = in_real("to (deg)");
    char s[240];
    strcpy(s, "(");
    strcat(s, ex_src(f));
    strcat(s, ")");
    /* convert degrees: substitute x -> x*pi/180 by evaluating with scaled variable */
    double r[40];
    int n = nsolve(f, 'x', lo * M_PI / 180, hi * M_PI / 180, r, 40);
    char *o = sbuf();
    for (int i = 0; i < n; i++) { if (o[0]) strcat(o, ", "); strcat(o, fd(r[i] * 180 / M_PI)); }
    outf("Solved numerically in the range");
    outf(ANS "x = %s" S_DEG, o[0] ? o : "none");
    if (n) { o = sbuf(); for (int i = 0; i < n; i++) { if (o[0]) strcat(o, ", "); strcat(o, fx(r[i])); } outf("Radians: %s", o); }
}

static const calc_t trig_c[] = {
    {"Solve sin/cos/tan in a range", t_general},
    {"R cos(x - " S_AL ") form", t_rform},
    {"Solve ANY trig equation (numeric)", t_double},
    {"Evaluate sec, cosec, cot", t_eval},
};

/* ======================= DIFFERENTIATION ======================= */
static void d_derive(void) {
    node *f = in_fn("y = f(x)", 'x');
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate that");
    outf("Using chain/product/quotient rules:");
    outf(ANS "dy/dx = %s", ex_str(d));
    node *d2 = ex_diff(d, 'x');
    if (d2) outf("d" S_SQ "y/dx" S_SQ " = %s", ex_str(d2));
    double x = in_real("Evaluate at x =");
    outf(ANS "dy/dx = %s at x = %s", fxd(ex_evalr(d, 'x', x)), fx(x));
}

static void d_implicit(void) {
    in_note("Write as F(x, y) = 0. e.g. x^2+y^2-25 or x^2y+y^3-3");
    node *F = in_fn("F(x, y)", 'x');
    node *Fx = ex_diff(F, 'x'), *Fy = ex_diff(F, 'y');
    if (!Fx || !Fy) fail("Can't differentiate that");
    outf("Differentiate each term w.r.t. x:");
    outf("dy/dx = -(dF/dx)/(dF/dy)  (partial derivatives)");
    outf(ANS "dy/dx = -(%s)/(%s)", ex_str(Fx), ex_str(Fy));
    double P[2];
    in_vec("At point (x, y)", P, 2);
    ex_setvar('x', R(P[0])); ex_setvar('y', R(P[1]));
    double chk = ex_eval(F).re, a = ex_eval(Fx).re, b = ex_eval(Fy).re;
    if (fabs(chk) > 1e-6) outf("Warning: point not on curve (F = %s)", fd(chk));
    if (fabs(b) < 1e-12) { outf(ANS "Vertical tangent: x = %s", fx(P[0])); return; }
    double m = -a / b;
    outf(ANS "Gradient = %s", fx(m));
    outf(ANS "Tangent: y = %s%s", fcoef(m, "x", 1), fcoef(P[1] - m * P[0], "", 0));
    if (fabs(m) > 1e-12) outf(ANS "Normal: y = %s%s", fcoef(-1 / m, "x", 1), fcoef(P[1] + P[0] / m, "", 0));
}

static void d_param(void) {
    node *X = in_fn("x = f(t)", 't'), *Y = in_fn("y = g(t)", 't');
    node *dx = ex_diff(X, 't'), *dy = ex_diff(Y, 't');
    if (!dx || !dy) fail("Can't differentiate");
    outf("dx/dt = %s", ex_str(dx));
    outf("dy/dt = %s", ex_str(dy));
    outf(ANS "dy/dx = (%s)/(%s)", ex_str(dy), ex_str(dx));
    double t = in_real("At t =");
    double x = ex_evalr(X, 't', t), y = ex_evalr(Y, 't', t), a = ex_evalr(dx, 't', t), b = ex_evalr(dy, 't', t);
    outf("Point (%s, %s)", fx(x), fx(y));
    if (fabs(a) < 1e-12) { outf(ANS "Vertical tangent x = %s", fx(x)); return; }
    double m = b / a;
    outf(ANS "Gradient = %s", fx(m));
    outf(ANS "Tangent: y = %s%s", fcoef(m, "x", 1), fcoef(y - m * x, "", 0));
    if (fabs(m) > 1e-12) outf(ANS "Normal: y = %s%s", fcoef(-1 / m, "x", 1), fcoef(y + x / m, "", 0));
}

static void d_rate(void) {
    in_note("Connected rates: dy/dt = dy/dx " S_TIMES " dx/dt");
    node *f = in_fn("y = f(x)", 'x');
    double x = in_real("At x ="), r = in_real("dx/dt =");
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate");
    double m = ex_evalr(d, 'x', x);
    outf("dy/dx = %s = %s", ex_str(d), fx(m));
    outf(ANS "dy/dt = %s " S_TIMES " %s = %s", fx(m), fxp(r), fx(m * r));
}

static const calc_t diff_c[] = {
    {"Differentiate (chain/product/quotient)", d_derive},
    {"Implicit differentiation", d_implicit},
    {"Parametric differentiation", d_param},
    {"Connected rates of change", d_rate},
};

/* ======================= INTEGRATION ======================= */
static void i_trap(void) {
    node *f = in_fn("y = f(x)", 'x');
    double a = in_real("a"), b = in_real("b");
    int n = in_int("Number of strips", 1, 100);
    double h = (b - a) / n, s = 0;
    outf("h = (b - a)/n = %s", fx(h));
    for (int i = 0; i <= n; i++) {
        double x = a + i * h, y = ex_evalr(f, 'x', x);
        if (i <= 10 || i == n) outf("x = %s, y = %s", fd(x), fdn(y, 6));
        s += (i == 0 || i == n) ? y : 2 * y;
    }
    outf("Area " S_APX " (h/2)[y0 + yn + 2(y1 + ... )]");
    outf(ANS "Trapezium estimate = %s", fdn(h / 2 * s, 6));
    int ok;
    double I = integrate(f, 'x', a, b, &ok);
    outf("Exact value = %s", fxd(I));
    outf("%s", h / 2 * s > I ? "Overestimate (curve concave up)" : "Underestimate (curve concave down)");
}

static void i_sep(void) {
    in_note("dy/dx = f(x)g(y): " S_INT " dy/g(y) = " S_INT " f(x) dx");
    node *f = in_fn("dy/dx = f(x, y)", 'x');
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
    }
    outf("Numerical solution (to check your algebra):");
    outf(ANS "y(%s) = %s", fx(x1), fxd(y));
}

static void i_param(void) {
    in_note("Area = " S_INT " y (dx/dt) dt");
    node *X = in_fn("x = f(t)", 't'), *Y = in_fn("y = g(t)", 't');
    double a = in_real("t from"), b = in_real("t to");
    node *dx = ex_diff(X, 't');
    if (!dx) fail("Can't differentiate x(t)");
    node *ig = ex_simplify(ex_bin('*', Y, dx));
    outf("y dx/dt = %s", ex_str(ig));
    int ok;
    double I = integrate(ig, 't', a, b, &ok);
    outf(ANS "Area = %s", fxd(fabs(I)));
}

static const calc_t int_c[] = {
    {"Definite integral (any function)", k_integral},
    {"Trapezium rule", i_trap},
    {"Area for parametric curve", i_param},
    {"Partial fractions", s_partial},
    {"Separable DE (numeric check)", i_sep},
};

/* ======================= NUMERICAL METHODS ======================= */
static void n_sign(void) {
    node *f = in_fn("f(x)", 'x');
    double a = in_real("a"), b = in_real("b");
    double fa = ex_evalr(f, 'x', a), fb = ex_evalr(f, 'x', b);
    outf("f(%s) = %s", fx(a), fd(fa));
    outf("f(%s) = %s", fx(b), fd(fb));
    if ((fa < 0) != (fb < 0)) outf(ANS "Sign change and f continuous: a root lies between %s and %s", fx(a), fx(b));
    else outf(ANS "No sign change (could be 0 or an even number of roots)");
}

static void n_newton(void) {
    node *f = in_fn("f(x)", 'x');
    double x = in_real("x0");
    int n = in_int("Iterations", 1, 20);
    node *d = ex_diff(f, 'x');
    if (!d) fail("Can't differentiate");
    outf("f'(x) = %s", ex_str(d));
    outf("x_(n+1) = x_n - f(x_n)/f'(x_n)");
    for (int i = 1; i <= n; i++) {
        double fv = ex_evalr(f, 'x', x), dv = ex_evalr(d, 'x', x);
        if (fabs(dv) < 1e-15) fail("f'(x) = 0: method fails");
        x = x - fv / dv;
        outf(ANS "x%d = %s", i, fdn(x, 9));
    }
    outf("f(x) = %s", fd(ex_evalr(f, 'x', x)));
}

static void n_fixed(void) {
    node *g = in_fn("x_(n+1) = g(x_n), g(x) =", 'x');
    double x = in_real("x0");
    int n = in_int("Iterations", 1, 30);
    for (int i = 1; i <= n; i++) { x = ex_evalr(g, 'x', x); outf(ANS "x%d = %s", i, fdn(x, 9)); }
    node *d = ex_diff(g, 'x');
    if (d) { double s = ex_evalr(d, 'x', x); outf("|g'(x)| = %s: %s", fd(fabs(s)), fabs(s) < 1 ? "converges" : "diverges"); outf("%s", s > 0 ? "Staircase diagram" : "Cobweb diagram"); }
}

static void n_roots(void) {
    node *f = in_fn("f(x)", 'x');
    double a = in_real("Search from x ="), b = in_real("to x =");
    double r[30];
    int n = nsolve(f, 'x', a, b, r, 30);
    if (!n) outf(ANS "No roots found in range");
    for (int i = 0; i < n; i++) outf(ANS "x = %s", fxd(r[i]));
}

static const calc_t num_c[] = {
    {"Sign change check", n_sign},
    {"Newton-Raphson", n_newton},
    {"Fixed point iteration", n_fixed},
    {"Find all roots in a range", n_roots},
};

/* ======================= FUNCTIONS ======================= */
static void fn_comp(void) {
    node *f = in_fn("f(x)", 'x'), *g = in_fn("g(x)", 'x');
    double x = in_real("x =");
    double gx = ex_evalr(g, 'x', x), fx_ = ex_evalr(f, 'x', x);
    outf("g(%s) = %s,  f(%s) = %s", fx(x), fx(gx), fx(x), fx(fx_));
    outf(ANS "fg(%s) = f(%s) = %s", fx(x), fx(gx), fxd(ex_evalr(f, 'x', gx)));
    outf(ANS "gf(%s) = g(%s) = %s", fx(x), fx(fx_), fxd(ex_evalr(g, 'x', fx_)));
}

static node *invf;
static double invt;
static double invh(double x) { return ex_evalr(invf, 'x', x) - invt; }
static void fn_inv(void) {
    invf = in_fn("f(x)", 'x');
    invt = in_real("Find f^-1(k), k =");
    double lo = in_real("Search x from"), hi = in_real("to");
    double r[5];
    int n = nsolve_fn(invh, lo, hi, r, 5);
    if (!n) outf(ANS "No x in range with f(x) = %s", fx(invt));
    for (int i = 0; i < n; i++) outf(ANS "f^-1(%s) = %s", fx(invt), fxd(r[i]));
    if (n > 1) outf("More than one: f isn't one-to-one on this range");
}

static const calc_t fun_c[] = {
    {"Composite functions fg, gf", fn_comp},
    {"Inverse function value", fn_inv},
};

const topic_t m_u3[] = {
    {"Sequences & series", seq_c, NEL(seq_c)},
    {"Trigonometry", trig_c, NEL(trig_c)},
    {"Differentiation", diff_c, NEL(diff_c)},
    {"Integration", int_c, NEL(int_c)},
    {"Numerical methods", num_c, NEL(num_c)},
    {"Functions", fun_c, NEL(fun_c)},
};
const int m_u3_n = NEL(m_u3);
