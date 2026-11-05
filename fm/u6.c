/* Further Maths Unit 6: Further Mechanics B */
#include "../common/core.h"
#include "../common/util.h"
#include <string.h>

#define G 9.8

/* ======================= RECTILINEAR MOTION ======================= */
static node *fa;
static double av(double v) { return ex_evalr(fa, 'v', v); }

static void r_av(void) {
    in_note("a = f(v), e.g. -0.2v" S_SQ " or 9.8 - 0.1v");
    fa = in_fn("a = f(v)", 'v');
    double v0 = in_real("Initial speed v0"), v1 = in_real("Final speed v1");
    char s[240];
    strcpy(s, "1/("); strcat(s, ex_src(fa)); strcat(s, ")");
    const char *err;
    node *g = ex_parse(s, &err);
    strcpy(s, "v/("); strcat(s, ex_src(fa)); strcat(s, ")");
    node *h = ex_parse(s, &err);
    if (!g || !h) fail("Expression too long");
    int ok1, ok2;
    double T = integrate(g, 'v', v0, v1, &ok1), X = integrate(h, 'v', v0, v1, &ok2);
    outf("dv/dt = f(v):  t = " S_INT " dv/f(v)");
    outf("v dv/dx = f(v):  x = " S_INT " v dv/f(v)");
    if (!ok1) outf(ANS "Time: doesn't converge (never reaches v1)");
    else outf(ANS "Time = %s s", fxd(T));
    if (!ok2) outf(ANS "Distance: doesn't converge");
    else outf(ANS "Distance = %s m", fxd(X));
    double w[4];
    int n = nsolve_fn(av, -1e4, 1e4, w, 4);
    for (int i = 0; i < n; i++) outf("Terminal speed (a = 0): v = %s", fxd(w[i]));
}

static void r_ax(void) {
    in_note("a = f(x): v dv/dx = f(x)");
    node *f = in_fn("a = f(x)", 'x');
    double x0 = in_real("x0"), v0 = in_real("v at x0"), x1 = in_real("Find v at x =");
    int ok;
    double I = integrate(f, 'x', x0, x1, &ok);
    outf("(1/2)v" S_SQ " - (1/2)v0" S_SQ " = " S_INT " f(x) dx = %s", fxd(I));
    double v2 = v0 * v0 + 2 * I;
    if (v2 < 0) outf(ANS "v" S_SQ " < 0: never reaches x = %s", fx(x1));
    else outf(ANS "v = " S_PM "%s m/s", fxd(sqrt(v2)));
}

static void r_at(void) {
    in_note("a = f(t)");
    node *f = in_fn("a = f(t)", 't');
    double v0 = in_real("v at t = 0"), x0 = in_real("x at t = 0"), t = in_real("Find at t =");
    ser s;
    const char *err;
    int ok;
    double V = v0 + integrate(f, 't', 0, t, &ok);
    int N = 200;
    double h = t / N, sx = 0;
    for (int j = 0; j <= N; j++) {
        double tt = j * h;
        double vv = v0 + integrate(f, 't', 0, tt, &ok);
        sx += vv * (j == 0 || j == N ? 1 : (j % 2 ? 4 : 2));
    }
    double X = x0 + sx * h / 3;
    (void)s; (void)err;
    outf("v = v0 + " S_INT " a dt,  x = x0 + " S_INT " v dt");
    outf(ANS "v(%s) = %s", fx(t), fxd(V));
    outf(ANS "x(%s) = %s", fx(t), fxd(X));
}

