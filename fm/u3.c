/* Further Maths Unit 3: Further Mechanics A */
#include "../common/core.h"
#include "../common/util.h"
#include <string.h>

#define G 9.8

/* ======================= MOMENTUM & IMPULSE ======================= */
static void mo_collide(void) {
    in_note("Direct collision. Take one direction as positive.");
    double m1 = in_pos("Mass A"), u1 = in_real("Velocity of A before");
    double m2 = in_pos("Mass B"), u2 = in_real("Velocity of B before");
    double e = in_real("Coefficient of restitution e");
    if (e < 0 || e > 1) fail("Need 0 " S_LE " e " S_LE " 1");
    outf("CLM: %s(%s) + %s(%s) = %sv1 + %sv2", fx(m1), fx(u1), fx(m2), fx(u2), fx(m1), fx(m2));
    double P = m1 * u1 + m2 * u2;
    outf("     %sv1 + %sv2 = %s", fx(m1), fx(m2), fx(P));
    outf("NEL: v2 - v1 = -e(u2 - u1) = %s", fx(-e * (u2 - u1)));
    double s = e * (u1 - u2);
    double v1 = (P - m2 * s) / (m1 + m2), v2 = v1 + s;
    outf("Solve simultaneously:");
    outf(ANS "v1 = %s,  v2 = %s", fx(v1), fx(v2));
    double ke0 = 0.5 * m1 * u1 * u1 + 0.5 * m2 * u2 * u2, ke1 = 0.5 * m1 * v1 * v1 + 0.5 * m2 * v2 * v2;
    outf("KE before = %s, after = %s", fx(ke0), fx(ke1));
    outf(ANS "Loss of KE = %s J", fxd(ke0 - ke1));
    outf(ANS "Impulse on B = %s Ns (on A: %s)", fx(m2 * (v2 - u2)), fx(m1 * (v1 - u1)));
    if (v1 > v2 + 1e-12) outf("Note: A still faster than B: check directions");
}

static void mo_finde(void) {
    double u1 = in_real("A before"), u2 = in_real("B before"), v1 = in_real("A after"), v2 = in_real("B after");
    if (u1 == u2) fail("No approach");
    outf("e = speed of separation / speed of approach");
    outf("= (%s - %s)/(%s - %s)", fxp(v2), fxp(v1), fxp(u1), fxp(u2));
    double e = (v2 - v1) / (u1 - u2);
    outf(ANS "e = %s", fx(e));
    if (e < 0 || e > 1) outf("Not between 0 and 1: check the data");
}

static void mo_wall(void) {
    double m = in_pos("Mass"), u = in_pos("Speed before (towards wall)"), e = in_real("e");
    double v = e * u;
    outf("Rebound speed = eu = %s(%s)", fx(e), fx(u));
    outf(ANS "v = %s m/s", fx(v));
    outf("Impulse = m(v - (-u)) = %s(%s + %s)", fx(m), fx(v), fx(u));
    outf(ANS "Impulse = %s Ns", fx(m * (v + u)));
    outf(ANS "KE lost = %s J", fxd(0.5 * m * (u * u - v * v)));
}

static void mo_impulse(void) {
    static const char *o[] = {"Find impulse from velocities", "Find final velocity", "Impulse from force F(t) over time"};
    int k = in_choice("Find", o, 3);
    double m = in_pos("Mass");
    if (k == 0) {
        double u = in_real("u"), v = in_real("v");
        outf("I = mv - mu = %s(%s) - %s(%s)", fx(m), fx(v), fx(m), fx(u));
        outf(ANS "I = %s Ns", fx(m * (v - u)));
    } else if (k == 1) {
        double u = in_real("u"), I = in_real("Impulse I");
        outf("v = u + I/m = %s + %s/%s", fx(u), fxp(I), fx(m));
        outf(ANS "v = %s m/s", fx(u + I / m));
    } else {
        node *f = in_fn("F(t)", 't');
        double a = in_real("From t ="), b = in_real("To t =");
        int ok;
        double I = integrate(f, 't', a, b, &ok);
        outf("I = " S_INT " F dt = %s", fxd(I));
        double u = in_real("Initial velocity u");
        outf(ANS "I = %s Ns, v = u + I/m = %s", fx(I), fxd(u + I / m));
    }
}

