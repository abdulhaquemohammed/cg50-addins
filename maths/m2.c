/* A Level Maths Unit 2: Applied Mathematics A (AS) - statistics & mechanics */
#include "../common/core.h"
#include "../common/util.h"
#include "../common/stats.h"
#include "../common/shared.h"
#include <string.h>

#define G 9.8
#define MAXN 60
static const char *f4(double x) { return fdp(x, 4); }

/* ======================= DATA ======================= */
static void sortd(double *v, int n) {
    for (int i = 1; i < n; i++) { double t = v[i]; int j = i - 1; while (j >= 0 && v[j] > t) { v[j + 1] = v[j]; j--; } v[j + 1] = t; }
}
static double med(const double *v, int n) { if (n <= 0) return NAN; return n % 2 ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2; }

static void s_list(void) {
    double v[MAXN];
    int n = in_list("Data values", v, MAXN);
    sortd(v, n);
    double s = 0, s2 = 0;
    for (int i = 0; i < n; i++) { s += v[i]; s2 += v[i] * v[i]; }
    double mean = s / n, var = s2 / n - mean * mean;
    outf("n = %d, " S_SIG "x = %s, " S_SIG "x" S_SQ " = %s", n, fx(s), fx(s2));
    outf(ANS "Mean = %s", fxd(mean));
    outf(ANS "SD = %s (divisor n)", fd(sqrt(var)));
    if (n > 1) outf("SD = %s (divisor n - 1)", fd(sqrt(var * n / (n - 1))));
    if (n < 4) { outf(ANS "Median = %s", fx(med(v, n))); outf("Need 4+ values for quartiles"); return; }
    double Q1 = med(v, n / 2), Q3 = med(v + (n + 1) / 2, n / 2);
    outf(ANS "Median = %s", fx(med(v, n)));
    outf(ANS "Q1 = %s, Q3 = %s, IQR = %s", fx(Q1), fx(Q3), fx(Q3 - Q1));
    outf("Min = %s, Max = %s, Range = %s", fx(v[0]), fx(v[n - 1]), fx(v[n - 1] - v[0]));
    double lo = Q1 - 1.5 * (Q3 - Q1), hi = Q3 + 1.5 * (Q3 - Q1);
    outf("Outlier fences: < %s or > %s", fx(lo), fx(hi));
    char *o = sbuf();
    for (int i = 0; i < n; i++) if (v[i] < lo || v[i] > hi) { if (o[0]) strcat(o, ", "); strcat(o, fx(v[i])); }
    outf("Outliers: %s", o[0] ? o : "none");
    outf("Also mean " S_PM " 2SD: %s to %s", fd(mean - 2 * sqrt(var)), fd(mean + 2 * sqrt(var)));
}

static void s_freq(void) {
    double x[MAXN], f[MAXN];
    int n = in_list("Values (or class midpoints)", x, MAXN);
    int m = in_list("Frequencies", f, MAXN);
    if (n != m) fail("Lists differ in length");
    double N = 0, s = 0, s2 = 0;
    for (int i = 0; i < n; i++) { N += f[i]; s += f[i] * x[i]; s2 += f[i] * x[i] * x[i]; }
    double mean = s / N;
    outf(S_SIG "f = %s, " S_SIG "fx = %s, " S_SIG "fx" S_SQ " = %s", fx(N), fx(s), fx(s2));
    outf(ANS "Mean = %s", fxd(mean));
    outf(ANS "SD = %s", fd(sqrt(s2 / N - mean * mean)));
    static const char *o[] = {"Done", "Median by interpolation (grouped)"};
    if (in_choice("More?", o, 2)) {
        double b[MAXN];
        int k = in_list("Class boundaries (n + 1 values)", b, MAXN);
        if (k != n + 1) fail("Need one more boundary than classes");
        double half = N / 2, cum = 0;
        for (int i = 0; i < n; i++) {
            if (cum + f[i] >= half) {
                double mdn = b[i] + (half - cum) / f[i] * (b[i + 1] - b[i]);
                outf("Median class %s-%s, cum freq before = %s", fx(b[i]), fx(b[i + 1]), fx(cum));
                outf(ANS "Median = %s + (%s - %s)/%s " S_TIMES " %s = %s", fx(b[i]), fx(half), fx(cum), fx(f[i]), fx(b[i + 1] - b[i]), fd(mdn));
                break;
            }
            cum += f[i];
        }
    }
}

static const calc_t data_c[] = {
    {"Summary stats from a list", s_list},
    {"Frequency table / grouped data", s_freq},
};