static void r_resist(void) {
    in_note("Mass m, driving force P, resistance kv or kv" S_SQ);
    double m = in_pos("Mass m"), P = in_real("Constant force (0 if none)"), k = in_pos("k");
    static const char *o[] = {"Resistance kv", "Resistance kv" S_SQ};
    int t = in_choice("Model", o, 2);
    double v0 = in_real("Initial speed"), v1 = in_real("Final speed");
    outf("m dv/dt = %s - %s", fx(P), t ? "kv" S_SQ : "kv");
    double T, X;
    if (t == 0) {
        /* m dv/(P - kv) = dt */
        outf("t = " S_INT " m/(P - kv) dv = -(m/k) ln|P - kv|");
        T = -(m / k) * log(fabs((P - k * v1) / (P - k * v0)));
        outf("x = " S_INT " mv/(P - kv) dv");
        X = (m / k) * (-(v1 - v0) - (P / k) * log(fabs((P - k * v1) / (P - k * v0))));
        if (P > 0) outf("Terminal speed P/k = %s", fx(P / k));
    } else {
        if (P == 0) {
            outf("t = (m/k)(1/v1 - 1/v0)");
            T = (m / k) * (1 / v1 - 1 / v0);
            outf("x = (m/k) ln(v0/v1)");
            X = (m / k) * log(v0 / v1);
        } else {
            double V = sqrt(fabs(P / k));
            outf("Terminal speed " S_SQRT "(P/k) = %s", fxd(V));
            T = (m / (2 * k * V)) * (log(fabs((V + v1) / (V - v1))) - log(fabs((V + v0) / (V - v0))));
            X = -(m / (2 * k)) * log(fabs((P - k * v1 * v1) / (P - k * v0 * v0)));
        }
    }
    if (T != T || isinf(T)) outf(ANS "Never reaches that speed");
    else outf(ANS "Time = %s s", fxd(T));
    if (X == X && !isinf(X)) outf(ANS "Distance = %s m", fxd(X));
}

static const calc_t rect_c[] = {
    {"a = f(v): time & distance", r_av},
    {"a = f(x): speed at x", r_ax},
    {"a = f(t): v and x at t", r_at},
    {"Resistance kv or kv" S_SQ, r_resist},
};

/* ======================= 2D MOMENTUM ======================= */
static void m_oblique(void) {
    in_note("Smooth spheres; line of centres along direction given");
    double m1 = in_pos("Mass A"), m2 = in_pos("Mass B");
    double u1[2], u2[2], d[2];
    in_vec("Velocity of A (x, y)", u1, 2);
    in_vec("Velocity of B (x, y)", u2, 2);
    in_vec("Line of centres direction", d, 2);
    double e = in_real("e");
    double L = hypot(d[0], d[1]);
    if (L == 0) fail("Direction can't be zero");
    double n[2] = {d[0] / L, d[1] / L}, t[2] = {-n[1], n[0]};
    double a1 = u1[0] * n[0] + u1[1] * n[1], a2 = u2[0] * n[0] + u2[1] * n[1];
    double p1 = u1[0] * t[0] + u1[1] * t[1], p2 = u2[0] * t[0] + u2[1] * t[1];
    outf("Components along line of centres: %s, %s", fx(a1), fx(a2));
    outf("Perpendicular components unchanged: %s, %s", fx(p1), fx(p2));
    double P = m1 * a1 + m2 * a2, s = e * (a1 - a2);
    double b1 = (P - m2 * s) / (m1 + m2), b2 = b1 + s;
    outf("CLM + NEL along the line:");
    outf(ANS "Along line after: A %s, B %s", fx(b1), fx(b2));
    double v1[2] = {b1 * n[0] + p1 * t[0], b1 * n[1] + p1 * t[1]}, v2[2] = {b2 * n[0] + p2 * t[0], b2 * n[1] + p2 * t[1]};
    outf(ANS "v_A = %s, speed %s", fvec(v1, 2), fxd(hypot(v1[0], v1[1])));
    outf(ANS "v_B = %s, speed %s", fvec(v2, 2), fxd(hypot(v2[0], v2[1])));
    double I[2] = {m2 * (v2[0] - u2[0]), m2 * (v2[1] - u2[1])};
    outf("Impulse on B = %s, magnitude %s", fvec(I, 2), fxd(hypot(I[0], I[1])));
    double ke0 = 0.5 * m1 * (u1[0] * u1[0] + u1[1] * u1[1]) + 0.5 * m2 * (u2[0] * u2[0] + u2[1] * u2[1]);
    double ke1 = 0.5 * m1 * (v1[0] * v1[0] + v1[1] * v1[1]) + 0.5 * m2 * (v2[0] * v2[0] + v2[1] * v2[1]);
    outf("KE lost = %s J", fxd(ke0 - ke1));
}