static const calc_t mom_c[] = {
    {"Direct collision (CLM + NEL)", mo_collide},
    {"Find e from velocities", mo_finde},
    {"Collision with a wall", mo_wall},
    {"Impulse = change in momentum", mo_impulse},
};

/* ======================= HOOKE'S LAW ======================= */
static void h_tension(void) {
    double lam = in_pos("Modulus " S_LA " (N)"), l = in_pos("Natural length l"), x = in_real("Extension x");
    outf("T = " S_LA "x/l = %s(%s)/%s", fx(lam), fx(x), fx(l));
    outf(ANS "T = %s N", fxd(lam * x / l));
    outf("EPE = " S_LA "x" S_SQ "/2l = %s(%s)" S_SQ "/%s", fx(lam), fx(x), fx(2 * l));
    outf(ANS "EPE = %s J", fxd(lam * x * x / (2 * l)));
}

static void h_hang(void) {
    double m = in_pos("Mass m (kg)"), lam = in_pos("Modulus " S_LA), l = in_pos("Natural length l");
    outf("Equilibrium: T = mg, " S_LA "e/l = %s(9.8)", fx(m));
    double e = m * G * l / lam;
    outf(ANS "Extension e = mgl/" S_LA " = %s m", fxd(e));
    outf(ANS "Length = %s m", fxd(l + e));
}

static void h_drop(void) {
    in_note("Particle on string fixed at O, released from O (falls l then stretches)");
    double m = in_pos("Mass m"), lam = in_pos("Modulus " S_LA), l = in_pos("Natural length l");
    double v0 = in_real("Initial speed (0 if released)");
    outf("At lowest point (extension x), speed 0:");
    outf("Loss of GPE + initial KE = EPE");
    outf("%s(9.8)(%s + x) + %s = %sx" S_SQ "/%s", fx(m), fx(l), fx(0.5 * m * v0 * v0), fx(lam), fx(2 * l));
    double A = lam / (2 * l), B = -m * G, C = -(m * G * l + 0.5 * m * v0 * v0);
    double q[3] = {C, B, A};
    outf("%s = 0", fpoly(q, 2, "x"));
    double x = (-B + sqrt(B * B - 4 * A * C)) / (2 * A);
    outf(ANS "Max extension x = %s m", fxd(x));
    outf(ANS "Lowest point is %s m below O", fd(l + x));
    double e = m * G * l / lam;
    double vmax2 = v0 * v0 + 2 * G * (l + e) - lam * e * e / (l * m);
    outf("Max speed at equilibrium (ext %s):", fd(e));
    outf(ANS "v max = %s m/s", fd(sqrt(vmax2)));
    outf("Max tension = %s N", fd(lam * x / l));
}

static void h_energy(void) {
    in_note("Speed when extension changes (horizontal, smooth)");
    double m = in_pos("Mass m"), lam = in_pos("Modulus " S_LA), l = in_pos("Natural length l");
    double x1 = in_real("Extension at start"), v1 = in_real("Speed at start"), x2 = in_real("Extension at end");
    double E1 = 0.5 * m * v1 * v1 + lam * x1 * x1 / (2 * l), EPE2 = lam * x2 * x2 / (2 * l);
    outf("KE + EPE constant:");
    outf("%s + %s = (1/2)(%s)v" S_SQ " + %s", fx(0.5 * m * v1 * v1), fx(lam * x1 * x1 / (2 * l)), fx(m), fx(EPE2));
    double v2 = (E1 - EPE2) * 2 / m;
    if (v2 < 0) outf(ANS "Doesn't reach that extension");
    else outf(ANS "v = %s m/s", fxd(sqrt(v2)));
}

static const calc_t hooke_c[] = {
    {"Tension and elastic energy", h_tension},
    {"Hanging in equilibrium", h_hang},
    {"Falling on elastic string", h_drop},
    {"Speed via energy (horizontal)", h_energy},
};