/* ======================= PROBABILITY ======================= */
static void p_venn(void) {
    double a = in_real("P(A)"), b = in_real("P(B)");
    static const char *o[] = {"Know P(A and B)", "Know P(A or B)", "A and B independent", "A and B mutually exclusive"};
    int k = in_choice("Also know", o, 4);
    double ab, aub;
    if (k == 0) { ab = in_real("P(A and B)"); aub = a + b - ab; }
    else if (k == 1) { aub = in_real("P(A or B)"); ab = a + b - aub; }
    else if (k == 2) { ab = a * b; aub = a + b - ab; outf("Independent: P(A and B) = P(A)P(B)"); }
    else { ab = 0; aub = a + b; }
    outf(ANS "P(A and B) = %s", fx(ab));
    outf(ANS "P(A or B) = %s", fx(aub));
    outf("P(A only) = %s, P(B only) = %s", fx(a - ab), fx(b - ab));
    outf("P(neither) = %s", fx(1 - aub));
    if (b > 0) outf("P(A | B) = %s", fx(ab / b));
    if (a > 0) outf("P(B | A) = %s", fx(ab / a));
    outf("%s", fabs(ab - a * b) < 1e-12 ? "P(A)P(B) = P(A and B): independent" : "P(A)P(B) " S_NE " P(A and B): not independent");
    if (fabs(ab) < 1e-12) outf("P(A and B) = 0: mutually exclusive");
}

static void p_binom(void) {
    int n = in_int("n", 1, 100000);
    double p = in_real("p");
    if (p < 0 || p > 1) fail("p must be 0 to 1");
    outf("X ~ B(%d, %s): mean np = %s, var np(1-p) = %s", n, fx(p), fx(n * p), fx(n * p * (1 - p)));
    static const char *o[] = {"P(X = r)", "P(X " S_LE " r)", "P(X " S_GE " r)", "P(a " S_LE " X " S_LE " b)"};
    int k = in_choice("Find", o, 4);
    if (k == 3) {
        int a = in_int("a", 0, n), b = in_int("b", 0, n);
        outf(ANS "= %s", f4(binom_cdf(b, n, p) - (a ? binom_cdf(a - 1, n, p) : 0)));
        return;
    }
    int r = in_int("r", 0, n);
    if (k == 0) {
        outf("P(X = r) = nCr p^r (1-p)^(n-r)");
        outf(ANS "= %s", f4(binom_pmf(r, n, p)));
    } else if (k == 1) outf(ANS "P(X " S_LE " %d) = %s", r, f4(binom_cdf(r, n, p)));
    else { outf("= 1 - P(X " S_LE " %d)", r - 1); outf(ANS "P(X " S_GE " %d) = %s", r, f4(1 - (r ? binom_cdf(r - 1, n, p) : 0))); }
}

static void p_btest(void) {
    in_note("Binomial hypothesis test for p");
    int n = in_int("n", 1, 10000);
    double p0 = in_real("p under H0");
    static const char *t[] = {"H1: p > p0", "H1: p < p0", "H1: p " S_NE " p0"};
    int tail = in_choice("H1", t, 3);
    static const char *lv[] = {"10%", "5%", "1%"};
    static const double lvv[] = {0.10, 0.05, 0.01};
    double al = lvv[in_choice("Significance level", lv, 3)];
    int x = in_int("Observed x", 0, n);
    double a = tail == 2 ? al / 2 : al;
    if (tail != 1) {
        int c = n + 1;
        while (c > 0 && 1 - binom_cdf(c - 2, n, p0) <= a) c--;
        outf("Upper critical region X " S_GE " %d, P = %s", c, f4(1 - (c ? binom_cdf(c - 1, n, p0) : 0)));
    }
    if (tail != 0) {
        int c = -1;
        while (c < n && binom_cdf(c + 1, n, p0) <= a) c++;
        if (c >= 0) outf("Lower critical region X " S_LE " %d, P = %s", c, f4(binom_cdf(c, n, p0)));
        else outf("Lower critical region: empty");
    }
    double pv;
    if (tail == 0) pv = 1 - (x ? binom_cdf(x - 1, n, p0) : 0);
    else if (tail == 1) pv = binom_cdf(x, n, p0);
    else pv = x >= n * p0 ? 1 - (x ? binom_cdf(x - 1, n, p0) : 0) : binom_cdf(x, n, p0);
    outf("P(X %s %d) = %s", (tail == 1 || (tail == 2 && x < n * p0)) ? S_LE : S_GE, x, f4(pv));
    if (tail == 2) outf("Compare with %s (two-tailed)", fd(a));
    outf(ANS "%s", pv <= a ? "Reject H0: significant evidence" : "Do not reject H0: insufficient evidence");
}

