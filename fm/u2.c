/* Further Maths Unit 2: Further Statistics A */
#include "../common/core.h"
#include "../common/util.h"
#include "../common/stats.h"
#include <string.h>

#define MAXN 60

static const char *f4(double x) { return fdp(x, 4); }

static double in_prob(const char *l) {
    double p = in_real(l);
    if (p < 0 || p > 1) fail("Probability must be 0 to 1");
    return p;
}

static int in_tail(void) {
    static const char *o[] = {"H1: " S_RHO " > 0 (one-tailed)", "H1: " S_RHO " < 0 (one-tailed)", "H1: " S_RHO " " S_NE " 0 (two-tailed)"};
    return in_choice("Alternative hypothesis", o, 3);
}

static double in_alpha(void) {
    static const char *o[] = {"10%", "5%", "2.5%", "1%", "0.5%"};
    static const double v[] = {0.10, 0.05, 0.025, 0.01, 0.005};
    return v[in_choice("Significance level", o, 5)];
}

/* ======================= DISCRETE RV ======================= */
void d_table(void) {
    double x[MAXN], p[MAXN];
    int n = in_list("Values of x", x, MAXN);
    int m = in_list("P(X = x) (or frequencies)", p, MAXN);
    if (m != n) fail("Lists must be the same length");
    double sp = 0;
    for (int i = 0; i < n; i++) sp += p[i];
    if (fabs(sp - 1) > 1e-9) { outf("Probabilities sum to %s: treating as frequencies", fx(sp)); for (int i = 0; i < n; i++) p[i] /= sp; }
    double E = 0, E2 = 0;
    for (int i = 0; i < n; i++) { E += x[i] * p[i]; E2 += x[i] * x[i] * p[i]; }
    outf("E(X) = " S_SIG "xP(X = x)");
    outf(ANS "E(X) = %s", fxd(E));
    outf("E(X" S_SQ ") = " S_SIG "x" S_SQ "P(X = x) = %s", fxd(E2));
    outf("Var(X) = E(X" S_SQ ") - [E(X)]" S_SQ " = %s - %s", fx(E2), fxp(E * E));
    outf(ANS "Var(X) = %s", fxd(E2 - E * E));
    outf("SD = %s", fd(sqrt(E2 - E * E)));
    static const char *o[] = {"Done", "Also E(g(X)) for a function g"};
    if (in_choice("More?", o, 2)) {
        node *g = in_fn("g(x)", 'x');
        double Eg = 0, Eg2 = 0;
        for (int i = 0; i < n; i++) { double v = ex_evalr(g, 'x', x[i]); Eg += v * p[i]; Eg2 += v * v * p[i]; }
        outf(ANS "E(g(X)) = %s", fxd(Eg));
        outf(ANS "Var(g(X)) = %s", fxd(Eg2 - Eg * Eg));
    }
}

static void d_lincomb(void) {
    in_note("X, Y independent. Find aX + bY + c");
    double ex = in_real("E(X)"), vx = in_real("Var(X)"), ey = in_real("E(Y)"), vy = in_real("Var(Y)");
    double a = in_real("a"), b = in_real("b"), c = in_real("c");
    outf("E(aX + bY + c) = aE(X) + bE(Y) + c");
    double E = a * ex + b * ey + c, V = a * a * vx + b * b * vy;
    outf(ANS "E = %s", fx(E));
    outf("Var(aX + bY + c) = a" S_SQ "Var(X) + b" S_SQ "Var(Y)");
    outf("= %s(%s) + %s(%s)", fx(a * a), fx(vx), fx(b * b), fx(vy));
    outf(ANS "Var = %s", fx(V));
    outf("Note: X1 + X2 + ... + Xn has variance n" S_SG S_SQ ",");
    outf("but nX has variance n" S_SQ S_SG S_SQ);
    static const char *o[] = {"Done", "X, Y normal: find a probability"};
    if (in_choice("More?", o, 2)) {
        double k = in_real("P(aX + bY + c > k), k =");
        double z = (k - E) / sqrt(V);
        outf("z = (%s - %s)/" S_SQRT "%s = %s", fx(k), fx(E), fx(V), f4(z));
        outf(ANS "P(> k) = %s", f4(1 - norm_cdf(z)));
        outf(ANS "P(< k) = %s", f4(norm_cdf(z)));
    }
}