/* ======================= WORK, ENERGY, POWER ======================= */
static void w_power(void) {
    in_note("Vehicle: driving force F = P/v");
    double P = in_pos("Power P (W)"), m = in_pos("Mass m (kg)"), R_ = in_real("Resistance R (N)");
    double al = in_real("Slope angle (deg, + uphill, 0 flat)") * M_PI / 180;
    double comp = m * G * sin(al);
    outf("Max speed: a = 0, so P/v = R + mg sin" S_AL);
    outf("P/v = %s + %s", fx(R_), fd(comp));
    if (R_ + comp <= 0) outf("Resistance + slope " S_LE " 0: no max speed");
    else outf(ANS "v max = %s m/s", fxd(P / (R_ + comp)));
    double v = in_pos("Acceleration at speed v =");
    double F = P / v;
    outf("F = P/v = %s", fxd(F));
    outf("F - R - mg sin" S_AL " = ma");
    outf(ANS "a = %s m/s" S_SQ, fxd((F - R_ - comp) / m));
}

static void w_slope(void) {
    in_note("Particle moving along a line (flat or slope)");
    double m = in_pos("Mass m"), u = in_real("Initial speed u"), d = in_pos("Distance d");
    double al = in_real("Angle (deg, + = moving uphill)") * M_PI / 180;
    double mu = in_real("Friction " S_MU " (0 if smooth)"), P_ = in_real("Extra force along motion (N)");
    double N_ = m * G * cos(al), F = mu * N_;
    outf("R = mg cos" S_AL " = %s, friction = " S_MU "R = %s", fd(N_), fd(F));
    double Wg = -m * G * sin(al) * d, Wf = -F * d, Wp = P_ * d;
    outf("Work by weight = %s J", fd(Wg));
    outf("Work against friction = %s J", fd(-Wf));
    if (P_) outf("Work by force = %s J", fd(Wp));
    double KE = 0.5 * m * u * u + Wg + Wf + Wp;
    outf("Final KE = %s + (%s) = %s", fd(0.5 * m * u * u), fd(Wg + Wf + Wp), fd(KE));
    if (KE < 0) {
        double dec = m * G * sin(al) + F - P_;
        outf(ANS "Stops before d: distance = %s m", fd(0.5 * m * u * u / dec));
    } else outf(ANS "v = %s m/s", fxd(sqrt(2 * KE / m)));
}

static void w_variable(void) {
    node *f = in_fn("Force F(x)", 'x');
    double a = in_real("From x ="), b = in_real("To x =");
    int ok;
    double W = integrate(f, 'x', a, b, &ok);
    outf("W = " S_INT " F dx");
    outf(ANS "Work = %s J", fxd(W));
    double m = in_pos("Mass (for speed)");
    double u = in_real("Initial speed");
    double ke = 0.5 * m * u * u + W;
    if (ke >= 0) outf(ANS "Final speed = %s m/s", fxd(sqrt(2 * ke / m)));
}

static const calc_t work_c[] = {
    {"Power, max speed, acceleration", w_power},
    {"Work-energy on a slope", w_slope},
    {"Work by a variable force", w_variable},
};

/* ======================= CIRCULAR MOTION ======================= */
static void c_basic(void) {
    static const char *o[] = {"Know v", "Know " S_OM " (rad/s)", "Know rev/min"};
    int k = in_choice("Speed given as", o, 3);
    double r = in_pos("Radius r"), x = in_pos(k == 0 ? "v" : k == 1 ? S_OM : "rev/min");
    double w = k == 0 ? x / r : k == 1 ? x : x * 2 * M_PI / 60;
    double v = w * r;
    outf(S_OM " = %s rad/s, v = r" S_OM " = %s m/s", fxd(w), fxd(v));
    outf(ANS "a = v" S_SQ "/r = r" S_OM S_SQ " = %s m/s" S_SQ, fxd(v * v / r));
    outf("Period = 2" S_PI "/" S_OM " = %s s", fxd(2 * M_PI / w));
    double m = in_pos("Mass (for force)");
    outf(ANS "Force towards centre = %s N", fxd(m * v * v / r));
}