static const calc_t prob_c[] = {
    {"Venn / two events", p_venn},
    {"Binomial probabilities", p_binom},
    {"Binomial hypothesis test", p_btest},
    {"Poisson probabilities", p_poisson},
};

/* ======================= KINEMATICS ======================= */
static void k_suvat(void) {
    in_note("Type x (X," S_TH ",T key) for each unknown");
    static const char *nm[] = {"s", "u", "v", "a", "t"};
    double val[5];
    int known[5] = {0}, nk = 0;
    for (int i = 0; i < 5; i++) { val[i] = in_maybe(nm[i], &known[i]); nk += known[i]; }
    if (nk < 3) fail("Need 3 known values");
    double s = val[0], u = val[1], v = val[2], a = val[3], t = val[4];
    int K = (known[0] << 0) | (known[1] << 1) | (known[2] << 2) | (known[3] << 3) | (known[4] << 4);
#define HAS(x) (K & (1 << (x)))
    if (HAS(1) && HAS(3) && HAS(4)) { outf("v = u + at, s = ut + (1/2)at" S_SQ); v = u + a * t; s = u * t + 0.5 * a * t * t; }
    else if (HAS(1) && HAS(2) && HAS(4)) { outf("a = (v - u)/t, s = (u + v)t/2"); a = (v - u) / t; s = (u + v) * t / 2; }
    else if (HAS(1) && HAS(2) && HAS(3)) { outf("t = (v - u)/a, s = (v" S_SQ " - u" S_SQ ")/2a"); t = (v - u) / a; s = (v * v - u * u) / (2 * a); }
    else if (HAS(0) && HAS(1) && HAS(4)) { outf("a = 2(s - ut)/t" S_SQ); a = 2 * (s - u * t) / (t * t); v = u + a * t; }
    else if (HAS(0) && HAS(2) && HAS(4)) { outf("s = vt - (1/2)at" S_SQ); a = 2 * (v * t - s) / (t * t); u = v - a * t; }
    else if (HAS(0) && HAS(3) && HAS(4)) { outf("u = (s - (1/2)at" S_SQ ")/t"); u = (s - 0.5 * a * t * t) / t; v = u + a * t; }
    else if (HAS(0) && HAS(1) && HAS(2)) { outf("t = 2s/(u + v)"); t = 2 * s / (u + v); a = (v - u) / t; }
    else if (HAS(0) && HAS(1) && HAS(3)) {
        outf("v" S_SQ " = u" S_SQ " + 2as");
        double v2 = u * u + 2 * a * s;
        if (v2 < 0) fail("v" S_SQ " < 0: never reaches s");
        v = sqrt(v2);
        outf("v = " S_PM "%s", fxd(v));
        t = (v - u) / a;
        if (t < 0) { v = -v; t = (v - u) / a; }
        double t2 = (-sqrt(v2) - u) / a;
        if (t2 > 0 && fabs(t2 - t) > 1e-9) outf("Other time: t = %s (v = %s)", fd(t2), fd(-v));
    } else if (HAS(0) && HAS(2) && HAS(3)) { outf("u" S_SQ " = v" S_SQ " - 2as"); double u2 = v * v - 2 * a * s; if (u2 < 0) fail("u" S_SQ " < 0"); u = sqrt(u2); t = (v - u) / a; }
    else fail("Need 3 known values");
    outf(ANS "s = %s, u = %s, v = %s", fxd(s), fxd(u), fxd(v));
    outf(ANS "a = %s, t = %s", fxd(a), fxd(t));
#undef HAS
}

static void k_variable(void) {
    static const char *o[] = {"Know x(t): differentiate", "Know v(t)", "Know a(t): integrate"};
    int k = in_choice("Given", o, 3);
    node *f = in_fn(k == 0 ? "x(t)" : k == 1 ? "v(t)" : "a(t)", 't');
    if (k == 0) {
        node *v = ex_diff(f, 't'), *a = v ? ex_diff(v, 't') : NULL;
        if (!v || !a) fail("Can't differentiate");
        outf(ANS "v = dx/dt = %s", ex_str(v));
        outf(ANS "a = dv/dt = %s", ex_str(a));
        double t = in_real("At t =");
        outf(ANS "x = %s, v = %s, a = %s", fxd(ex_evalr(f, 't', t)), fxd(ex_evalr(v, 't', t)), fxd(ex_evalr(a, 't', t)));
        double r[10];
        int n = nsolve(v, 't', 0, fmax(t, 20), r, 10);
        for (int i = 0; i < n; i++) outf("At rest (v = 0) at t = %s", fx(r[i]));
    } else if (k == 1) {
        node *a = ex_diff(f, 't');
        if (a) outf(ANS "a = dv/dt = %s", ex_str(a));
        double t1 = in_real("Displacement from t ="), t2 = in_real("to t =");
        int ok;
        double r[10];
        int n = nsolve(f, 't', t1, t2, r, 10);
        double tot = 0, prev = t1, disp = integrate(f, 't', t1, t2, &ok);
        for (int i = 0; i <= n; i++) { double e = i < n ? r[i] : t2; tot += fabs(integrate(f, 't', prev, e, &ok)); prev = e; }
        outf(ANS "Displacement = %s", fxd(disp));
        outf(ANS "Distance travelled = %s", fxd(tot));
        if (n) outf("(changes direction at t = %s)", fx(r[0]));
    } else {
        double v0 = in_real("v at t = 0"), t = in_real("Find v at t =");
        int ok;
        outf(ANS "v = %s", fxd(v0 + integrate(f, 't', 0, t, &ok)));
    }
}