static const calc_t disc_c[] = {
    {"E(X), Var(X) from a table", d_table},
    {"Linear combinations aX + bY", d_lincomb},
};

/* ======================= CONTINUOUS RV ======================= */
static node *pdf;
static double pa_, pb_;
static double cdf_minus_half(double m) { int ok; return integrate(pdf, 'x', pa_, m, &ok) - 0.5; }

static void c_pdf(void) {
    pdf = in_fn("f(x)", 'x');
    double a = in_real("f defined from x ="), b = in_real("to x =");
    pa_ = a; pb_ = b;
    int ok;
    double I = integrate(pdf, 'x', a, b, &ok);
    outf(S_INT " f(x) dx = %s %s", fxd(I), fabs(I - 1) < 1e-6 ? "(valid pdf)" : "(NOT 1: not a valid pdf)");
    char s[240];
    const char *err;
    strcpy(s, "x("); strcat(s, ex_src(pdf)); strcat(s, ")");
    node *xf = ex_parse(s, &err);
    strcpy(s, "x^2("); strcat(s, ex_src(pdf)); strcat(s, ")");
    node *x2f = ex_parse(s, &err);
    if (!xf || !x2f) fail("Too long");
    double E = integrate(xf, 'x', a, b, &ok), E2 = integrate(x2f, 'x', a, b, &ok);
    outf("E(X) = " S_INT " x f(x) dx");
    outf(ANS "E(X) = %s", fxd(E));
    outf("E(X" S_SQ ") = %s", fxd(E2));
    outf(ANS "Var(X) = %s", fxd(E2 - E * E));
    double m[3];
    int n = nsolve_fn(cdf_minus_half, a, b, m, 3);
    if (n) { outf("Median: F(m) = 0.5"); outf(ANS "m = %s", fxd(m[0])); }
    /* mode: max of f */
    double best = a, bv = -1e300;
    for (int i = 0; i <= 2000; i++) { double x = a + (b - a) * i / 2000, v = ex_evalr(pdf, 'x', x); if (v > bv) { bv = v; best = x; } }
    outf("Mode " S_APX " %s", fd(best));
    static const char *o[] = {"Done", "P(c < X < d)", "F(x) at a value"};
    int k = in_choice("More?", o, 3);
    if (k == 1) {
        double c = in_real("c"), d = in_real("d");
        outf(ANS "P(%s < X < %s) = %s", fx(c), fx(d), fxd(integrate(pdf, 'x', fmax(a, c), fmin(b, d), &ok)));
    } else if (k == 2) {
        double x = in_real("x");
        outf(ANS "F(%s) = %s", fx(x), fxd(x <= a ? 0 : x >= b ? 1 : integrate(pdf, 'x', a, x, &ok)));
    }
}

static void c_findk(void) {
    in_note("f(x) = k g(x): enter g(x)");
    node *g = in_fn("g(x)", 'x');
    double a = in_real("From x ="), b = in_real("To x =");
    int ok;
    double I = integrate(g, 'x', a, b, &ok);
    outf("k " S_INT " g(x) dx = 1,  " S_INT " g(x) dx = %s", fxd(I));
    outf(ANS "k = %s", fxd(1 / I));
}

static void c_uniform(void) {
    double a = in_real("a"), b = in_real("b");
    if (b <= a) fail("Need b > a");
    outf("f(x) = 1/(b - a) = %s on [%s, %s]", fx(1 / (b - a)), fx(a), fx(b));
    outf(ANS "E(X) = (a + b)/2 = %s", fx((a + b) / 2));
    outf(ANS "Var(X) = (b - a)" S_SQ "/12 = %s", fx((b - a) * (b - a) / 12));
    double c = in_real("P(X < c), c =");
    outf(ANS "P(X < %s) = %s", fx(c), fx(c <= a ? 0 : c >= b ? 1 : (c - a) / (b - a)));
}