static void c_conical(void) {
    in_note("Conical pendulum: string length l at angle " S_TH " to vertical");
    double m = in_pos("Mass m"), l = in_pos("String length l");
    static const char *o[] = {"Know angle " S_TH, "Know radius r", "Know " S_OM};
    int k = in_choice("Given", o, 3);
    double th;
    if (k == 0) th = in_real(S_TH " (deg)") * M_PI / 180;
    else if (k == 1) th = asin(in_pos("r") / l);
    else { double w = in_pos(S_OM); outf("T = m" S_OM S_SQ "l (from T sin" S_TH " = m" S_OM S_SQ "l sin" S_TH ")"); double T = m * w * w * l; outf("cos" S_TH " = mg/T"); if (m * G / T > 1) fail(S_OM " too small to lift the string"); th = acos(m * G / T); }
    double r = l * sin(th);
    outf("Vertical: T cos" S_TH " = mg");
    outf("Horizontal: T sin" S_TH " = m" S_OM S_SQ "r");
    double T = m * G / cos(th), w = sqrt(G / (l * cos(th)));
    outf(ANS S_TH " = %s, r = %s m", fdeg(th), fd(r));
    outf(ANS "T = %s N", fd(T));
    outf(ANS S_OM " = %s rad/s, v = %s m/s", fd(w), fd(w * r));
    outf("Period = %s s, height below O = %s m", fd(2 * M_PI / w), fd(l * cos(th)));
}

static void c_banked(void) {
    double r = in_pos("Radius r"), th = in_real("Bank angle (deg)") * M_PI / 180, mu = in_real("Friction " S_MU " (0 if none)");
    double v0 = sqrt(r * G * tan(th));
    outf("No friction needed: v" S_SQ " = rg tan" S_TH);
    outf(ANS "v = %s m/s", fd(v0));
    if (mu > 0) {
        double s = sin(th), c = cos(th);
        double vmax2 = r * G * (s + mu * c) / (c - mu * s);
        double vmin2 = r * G * (s - mu * c) / (c + mu * s);
        outf("Max speed: friction down the slope");
        if (c - mu * s <= 0) outf(ANS "No maximum speed");
        else outf(ANS "v max = %s m/s", fd(sqrt(vmax2)));
        outf("Min speed: friction up the slope");
        if (vmin2 <= 0) outf(ANS "No minimum (can be stationary)");
        else outf(ANS "v min = %s m/s", fd(sqrt(vmin2)));
    }
}