static void m_wall2d(void) {
    in_note("Smooth wall: perpendicular component times e");
    double m = in_pos("Mass");
    double u = in_pos("Speed u"), al = in_real("Angle between path and wall (deg)") * M_PI / 180, e = in_real("e");
    double par = u * cos(al), per = u * sin(al);
    outf("Parallel: %s (unchanged)", fd(par));
    outf("Perpendicular: %s " S_ARR " e" S_TIMES "%s = %s", fd(per), fd(per), fd(e * per));
    double v = hypot(par, e * per);
    outf(ANS "Speed after = %s", fxd(v));
    outf(ANS "Angle with wall = %s", fdeg(atan2(e * per, par)));
    outf(ANS "Impulse = %s Ns", fxd(m * (1 + e) * per));
    outf("KE lost = %s J", fxd(0.5 * m * (u * u - v * v)));
}

static void m_vec(void) {
    in_note("Momentum and impulse with vectors");
    double m = in_pos("Mass");
    double u[2], I[2];
    in_vec("Initial velocity (x, y)", u, 2);
    in_vec("Impulse (x, y)", I, 2);
    double v[2] = {u[0] + I[0] / m, u[1] + I[1] / m};
    outf("v = u + I/m");
    outf(ANS "v = %s, speed %s", fvec(v, 2), fxd(hypot(v[0], v[1])));
    outf("Direction = %s from x-axis", fdeg(atan2(v[1], v[0])));
}

static const calc_t mom_c[] = {
    {"Oblique collision of spheres", m_oblique},
    {"Oblique impact with a wall", m_wall2d},
    {"Impulse as a vector", m_vec},
};

/* ======================= CENTRES OF MASS ======================= */
static void cm_particles(void) {
    int n = in_int("Number of particles", 1, 12);
    double M = 0, sx = 0, sy = 0;
    for (int i = 0; i < n; i++) {
        double v[3];
        char lab[40] = "m, x, y for particle ";
        strcat(lab, fd(i + 1));
        in_vec(lab, v, 3);
        M += v[0]; sx += v[0] * v[1]; sy += v[0] * v[2];
    }
    if (M == 0) fail("Total mass 0");
    outf(S_SIG "m = %s,  " S_SIG "mx = %s,  " S_SIG "my = %s", fx(M), fx(sx), fx(sy));
    outf(ANS "x-bar = %s,  y-bar = %s", fx(sx / M), fx(sy / M));
}

static const char *shapes[] = {"Rectangle", "Triangle", "Semicircle lamina", "Sector lamina",
                               "Circular arc (wire)", "Solid hemisphere", "Hollow hemisphere", "Solid cone",
                               "Hollow cone", "Circle (disc)"};

static void cm_standard(void) {
    int k = in_choice("Shape", shapes, NEL(shapes));
    double r, h, a;
    switch (k) {
    case 0: outf(ANS "Centre of the rectangle"); break;
    case 1: outf(ANS "2/3 of the way along each median from the vertex"); outf("= average of the 3 vertices"); break;
    case 2: r = in_pos("r"); outf("4r/(3" S_PI ") from the centre"); outf(ANS "= %s", fxd(4 * r / (3 * M_PI))); outf("Area = %s", fxd(M_PI * r * r / 2)); break;
    case 3: r = in_pos("r"); a = in_pos("Half angle " S_AL " (rad)"); outf("2r sin" S_AL "/(3" S_AL ") from the centre"); outf(ANS "= %s", fxd(2 * r * sin(a) / (3 * a))); outf("Area = r" S_SQ S_AL " = %s", fxd(r * r * a)); break;
    case 4: r = in_pos("r"); a = in_pos("Half angle " S_AL " (rad)"); outf("r sin" S_AL "/" S_AL " from the centre"); outf(ANS "= %s", fxd(r * sin(a) / a)); outf("Length = 2r" S_AL " = %s", fxd(2 * r * a)); break;
    case 5: r = in_pos("r"); outf("3r/8 from the plane face"); outf(ANS "= %s", fxd(3 * r / 8)); break;
    case 6: r = in_pos("r"); outf("r/2 from the plane face"); outf(ANS "= %s", fxd(r / 2)); break;
    case 7: h = in_pos("Height h"); outf("h/4 from the base"); outf(ANS "= %s", fxd(h / 4)); break;
    case 8: h = in_pos("Height h"); outf("h/3 from the base"); outf(ANS "= %s", fxd(h / 3)); break;
    case 9: outf(ANS "At the centre"); break;
    }
}