static const calc_t kin_c[] = {
    {"SUVAT solver", k_suvat},
    {"Variable acceleration", k_variable},
};

/* ======================= FORCES ======================= */
static void f_pulley(void) {
    static const char *o[] = {"Two masses over a pulley (both hanging)", "Mass on table pulled by hanging mass"};
    int k = in_choice("Setup", o, 2);
    double m1 = in_pos(k ? "Mass on table" : "Heavier mass m1"), m2 = in_pos(k ? "Hanging mass" : "Lighter mass m2");
    double mu = k ? in_real("Friction " S_MU " on table (0 smooth)") : 0;
    double a, T;
    if (!k) {
        outf("m1: m1g - T = m1a,  m2: T - m2g = m2a");
        a = (m1 - m2) * G / (m1 + m2); T = m2 * (G + a);
    } else {
        double F = mu * m1 * G;
        outf("Table: T - %s = m1a,  hanging: m2g - T = m2a", mu ? S_MU "m1g" : "0");
        a = (m2 * G - F) / (m1 + m2);
        if (a < 0) { outf(ANS "Friction holds it: no motion"); return; }
        T = m2 * (G - a);
    }
    outf(ANS "a = %s m/s" S_SQ, fxd(a));
    outf(ANS "T = %s N", fxd(T));
    outf("Force on pulley = %s N", fxd(k ? T * sqrt(2) : 2 * T));
}

static void f_resolve(void) {
    int n = in_int("Number of forces", 1, 8);
    double X = 0, Y = 0;
    for (int i = 0; i < n; i++) {
        double v[2];
        char lab[48] = "Force ";
        strcat(lab, fd(i + 1));
        strcat(lab, ": size, angle from +x (deg)");
        in_vec(lab, v, 2);
        double c = v[0] * cos(v[1] * M_PI / 180), s = v[0] * sin(v[1] * M_PI / 180);
        outf("F%d: (%s, %s)", i + 1, fd(c), fd(s));
        X += c; Y += s;
    }
    outf(ANS "Resultant = (%s, %s)", fd(X), fd(Y));
    outf(ANS "Magnitude %s at %s", fd(hypot(X, Y)), fdeg(atan2(Y, X)));
    double m = in_real("Mass (0 to skip)");
    if (m > 0) outf(ANS "a = %s m/s" S_SQ, fd(hypot(X, Y) / m));
}

static void f_lift(void) {
    double m = in_pos("Mass (kg)"), a = in_real("Acceleration (+ up)");
    outf("R - mg = ma");
    outf(ANS "Reaction/tension = %s N", fxd(m * (G + a)));
}

static void f_connected(void) {
    in_note("Car (m1) towing trailer (m2) on level road");
    double m1 = in_pos("Car mass"), m2 = in_pos("Trailer mass"), D = in_real("Driving force"), R1 = in_real("Resistance on car"), R2 = in_real("Resistance on trailer");
    double a = (D - R1 - R2) / (m1 + m2);
    outf("Whole system: D - R1 - R2 = (m1 + m2)a");
    outf(ANS "a = %s m/s" S_SQ, fxd(a));
    outf("Trailer: T - R2 = m2a");
    outf(ANS "Tension = %s N", fxd(R2 + m2 * a));
}

static const calc_t force_c[] = {
    {"Connected particles / pulleys", f_pulley},
    {"Car and trailer", f_connected},
    {"Resultant of forces", f_resolve},
    {"Lift / vertical motion", f_lift},
};

const topic_t m_u2[] = {
    {"Data presentation & summaries", data_c, NEL(data_c)},
    {"Probability & distributions", prob_c, NEL(prob_c)},
    {"Kinematics", kin_c, NEL(kin_c)},
    {"Forces & Newton's laws", force_c, NEL(force_c)},
};
const int m_u2_n = NEL(m_u2);