static void c_vertical(void) {
    static const char *o[] = {"String (needs T " S_GE " 0)", "Rod (can push)", "Inside smooth surface", "Outside smooth sphere"};
    int k = in_choice("Particle on", o, 4);
    double m = in_pos("Mass m"), r = in_pos("Radius r"), u = in_real("Speed at bottom u");
    if (k == 3) {
        outf("Starts at top with speed u, slides down");
        outf("At angle " S_TH " from vertical: v" S_SQ " = u" S_SQ " + 2gr(1 - cos" S_TH ")");
        outf("R = mg cos" S_TH " - mv" S_SQ "/r, leaves surface when R = 0");
        double c = (u * u + 2 * G * r) / (3 * G * r);
        if (c > 1) { outf(ANS "Leaves immediately at the top"); return; }
        outf(ANS "cos" S_TH " = (u" S_SQ " + 2gr)/(3gr) = %s", fx(c));
        outf(ANS S_TH " = %s", fdeg(acos(c)));
        outf("Speed then: %s m/s", fd(sqrt(u * u + 2 * G * r * (1 - c))));
        return;
    }
    outf("Energy: v" S_SQ " = u" S_SQ " - 2gr(1 - cos" S_TH "), " S_TH " from bottom");
    outf("Radial: T - mg cos" S_TH " = mv" S_SQ "/r");
    double vt2 = u * u - 4 * G * r;
    outf("At top: v" S_SQ " = %s", fd(vt2));
    double Ttop = m * vt2 / r - m * G;
    if (k == 1) {
        if (vt2 >= 0) outf(ANS "Completes circle (v" S_SQ " " S_GE " 0 at top)");
        else outf(ANS "Doesn't reach top: oscillates");
    } else {
        outf("Tension/reaction at top = %s N", fd(Ttop));
        if (Ttop >= -1e-9) outf(ANS "Completes circle (needs u" S_SQ " " S_GE " 5gr = %s)", fd(5 * G * r));
        else {
            /* T = 0: m(u^2 - 2gr + 2gr cos)/r + mg cos = 0 -> cos = (2gr - u^2)/(3gr) */
            double c = (2 * G * r - u * u) / (3 * G * r);
            if (c > 1) { outf(ANS "Never leaves the bottom?"); return; }
            if (c < -1) { outf(ANS "Completes circle"); return; }
            double th = acos(c);
            double v2 = u * u - 2 * G * r * (1 - c);
            if (v2 < 0) { outf(ANS "Stops and swings back (" S_TH " < 90" S_DEG ")"); outf("Max angle: cos" S_TH " = %s", fx(1 - u * u / (2 * G * r))); return; }
            outf(ANS "%s goes slack/leaves when cos" S_TH " = %s", k == 0 ? "String" : "Particle", fx(c));
            outf(ANS S_TH " = %s from bottom", fdeg(th));
            outf("Speed then = %s m/s", fd(sqrt(v2)));
        }
    }
    double th = in_real("Find T and v at " S_TH " (deg) =") * M_PI / 180;
    double v2 = u * u - 2 * G * r * (1 - cos(th));
    if (v2 < 0) { outf(ANS "Doesn't reach that angle"); return; }
    outf(ANS "v = %s m/s", fd(sqrt(v2)));
    outf(ANS "%s = %s N", k == 0 ? "T" : k == 1 ? "Force in rod" : "R", fd(m * v2 / r + m * G * cos(th)));
}

static const calc_t circ_c[] = {
    {"v, " S_OM ", acceleration, force", c_basic},
    {"Conical pendulum", c_conical},
    {"Banked track", c_banked},
    {"Vertical circle", c_vertical},
};

/* ======================= VECTOR CALCULUS ======================= */
static int in_dim(void) {
    static const char *o[] = {"2D (i, j)", "3D (i, j, k)"};
    return in_choice("Vectors", o, 2) + 2;
}

void vc_fromr(void) {
    int n = in_dim();
    node *r[3], *v[3], *a[3];
    in_vecfn("r(t) components", r, n, 't');
    for (int i = 0; i < n; i++) { v[i] = ex_diff(r[i], 't'); a[i] = v[i] ? ex_diff(v[i], 't') : NULL; if (!v[i] || !a[i]) fail("Can't differentiate"); }
    static const char *u[] = {"i", "j", "k"};
    outf("v = dr/dt:");
    for (int i = 0; i < n; i++) outf(ANS "  (%s)%s", ex_str(v[i]), u[i]);
    outf("a = dv/dt:");
    for (int i = 0; i < n; i++) outf(ANS "  (%s)%s", ex_str(a[i]), u[i]);
    double t = in_real("Evaluate at t =");
    double R_[3], V[3], A[3];
    for (int i = 0; i < n; i++) { R_[i] = ex_evalr(r[i], 't', t); V[i] = ex_evalr(v[i], 't', t); A[i] = ex_evalr(a[i], 't', t); }
    outf("t = %s:", fx(t));
    outf(ANS "r = %s", fvec(R_, n));
    outf(ANS "v = %s, speed = %s", fvec(V, n), fxd(sqrt(V[0] * V[0] + V[1] * V[1] + (n == 3 ? V[2] * V[2] : 0))));
    outf(ANS "a = %s, |a| = %s", fvec(A, n), fxd(sqrt(A[0] * A[0] + A[1] * A[1] + (n == 3 ? A[2] * A[2] : 0))));
    double m = in_real("Mass (0 to skip force)");
    if (m > 0) { double F[3]; for (int i = 0; i < n; i++) F[i] = m * A[i]; outf(ANS "F = ma = %s", fvec(F, n)); }
}

