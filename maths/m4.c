/* A Level Maths Unit 4: Applied Mathematics B (A2) */
#include "../common/core.h"
#include "../common/util.h"
#include "../common/stats.h"
#include "../common/shared.h"
#include <string.h>

#define G 9.8
static const char *f4(double x) { return fdp(x, 4); }

/* ======================= NORMAL DISTRIBUTION ======================= */
static void n_prob(void) {
    double mu = in_real(S_MU), sg = in_pos(S_SG " (standard deviation)");
    static const char *o[] = {"P(X < a)", "P(X > a)", "P(a < X < b)"};
    int k = in_choice("Find", o, 3);
    double a = in_real("a");
    double za = (a - mu) / sg;
    outf("z = (a - " S_MU ")/" S_SG " = (%s - %s)/%s = %s", fx(a), fx(mu), fx(sg), f4(za));
    if (k == 0) outf(ANS "P(X < %s) = %s", fx(a), f4(norm_cdf(za)));
    else if (k == 1) outf(ANS "P(X > %s) = %s", fx(a), f4(1 - norm_cdf(za)));
    else {
        double b = in_real("b"), zb = (b - mu) / sg;
        outf("z_b = %s", f4(zb));
        outf(ANS "P(%s < X < %s) = %s", fx(a), fx(b), f4(norm_cdf(zb) - norm_cdf(za)));
    }
}

static void n_inv(void) {
    double mu = in_real(S_MU), sg = in_pos(S_SG);
    static const char *o[] = {"P(X < a) = p", "P(X > a) = p", "Symmetric P(" S_MU "-k < X < " S_MU "+k) = p"};
    int k = in_choice("Find a with", o, 3);
    double p = in_real("p");
    if (p <= 0 || p >= 1) fail("0 < p < 1");
    double z = k == 0 ? norm_inv(p) : k == 1 ? norm_inv(1 - p) : norm_inv((1 + p) / 2);
    outf("z = %s", f4(z));
    if (k == 2) outf(ANS "k = %s, range %s to %s", fd(z * sg), fd(mu - z * sg), fd(mu + z * sg));
    else outf(ANS "a = " S_MU " + z" S_SG " = %s", fd(mu + z * sg));
}

static void n_params(void) {
    in_note("Find " S_MU " and/or " S_SG " from known probabilities");
    static const char *o[] = {"Find " S_MU " (know " S_SG ")", "Find " S_SG " (know " S_MU ")", "Find both (two conditions)"};
    int k = in_choice("Unknown", o, 3);
    if (k < 2) {
        double other = in_real(k == 0 ? S_SG : S_MU), a = in_real("Value a"), p = in_real("P(X < a)");
        double z = norm_inv(p);
        outf("z = %s, so (a - " S_MU ")/" S_SG " = %s", f4(z), f4(z));
        if (k == 0) outf(ANS S_MU " = %s", fd(a - z * other));
        else { if (fabs(z) < 1e-12) fail("z = 0: a is the mean"); outf(ANS S_SG " = %s", fd((a - other) / z)); }
    } else {
        double a = in_real("a1"), p = in_real("P(X < a1)"), b = in_real("a2"), q = in_real("P(X < a2)");
        double z1 = norm_inv(p), z2 = norm_inv(q);
        outf(S_MU " + %s" S_SG " = %s", f4(z1), fx(a));
        outf(S_MU " + %s" S_SG " = %s", f4(z2), fx(b));
        double sg = (b - a) / (z2 - z1);
        outf(ANS S_SG " = %s,  " S_MU " = %s", fd(sg), fd(a - z1 * sg));
    }
}

static void n_test(void) {
    in_note("Test for the mean (known " S_SG ")");
    double mu0 = in_real(S_MU " under H0"), sg = in_pos(S_SG), xbar = in_real("Sample mean"), n = in_pos("Sample size n");
    static const char *t[] = {"H1: " S_MU " > " S_MU "0", "H1: " S_MU " < " S_MU "0", "H1: " S_MU " " S_NE " " S_MU "0"};
    int tail = in_choice("H1", t, 3);
    static const char *lv[] = {"10%", "5%", "1%"};
    static const double lvv[] = {0.10, 0.05, 0.01};
    double al = lvv[in_choice("Significance level", lv, 3)];
    double se = sg / sqrt(n), z = (xbar - mu0) / se;
    outf("X-bar ~ N(%s, %s" S_SQ "/%s)", fx(mu0), fx(sg), fx(n));
    outf("z = (%s - %s)/(%s/" S_SQRT "%s) = %s", fx(xbar), fx(mu0), fx(sg), fx(n), f4(z));
    double p = tail == 0 ? 1 - norm_cdf(z) : tail == 1 ? norm_cdf(z) : 2 * (1 - norm_cdf(fabs(z)));
    double zc = tail == 2 ? norm_inv(1 - al / 2) : norm_inv(1 - al);
    outf("p-value = %s", f4(p));
    outf("Critical z = %s%s", tail == 2 ? S_PM : tail == 1 ? "-" : "", f4(zc));
    outf(ANS "%s", p < al ? "Reject H0: significant evidence" : "Do not reject H0: insufficient evidence");
}