static const calc_t cont_c[] = {
    {"pdf: E, Var, median, mode", c_pdf},
    {"Find k in a pdf", c_findk},
    {"Continuous uniform", c_uniform},
};

/* ======================= POISSON & EXPONENTIAL ======================= */
void p_poisson(void) {
    double l = in_pos(S_LA " (mean)");
    outf("Mean = Variance = %s", fx(l));
    static const char *o[] = {"P(X = r)", "P(X " S_LE " r)", "P(X " S_GE " r)", "P(a " S_LE " X " S_LE " b)", "Table r = 0..10"};
    int k = in_choice("Find", o, 5);
    if (k == 4) {
        for (int r = 0; r <= 10; r++) outf("r=%d: P = %s, cum = %s", r, f4(pois_pmf(r, l)), f4(pois_cdf(r, l)));
        return;
    }
    if (k == 3) {
        int a = in_int("a", 0, 10000), b = in_int("b", 0, 10000);
        outf("P(X " S_LE " %d) - P(X " S_LE " %d)", b, a - 1);
        outf(ANS "= %s", f4(pois_cdf(b, l) - (a > 0 ? pois_cdf(a - 1, l) : 0)));
        return;
    }
    int r = in_int("r", 0, 10000);
    if (k == 0) {
        outf("P(X = r) = e^-" S_LA S_LA "^r/r!");
        outf("= e^-%s(%s)^%d/%d!", fx(l), fx(l), r, r);
        outf(ANS "= %s", f4(pois_pmf(r, l)));
    } else if (k == 1) outf(ANS "P(X " S_LE " %d) = %s", r, f4(pois_cdf(r, l)));
    else { outf("1 - P(X " S_LE " %d)", r - 1); outf(ANS "P(X " S_GE " %d) = %s", r, f4(1 - (r > 0 ? pois_cdf(r - 1, l) : 0))); }
}

static void p_exp(void) {
    double l = in_pos("Rate " S_LA);
    outf("f(x) = " S_LA "e^(-" S_LA "x), F(x) = 1 - e^(-" S_LA "x)");
    outf(ANS "Mean = 1/" S_LA " = %s, Var = 1/" S_LA S_SQ " = %s", fx(1 / l), fx(1 / (l * l)));
    outf(ANS "Median = ln2/" S_LA " = %s", fd(log(2) / l));
    static const char *o[] = {"P(X < x)", "P(X > x)", "P(a < X < b)"};
    int k = in_choice("Find", o, 3);
    if (k < 2) {
        double x = in_real("x");
        double F = 1 - exp(-l * x);
        if (k == 0) outf(ANS "P(X < %s) = 1 - e^(%s) = %s", fx(x), fx(-l * x), f4(F));
        else outf(ANS "P(X > %s) = e^(%s) = %s", fx(x), fx(-l * x), f4(1 - F));
    } else {
        double a = in_real("a"), b = in_real("b");
        outf(ANS "= e^(%s) - e^(%s) = %s", fx(-l * a), fx(-l * b), f4(exp(-l * a) - exp(-l * b)));
    }
    outf("Memoryless: P(X > s + t | X > s) = P(X > t)");
}

static void p_sum(void) {
    in_note("Sum of independent Poissons is Poisson");
    double a = in_pos(S_LA "1"), b = in_pos(S_LA "2");
    int r = in_int("P(X + Y = r), r =", 0, 10000);
    outf(S_LA " = %s + %s = %s", fx(a), fx(b), fx(a + b));
    outf(ANS "P = %s", f4(pois_pmf(r, a + b)));
    outf(ANS "P(" S_LE " r) = %s", f4(pois_cdf(r, a + b)));
}

static void p_approx(void) {
    int n = in_int("Binomial n", 1, 100000);
    double p = in_prob("p");
    outf("n large, p small: B(n, p) " S_APX " Po(np)");
    outf(S_LA " = np = %s", fx(n * p));
    int r = in_int("P(X " S_LE " r), r =", 0, 100000);
    outf(ANS "Poisson approx: %s", f4(pois_cdf(r, n * p)));
    outf("Exact binomial: %s", f4(binom_cdf(r, n, p)));
}