static void cm_composite(void) {
    in_note("Enter parts. Use a negative mass/area for removed parts.");
    int n = in_int("Number of parts", 1, 10);
    double M = 0, sx = 0, sy = 0;
    for (int i = 0; i < n; i++) {
        double v[3];
        char lab[48] = "Part ";
        strcat(lab, fd(i + 1));
        strcat(lab, ": mass/area, x, y");
        in_vec(lab, v, 3);
        outf("Part %d: %s at (%s, %s)", i + 1, fx(v[0]), fx(v[1]), fx(v[2]));
        M += v[0]; sx += v[0] * v[1]; sy += v[0] * v[2];
    }
    outf("Table: " S_SIG "m = %s, " S_SIG "mx = %s, " S_SIG "my = %s", fx(M), fx(sx), fx(sy));
    if (fabs(M) < 1e-12) fail("Total is 0");
    double gx = sx / M, gy = sy / M;
    outf(ANS "Centre of mass (%s, %s)", fx(gx), fx(gy));
    static const char *o[] = {"Done", "Hang from a point"};
    if (in_choice("Suspend?", o, 2) == 0) return;
    double p[2];
    in_vec("Suspension point (x, y)", p, 2);
    double dx = gx - p[0], dy = gy - p[1];
    outf("COM hangs vertically below the point");
    outf("Angle of line PG to vertical: G is %s across, %s down", fx(fabs(dx)), fx(fabs(dy)));
    outf(ANS "tan" S_TH " = %s/%s,  " S_TH " = %s", fxp(fabs(dx)), fxp(fabs(dy)), fdeg(atan2(fabs(dx), fabs(dy))));
    outf("(angle between PG and the y-direction)");
}

static void cm_region(void) {
    static const char *o[] = {"Lamina under y = f(x)", "Solid of revolution about x-axis"};
    int k = in_choice("Body", o, 2);
    node *f = in_fn("y = f(x)", 'x');
    double a = in_real("From x ="), b = in_real("To x =");
    char s[240];
    const char *err;
    int ok;
    if (k == 0) {
        strcpy(s, "x("); strcat(s, ex_src(f)); strcat(s, ")");
        node *xf = ex_parse(s, &err);
        strcpy(s, "("); strcat(s, ex_src(f)); strcat(s, ")^2");
        node *f2 = ex_parse(s, &err);
        if (!xf || !f2) fail("Too long");
        double A = integrate(f, 'x', a, b, &ok), Mx = integrate(xf, 'x', a, b, &ok), My = 0.5 * integrate(f2, 'x', a, b, &ok);
        outf("A = " S_INT " y dx = %s", fxd(A));
        outf(S_INT " xy dx = %s", fxd(Mx));
        outf("(1/2)" S_INT " y" S_SQ " dx = %s", fxd(My));
        outf(ANS "x-bar = %s", fxd(Mx / A));
        outf(ANS "y-bar = %s", fxd(My / A));
    } else {
        strcpy(s, "("); strcat(s, ex_src(f)); strcat(s, ")^2");
        node *f2 = ex_parse(s, &err);
        strcpy(s, "x("); strcat(s, ex_src(f)); strcat(s, ")^2");
        node *xf2 = ex_parse(s, &err);
        if (!xf2 || !f2) fail("Too long");
        double V = integrate(f2, 'x', a, b, &ok), Mx = integrate(xf2, 'x', a, b, &ok);
        outf("V = " S_PI S_INT " y" S_SQ " dx = " S_PI " " S_TIMES " %s", fxd(V));
        outf(S_PI S_INT " xy" S_SQ " dx = " S_PI " " S_TIMES " %s", fxd(Mx));
        outf(ANS "x-bar = %s (on the axis)", fxd(Mx / V));
    }
}

static const calc_t com_c[] = {
    {"System of particles", cm_particles},
    {"Standard shapes", cm_standard},
    {"Composite body + suspension", cm_composite},
    {"Region under a curve / solid", cm_region},
};