static void n_approx(void) {
    int n = in_int("Binomial n", 1, 1000000);
    double p = in_real("p");
    double mu = n * p, sg = sqrt(n * p * (1 - p));
    outf("B(n, p) " S_APX " N(%s, %s)", fx(mu), fd(sg * sg));
    int r = in_int("P(X " S_LE " r), r =", 0, n);
    outf("Continuity correction: P(Y < %s)", fd(r + 0.5));
    outf(ANS "Normal approx = %s", f4(norm_cdf((r + 0.5 - mu) / sg)));
    outf("Exact binomial = %s", f4(binom_cdf(r, n, p)));
}

static const calc_t norm_c[] = {
    {"Normal probabilities", n_prob},
    {"Inverse normal", n_inv},
    {"Find " S_MU " or " S_SG, n_params},
    {"Hypothesis test for the mean", n_test},
    {"Normal approx to binomial", n_approx},
};

/* ======================= PROBABILITY & CORRELATION ======================= */
static void p_cond(void) {
    double a = in_real("P(A)"), ba = in_real("P(B | A)"), bna = in_real("P(B | not A)");
    double pb = a * ba + (1 - a) * bna;
    outf("Tree diagram:");
    outf("P(A and B) = %s " S_TIMES " %s = %s", fx(a), fx(ba), fx(a * ba));
    outf("P(not A and B) = %s " S_TIMES " %s = %s", fx(1 - a), fx(bna), fx((1 - a) * bna));
    outf(ANS "P(B) = %s", fxd(pb));
    if (pb > 0) outf(ANS "P(A | B) = %s/%s = %s", fx(a * ba), fx(pb), fxd(a * ba / pb));
}

static const calc_t prob_c[] = {
    {"Conditional probability (tree)", p_cond},
    {"PMCC + hypothesis test", cr_pmcc},
    {"Regression line", cr_regress},
};

/* ======================= PROJECTILES ======================= */
static void pr_full(void) {
    double u = in_pos("Speed u"), th = in_real("Angle above horizontal (deg)") * M_PI / 180, h = in_real("Launch height above ground");
    double ux = u * cos(th), uy = u * sin(th);
    outf("u_x = u cos" S_TH " = %s,  u_y = u sin" S_TH " = %s", fd(ux), fd(uy));
    double tm = uy / G;
    outf(ANS "Time to max height = %s s", fd(tm));
    outf(ANS "Max height = %s m above launch", fd(uy * uy / (2 * G)));
    double t = (uy + sqrt(uy * uy + 2 * G * h)) / G;
    outf("Lands when %st - 4.9t" S_SQ " = %s", fd(uy), fd(-h));
    outf(ANS "Time of flight = %s s", fd(t));
    outf(ANS "Range = %s m", fd(ux * t));
    double vy = uy - G * t;
    outf(ANS "Landing speed %s m/s at %s below horizontal", fd(hypot(ux, vy)), fdeg(atan2(-vy, ux)));
    outf("Path: y = x tan" S_TH " - gx" S_SQ "/(2u" S_SQ "cos" S_SQ S_TH ")");
    static const char *o[] = {"Done", "Position/velocity at time t", "Time(s) at a given height"};
    int k = in_choice("More?", o, 3);
    if (k == 1) {
        double T = in_real("t");
        outf(ANS "x = %s, y = %s", fd(ux * T), fd(h + uy * T - 0.5 * G * T * T));
        outf(ANS "v = (%s, %s), speed %s", fd(ux), fd(uy - G * T), fd(hypot(ux, uy - G * T)));
    } else if (k == 2) {
        double Y = in_real("Height above ground");
        double d = uy * uy - 2 * G * (Y - h);
        if (d < 0) { outf(ANS "Never reaches that height"); return; }
        double t1 = (uy - sqrt(d)) / G, t2 = (uy + sqrt(d)) / G;
        if (t1 >= 0) outf(ANS "t = %s s (x = %s)", fd(t1), fd(ux * t1));
        outf(ANS "t = %s s (x = %s)", fd(t2), fd(ux * t2));
    }
}