static const calc_t pois_c[] = {
    {"Poisson probabilities", p_poisson},
    {"Exponential distribution", p_exp},
    {"Sum of Poisson variables", p_sum},
    {"Poisson approx to binomial", p_approx},
};

/* ======================= CORRELATION & REGRESSION ======================= */
static int read_xy(double *x, double *y) {
    int n = in_list("x values", x, MAXN);
    int m = in_list("y values", y, MAXN);
    if (n != m) fail("Lists must be the same length");
    if (n < 3) fail("Need at least 3 pairs");
    return n;
}

static void sums(const double *x, const double *y, int n, double *sxx, double *syy, double *sxy, double *mx, double *my) {
    double Sx = 0, Sy = 0, Sx2 = 0, Sy2 = 0, Sxy = 0;
    for (int i = 0; i < n; i++) { Sx += x[i]; Sy += y[i]; Sx2 += x[i] * x[i]; Sy2 += y[i] * y[i]; Sxy += x[i] * y[i]; }
    outf(S_SIG "x = %s, " S_SIG "y = %s, n = %d", fx(Sx), fx(Sy), n);
    outf(S_SIG "x" S_SQ " = %s, " S_SIG "y" S_SQ " = %s, " S_SIG "xy = %s", fx(Sx2), fx(Sy2), fx(Sxy));
    *sxx = Sx2 - Sx * Sx / n; *syy = Sy2 - Sy * Sy / n; *sxy = Sxy - Sx * Sy / n;
    *mx = Sx / n; *my = Sy / n;
    outf("Sxx = %s, Syy = %s, Sxy = %s", fd(*sxx), fd(*syy), fd(*sxy));
}

static void conclude(double stat, double crit, int tail, const char *what) {
    int rej = tail == 0 ? stat > crit : tail == 1 ? stat < -crit : fabs(stat) > crit;
    outf("Critical value: %s%s", tail == 2 ? S_PM : tail == 1 ? "-" : "", f4(crit));
    outf(ANS "%s", rej ? "Reject H0: significant evidence of" : "Do not reject H0: no significant evidence of");
    outf(ANS "%s correlation", tail == 0 ? "positive" : tail == 1 ? "negative" : what);
}

void cr_pmcc(void) {
    double x[MAXN], y[MAXN];
    int n = read_xy(x, y);
    double sxx, syy, sxy, mx, my;
    sums(x, y, n, &sxx, &syy, &sxy, &mx, &my);
    double r = sxy / sqrt(sxx * syy);
    outf("r = Sxy/" S_SQRT "(SxxSyy)");
    outf(ANS "r = %s", f4(r));
    int tail = in_tail();
    double al = in_alpha();
    double crit = pmcc_crit(n, tail == 2 ? al / 2 : al);
    outf("H0: " S_RHO " = 0, n = %d", n);
    conclude(r, crit, tail, "a");
    outf("(Assumes bivariate normal data)");
}

static void rank_of(const double *v, int n, double *rk) {
    for (int i = 0; i < n; i++) {
        int less = 0, eq = 0;
        for (int j = 0; j < n; j++) { if (v[j] > v[i]) less++; else if (v[j] == v[i]) eq++; }
        rk[i] = less + (eq + 1) / 2.0; /* rank 1 = largest; ties averaged */
    }
}