/* try to represent f(t) as polynomial (degree <= 8) */
static int as_poly(node *f, double *c) {
    ser s;
    const char *err;
    if (!ex_series(f, 't', 0, &s, &err)) return 0;
    for (int k = 9; k <= SER_N; k++) if (fabs(s.c[k]) > 1e-12) return 0;
    for (int k = 0; k <= 8; k++) c[k] = s.c[k];
    for (double t = -2.3; t < 3; t += 1.7) if (fabs(peval_r(c, 8, t) - ex_evalr(f, 't', t)) > 1e-8 * (1 + fabs(peval_r(c, 8, t)))) return 0;
    return 1;
}

void vc_froma(void) {
    int n = in_dim();
    static const char *o[] = {"Know a(t), v(0), r(0)", "Know v(t), r(0)"};
    int k = in_choice("Given", o, 2);
    node *f[3];
    double v0[3] = {0}, r0[3] = {0}, t0;
    in_vecfn(k == 0 ? "a(t) components" : "v(t) components", f, n, 't');
    t0 = in_real("Initial time t0");
    if (k == 0) in_vec("v at t0", v0, n);
    in_vec("r at t0", r0, n);
    static const char *u[] = {"i", "j", "k"};
    double pc[3][10];
    int poly = 1;
    for (int i = 0; i < n; i++) if (!as_poly(f[i], pc[i])) poly = 0;
    if (poly) {
        outf("Integrate each component, + constant from initial values:");
        for (int pass = 0; pass < (k == 0 ? 2 : 1); pass++) {
            for (int i = 0; i < n; i++) {
                double q[10] = {0};
                for (int j = 0; j < 9; j++) q[j + 1] = pc[i][j] / (j + 1);
                double c0 = (pass == 0 && k == 0 ? v0[i] : r0[i]) - peval_r(q, 9, t0);
                q[0] = c0;
                memcpy(pc[i], q, sizeof q);
            }
            const char *nm = (k == 0 && pass == 0) ? "v" : "r";
            outf(ANS "%s(t) =", nm);
            for (int i = 0; i < n; i++) outf(ANS "  (%s)%s", fpoly(pc[i], 9, "t"), u[i]);
        }
    } else outf("Not polynomial: values found numerically");
    double t = in_real("Evaluate at t =");
    double V[3], Rr[3];
    for (int i = 0; i < n; i++) {
        if (poly) { Rr[i] = peval_r(pc[i], 9, t); }
        int ok;
        if (k == 0) {
            V[i] = v0[i] + integrate(f[i], 't', t0, t, &ok);
            if (!poly) {
                /* r = r0 + ∫ v: Simpson on v */
                int N = 200;
                double h = (t - t0) / N, s = 0;
                for (int j = 0; j <= N; j++) {
                    double tt = t0 + j * h;
                    double vv = v0[i] + integrate(f[i], 't', t0, tt, &ok);
                    s += vv * (j == 0 || j == N ? 1 : (j % 2 ? 4 : 2));
                }
                Rr[i] = r0[i] + s * h / 3;
            }
        } else {
            V[i] = ex_evalr(f[i], 't', t);
            if (!poly) Rr[i] = r0[i] + integrate(f[i], 't', t0, t, &ok);
        }
    }
    outf("At t = %s:", fx(t));
    outf(ANS "v = %s", fvec(V, n));
    outf(ANS "r = %s", fvec(Rr, n));
}

static const calc_t vcalc_c[] = {
    {"Given r(t): find v, a", vc_fromr},
    {"Given a(t) or v(t): integrate", vc_froma},
};

const topic_t fm_u3[] = {
    {"Momentum, impulse, restitution", mom_c, NEL(mom_c)},
    {"Hooke's law & elastic energy", hooke_c, NEL(hooke_c)},
    {"Work, energy & power", work_c, NEL(work_c)},
    {"Circular motion", circ_c, NEL(circ_c)},
    {"Vectors: differentiate/integrate", vcalc_c, NEL(vcalc_c)},
};
const int fm_u3_n = NEL(fm_u3);