/* ======================= RIGID BODIES ======================= */
static void rb_ladder(void) {
    in_note("Uniform ladder: smooth vertical wall, rough ground");
    double L = in_pos("Length"), W = in_pos("Weight of ladder (N)"), th = in_real("Angle with ground (deg)") * M_PI / 180;
    double Wm = in_real("Person's weight (0 if none)"), d = Wm > 0 ? in_real("Person's distance up ladder") : 0;
    outf("R (ground) = total weight = %s", fx(W + Wm));
    outf("Moments about foot:");
    outf("S" S_TIMES "L sin" S_TH " = W(L/2)cos" S_TH " + W'd cos" S_TH);
    double S_ = (W * L / 2 * cos(th) + Wm * d * cos(th)) / (L * sin(th));
    outf(ANS "S (wall) = %s N", fxd(S_));
    outf(ANS "Friction F = S = %s N", fxd(S_));
    outf(ANS "Least " S_MU " = F/R = %s", fxd(S_ / (W + Wm)));
    double mu = in_real(S_MU " at the ground (0 to skip)");
    if (mu > 0) {
        double Fmax = mu * (W + Wm);
        outf("Max friction = " S_MU "R = %s", fd(Fmax));
        if (Wm > 0) {
            double dmax = (Fmax * L * sin(th) - W * L / 2 * cos(th)) / (Wm * cos(th));
            if (dmax >= L) outf(ANS "Person can reach the top");
            else if (dmax < 0) outf(ANS "Slips even with no climbing");
            else outf(ANS "Slips when person is %s up", fxd(dmax));
        } else outf(ANS "%s", S_ <= Fmax + 1e-12 ? "Ladder is in equilibrium" : "Ladder slips");
    }
}

void rb_moments(void) {
    in_note("Rod along x-axis. Forces vertical (+ up). Find supports at A and B.");
    double L = in_pos("Rod length"), W = in_real("Rod weight (at centre unless entered)"), gx = in_real("Position of rod's weight");
    double xa = in_real("Support A at x ="), xb = in_real("Support B at x =");
    int n = in_int("Extra loads", 0, 6);
    double sumF = W, sumM = W * (gx - xa);
    for (int i = 0; i < n; i++) {
        double v[2];
        char lab[40] = "Load ";
        strcat(lab, fd(i + 1));
        strcat(lab, ": weight, x");
        in_vec(lab, v, 2);
        sumF += v[0]; sumM += v[0] * (v[1] - xa);
    }
    (void)L;
    if (xa == xb) fail("A and B must differ");
    outf("Moments about A: R_B(%s) = %s", fx(xb - xa), fx(sumM));
    double RB = sumM / (xb - xa), RA = sumF - RB;
    outf("Resolve: R_A + R_B = %s", fx(sumF));
    outf(ANS "R_A = %s N,  R_B = %s N", fx(RA), fx(RB));
    if (RA < -1e-12) outf("R_A < 0: rod tilts about B");
    if (RB < -1e-12) outf("R_B < 0: rod tilts about A");
}

static void rb_slide_topple(void) {
    in_note("Block (w wide, h tall) on a rough plane, angle increased");
    double w = in_pos("Width w"), h = in_pos("Height h"), mu = in_pos(S_MU);
    double ts = atan(mu), tt = atan(w / h);
    outf("Slides when tan" S_AL " > " S_MU ": " S_AL " = %s", fdeg(ts));
    outf("Topples when tan" S_AL " > w/h: " S_AL " = %s", fdeg(tt));
    outf(ANS "%s first", ts < tt ? "Slides" : ts > tt ? "Topples" : "Slides and topples together");
}

static const calc_t rigid_c[] = {
    {"Ladder against a wall", rb_ladder},
    {"Rod on two supports (moments)", rb_moments},
    {"Slide or topple on a slope", rb_slide_topple},
};