static void pr_angle(void) {
    in_note("Find angle to hit a point (x, y) with speed u");
    double u = in_pos("u"), X = in_pos("x"), Y = in_real("y");
    /* y = x T - g x^2 (1+T^2)/(2u^2), T = tan */
    double k = G * X * X / (2 * u * u);
    double q[3] = {-Y - k, X, -k};
    outf("Use 1/cos" S_SQ " = 1 + tan" S_SQ ":");
    outf("%s = 0 (T = tan" S_TH ")", fpoly(q, 2, "T"));
    double D = X * X - 4 * k * (Y + k);
    if (D < 0) { outf(ANS "Out of range"); return; }
    double T1 = (X + sqrt(D)) / (2 * k), T2 = (X - sqrt(D)) / (2 * k);
    outf(ANS S_TH " = %s or %s", fdeg(atan(T1)), fdeg(atan(T2)));
}

static const calc_t proj_c[] = {
    {"Projectile: everything", pr_full},
    {"Angle to hit a target", pr_angle},
};

/* ======================= FRICTION & SLOPES ======================= */
static void fr_slope(void) {
    double m = in_pos("Mass"), al = in_real("Slope angle (deg)") * M_PI / 180, mu = in_real(S_MU);
    double P = in_real("Applied force along slope (+ up, 0 none)");
    double N_ = m * G * cos(al), Fmax = mu * N_, comp = m * G * sin(al);
    outf("R = mg cos" S_AL " = %s", fd(N_));
    outf("Weight down slope = mg sin" S_AL " = %s", fd(comp));
    outf("F max = " S_MU "R = %s", fd(Fmax));
    double net = P - comp;
    if (fabs(net) <= Fmax + 1e-12) {
        outf(ANS "Stays at rest: friction needed = %s %s", fd(fabs(net)), net > 0 ? "(down slope)" : "(up slope)");
        if (fabs(fabs(net) - Fmax) < 1e-9) outf("Limiting equilibrium");
        return;
    }
    double a = (fabs(net) - Fmax) / m;
    outf(ANS "Moves %s the slope", net > 0 ? "up" : "down");
    outf(ANS "a = %s m/s" S_SQ, fd(a));
    outf("tan" S_AL " = %s vs " S_MU " = %s", fd(tan(al)), fd(mu));
}

static void fr_pull(void) {
    in_note("Force P at angle " S_TH " above horizontal, rough horizontal ground");
    double m = in_pos("Mass"), P = in_real("P"), th = in_real(S_TH " (deg)") * M_PI / 180, mu = in_real(S_MU);
    double R_ = m * G - P * sin(th);
    outf("R = mg - P sin" S_TH " = %s", fd(R_));
    double F = mu * R_, drive = P * cos(th);
    outf("P cos" S_TH " = %s, F max = %s", fd(drive), fd(F));
    if (drive <= F) outf(ANS "Doesn't move (friction = %s)", fd(drive));
    else outf(ANS "a = %s m/s" S_SQ, fd((drive - F) / m));
}

static const calc_t fric_c[] = {
    {"Particle on a rough slope", fr_slope},
    {"Pulled at an angle on rough ground", fr_pull},
};

/* ======================= VECTORS IN MECHANICS ======================= */
static void vm_const(void) {
    in_note("Constant acceleration vectors (2D)");
    double r0[2], u[2], a[2];
    in_vec("Initial position r0", r0, 2);
    in_vec("Initial velocity u", u, 2);
    in_vec("Acceleration a", a, 2);
    double t = in_real("t");
    double v[2] = {u[0] + a[0] * t, u[1] + a[1] * t};
    double r[2] = {r0[0] + u[0] * t + 0.5 * a[0] * t * t, r0[1] + u[1] * t + 0.5 * a[1] * t * t};
    outf("v = u + at,  r = r0 + ut + (1/2)at" S_SQ);
    outf(ANS "v = %s, speed %s", fvec(v, 2), fxd(hypot(v[0], v[1])));
    outf(ANS "r = %s", fvec(r, 2));
    outf("Bearing of v from north: %s", fdeg(atan2(v[0], v[1]) < 0 ? atan2(v[0], v[1]) + 2 * M_PI : atan2(v[0], v[1])));
    double m = in_real("Mass (0 to skip)");
    if (m > 0) { double F[2] = {m * a[0], m * a[1]}; outf(ANS "F = ma = %s, |F| = %s", fvec(F, 2), fxd(hypot(F[0], F[1]))); }
}

static const calc_t vec_c[] = {
    {"Constant acceleration (vectors)", vm_const},
    {"Given r(t): find v and a", vc_fromr},
    {"Given a(t) or v(t): integrate", vc_froma},
    {"Moments: rod on two supports", rb_moments},
};

const topic_t m_u4[] = {
    {"Normal distribution & tests", norm_c, NEL(norm_c)},
    {"Probability & correlation", prob_c, NEL(prob_c)},
    {"Projectiles", proj_c, NEL(proj_c)},
    {"Friction & inclined planes", fric_c, NEL(fric_c)},
    {"Vectors & moments", vec_c, NEL(vec_c)},
};
const int m_u4_n = NEL(m_u4);