static void cr_spearman(void) {
    double x[MAXN], y[MAXN], rx[MAXN], ry[MAXN];
    int n = read_xy(x, y);
    rank_of(x, n, rx);
    rank_of(y, n, ry);
    double d2 = 0;
    int ties = 0;
    outf("Ranks (1 = largest), d = difference:");
    for (int i = 0; i < n; i++) {
        double d = rx[i] - ry[i];
        d2 += d * d;
        if (rx[i] != floor(rx[i]) || ry[i] != floor(ry[i])) ties = 1;
        if (i < 15) outf("  %s: %s | %s: %s  d" S_SQ " = %s", fx(x[i]), fx(rx[i]), fx(y[i]), fx(ry[i]), fx(d * d));
    }
    outf(S_SIG "d" S_SQ " = %s", fx(d2));
    double rs = 1 - 6 * d2 / (n * ((double)n * n - 1));
    outf("rs = 1 - 6" S_SIG "d" S_SQ "/(n(n" S_SQ " - 1))");
    if (ties) {
        double sxx, syy, sxy, mx, my;
        outf("Tied ranks: use PMCC of the ranks");
        sums(rx, ry, n, &sxx, &syy, &sxy, &mx, &my);
        rs = sxy / sqrt(sxx * syy);
    }
    outf(ANS "rs = %s", f4(rs));
    int tail = in_tail();
    double al = in_alpha();
    int exact;
    double pu = spearman_p_upper(n, fabs(rs), &exact);
    double pv = tail == 2 ? 2 * pu : ((tail == 0 && rs > 0) || (tail == 1 && rs < 0)) ? pu : 1 - pu;
    if (pv > 1) pv = 1;
    outf("H0: no association, n = %d", n);
    outf("p-value = %s%s", f4(pv), exact ? " (exact)" : " (approx)");
    outf("Use the table critical value for n = %d", n);
    outf(ANS "%s at %s%%", pv < al ? "Reject H0: significant" : "Do not reject H0: not significant", fd(al * 100));
}

void cr_regress(void) {
    double x[MAXN], y[MAXN];
    int n = read_xy(x, y);
    double sxx, syy, sxy, mx, my;
    sums(x, y, n, &sxx, &syy, &sxy, &mx, &my);
    double b = sxy / sxx, a = my - b * mx;
    outf("b = Sxy/Sxx = %s", fd(b));
    outf("a = y-bar - b x-bar = %s - %s(%s)", fd(my), fd(b), fd(mx));
    outf(ANS "y = %s%s", fd(a), fcoef(b, "x", 0));
    outf("r = %s", f4(sxy / sqrt(sxx * syy)));
    double xv = in_real("Predict y at x =");
    outf(ANS "y = %s", fd(a + b * xv));
    double mn = x[0], mxx = x[0];
    for (int i = 1; i < n; i++) { if (x[i] < mn) mn = x[i]; if (x[i] > mxx) mxx = x[i]; }
    outf("%s", xv < mn || xv > mxx ? "Extrapolation: unreliable" : "Interpolation: reasonably reliable");
}

static const calc_t corr_c[] = {
    {"PMCC + hypothesis test", cr_pmcc},
    {"Spearman's rank + test", cr_spearman},
    {"Regression line y on x", cr_regress},
};

/* ======================= CHI-SQUARED ======================= */
static void chi_report(double X2, int df, double al) {
    double crit = chi2_inv(1 - al, df);
    double p = 1 - chi2_cdf(X2, df);
    outf(ANS S_CHI S_SQ " = %s", f4(X2));
    outf("Degrees of freedom = %d", df);
    outf("Critical value (%s%%) = %s", fd(al * 100), f4(crit));
    outf("p-value = %s", f4(p));
    outf(ANS "%s", X2 > crit ? "Reject H0" : "Do not reject H0");
}