/* ======================= SHM ======================= */
static void s_basic(void) {
    static const char *o[] = {"Know " S_OM, "Know period T"};
    int k = in_choice("Given", o, 2);
    double w = in_pos(k == 0 ? S_OM : "Period T");
    if (k == 1) { w = 2 * M_PI / w; outf(S_OM " = 2" S_PI "/T = %s", fxd(w)); }
    double a = in_pos("Amplitude a");
    outf("x = a cos(" S_OM "t) or a sin(" S_OM "t)");
    outf(ANS "Period = %s s", fxd(2 * M_PI / w));
    outf(ANS "Max speed a" S_OM " = %s", fxd(a * w));
    outf(ANS "Max accel a" S_OM S_SQ " = %s", fxd(a * w * w));
    double x = in_real("Displacement x (for speed)");
    if (fabs(x) > a) fail("|x| > a");
    outf("v" S_SQ " = " S_OM S_SQ "(a" S_SQ " - x" S_SQ ")");
    outf(ANS "v = %s, |accel| = %s", fxd(w * sqrt(a * a - x * x)), fxd(w * w * fabs(x)));
    double x1 = in_real("Time from centre to x ="), x2 = in_real("... to x =");
    if (fabs(x1) > a || fabs(x2) > a) fail("Beyond amplitude");
    double t1 = asin(x1 / a) / w, t2 = asin(x2 / a) / w;
    outf("x = a sin(" S_OM "t): t = sin^-1(x/a)/" S_OM);
    outf(ANS "Time between = %s s", fxd(fabs(t2 - t1)));
}

static void s_elastic(void) {
    in_note("Particle on elastic string/spring, vertical");
    double m = in_pos("Mass m"), lam = in_pos("Modulus " S_LA), l = in_pos("Natural length l");
    double e = m * G * l / lam;
    outf("Equilibrium extension e = mgl/" S_LA " = %s", fxd(e));
    outf("Displaced x from equilibrium: m x'' = -(" S_LA "/l)x");
    double w = sqrt(lam / (m * l));
    outf(ANS S_OM S_SQ " = " S_LA "/(ml) = %s", fxd(w * w));
    outf(ANS "Period = 2" S_PI "/" S_OM " = %s s", fxd(2 * M_PI / w));
    double a = in_pos("Amplitude a (pulled down)");
    outf(ANS "Max speed = %s", fxd(a * w));
    if (a > e) outf("a > e: string goes slack above the natural length (not full SHM)");
}

static void s_pend(void) {
    double l = in_pos("Pendulum length l");
    outf("Small oscillations: " S_TH "'' = -(g/l)" S_TH);
    outf(ANS "Period = 2" S_PI S_SQRT "(l/g) = %s s", fxd(2 * M_PI * sqrt(l / G)));
}

static void s_damped(void) {
    in_note("x'' + kx' + " S_OM S_SQ "x = 0");
    double k = in_real("k"), w2 = in_pos(S_OM S_SQ);
    double d = k * k - 4 * w2;
    outf("Auxiliary: m" S_SQ " + %sm + %s = 0, disc = %s", fx(k), fx(w2), fx(d));
    if (d > 1e-12) {
        double m1 = (-k + sqrt(d)) / 2, m2 = (-k - sqrt(d)) / 2;
        outf(ANS "Heavy damping: x = Ae^(%st) + Be^(%st)", fx(m1), fx(m2));
    } else if (d > -1e-12) outf(ANS "Critical damping: x = (A + Bt)e^(%st)", fx(-k / 2));
    else {
        double p = -k / 2, q = sqrt(-d) / 2;
        outf(ANS "Light damping: x = e^(%st)(A cos %st + B sin %st)", fx(p), fx(q), fx(q));
        outf("Period of oscillation = %s s", fxd(2 * M_PI / q));
    }
}

static const calc_t shm_c[] = {
    {"SHM: speed, period, timing", s_basic},
    {"Elastic string / spring SHM", s_elastic},
    {"Simple pendulum", s_pend},
    {"Damped oscillations", s_damped},
};

const topic_t fm_u6[] = {
    {"Rectilinear motion (variable a)", rect_c, NEL(rect_c)},
    {"Momentum in 2D", mom_c, NEL(mom_c)},
    {"Centres of mass", com_c, NEL(com_c)},
    {"Rigid bodies in equilibrium", rigid_c, NEL(rigid_c)},
    {"Simple harmonic motion", shm_c, NEL(shm_c)},
};
const int fm_u6_n = NEL(fm_u6);