void ch_gof(void) {
    double O[MAXN], E[MAXN], xs[MAXN];
    int n = in_list("Observed frequencies", O, MAXN);
    static const char *o[] = {"Expected given", "Uniform", "Poisson", "Binomial"};
    int k = in_choice("Model (H0)", o, 4);
    double tot = 0;
    for (int i = 0; i < n; i++) tot += O[i];
    int est = 0;
    if (k == 0) { int m = in_list("Expected frequencies", E, MAXN); if (m != n) fail("Lists differ in length"); }
    else if (k == 1) for (int i = 0; i < n; i++) E[i] = tot / n;
    else {
        int m = in_list("Values of x (last = 'or more')", xs, MAXN);
        if (m != n) fail("Lists differ in length");
        static const char *pe[] = {"Parameter given", "Estimate from data"};
        est = in_choice("Parameter", pe, 2);
        double mean = 0;
        for (int i = 0; i < n; i++) mean += xs[i] * O[i];
        mean /= tot;
        if (k == 2) {
            double l = est ? mean : in_pos(S_LA);
            if (est) outf(S_LA " = x-bar = %s", fd(l));
            double cum = 0;
            for (int i = 0; i < n; i++) {
                double p = (i == n - 1) ? 1 - cum : pois_pmf((int)xs[i], l);
                cum += p;
                E[i] = tot * p;
            }
        } else {
            int N = in_int("Binomial n", 1, 10000);
            double p = est ? mean / N : in_prob("p");
            if (est) outf("p = x-bar/n = %s", fd(p));
            double cum = 0;
            for (int i = 0; i < n; i++) {
                double q = (i == n - 1) ? 1 - cum : binom_pmf((int)xs[i], N, p);
                cum += q;
                E[i] = tot * q;
            }
        }
    }
    double al = in_alpha();
    /* combine cells with E < 5 from the ends inward */
    double Oc[MAXN], Ec[MAXN];
    int m = 0;
    double ao = 0, ae = 0;
    for (int i = 0; i < n; i++) {
        ao += O[i]; ae += E[i];
        if (ae >= 5) { Oc[m] = ao; Ec[m] = ae; m++; ao = ae = 0; }
    }
    if (ae > 0) { if (m) { Oc[m - 1] += ao; Ec[m - 1] += ae; } else { Oc[m] = ao; Ec[m] = ae; m++; } }
    if (m < n) outf("Cells combined so every E " S_GE " 5: %d cells", m);
    double X2 = 0;
    outf("O, E, (O-E)" S_SQ "/E:");
    for (int i = 0; i < m; i++) {
        double c = (Oc[i] - Ec[i]) * (Oc[i] - Ec[i]) / Ec[i];
        X2 += c;
        outf("  %s, %s, %s", fd(Oc[i]), fd(Ec[i]), f4(c));
    }
    int df = m - 1 - est;
    if (df < 1) fail("No degrees of freedom left");
    outf("df = cells - 1%s = %d", est ? " - 1 (estimated)" : "", df);
    chi_report(X2, df, al);
    outf("H0: the model fits the data");
}

static void ch_cont(void) {
    int r = in_int("Number of rows", 2, 6), c = in_int("Number of columns", 2, 6);
    double O[36];
    for (int i = 0; i < r; i++) {
        char lab[24] = "Row ";
        strcat(lab, fd(i + 1));
        double v[MAXN];
        int k = in_list(lab, v, MAXN);
        if (k != c) fail("Wrong number of values in row");
        for (int j = 0; j < c; j++) O[i * c + j] = v[j];
    }
    double al = in_alpha();
    double R_[6] = {0}, Cc[6] = {0}, T = 0;
    for (int i = 0; i < r; i++) for (int j = 0; j < c; j++) { R_[i] += O[i * c + j]; Cc[j] += O[i * c + j]; T += O[i * c + j]; }
    outf("E = row total " S_TIMES " column total / grand total");
    double X2 = 0;
    int small = 0;
    for (int i = 0; i < r; i++) {
        char *ln = sbuf();
        strcpy(ln, "E: ");
        for (int j = 0; j < c; j++) {
            double E = R_[i] * Cc[j] / T;
            if (E < 5) small = 1;
            X2 += (O[i * c + j] - E) * (O[i * c + j] - E) / E;
            if (j) strcat(ln, ", ");
            strcat(ln, fd(E));
        }
        out_line(ln);
    }
    if (small) outf("Warning: some E < 5, combine rows/columns");
    int df = (r - 1) * (c - 1);
    chi_report(X2, df, al);
    outf("H0: no association between the factors");
}

static const calc_t chi_c[] = {
    {"Goodness of fit", ch_gof},
    {"Contingency table", ch_cont},
};

const topic_t fm_u2[] = {
    {"Discrete random variables", disc_c, NEL(disc_c)},
    {"Continuous random variables", cont_c, NEL(cont_c)},
    {"Poisson & exponential", pois_c, NEL(pois_c)},
    {"Correlation & regression", corr_c, NEL(corr_c)},
    {"Chi-squared tests", chi_c, NEL(chi_c)},
};
const int fm_u2_n = NEL(fm_u2);
