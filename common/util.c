/* shared helpers: vector/matrix printing, polynomial tools */
#include "core.h"
#include "util.h"
#include <string.h>
#include <stdlib.h>

const char *fvec(const double *v, int n) {
    char *o = sbuf();
    strcpy(o, "(");
    for (int i = 0; i < n; i++) {
        if (i) strcat(o, ", ");
        const char *s = fx(v[i]);
        if (strlen(o) + strlen(s) < SBUF - 4) strcat(o, s);
    }
    strcat(o, ")");
    return o;
}

void out_mat(const char *name, const double *m, int r, int c) {
    for (int i = 0; i < r; i++) {
        char *o = sbuf();
        if (i == 0 && name) { strcpy(o, name); strcat(o, " = "); }
        else if (name) strcpy(o, "     ");
        strcat(o, "[ ");
        for (int j = 0; j < c; j++) {
            if (j) strcat(o, "   ");
            strcat(o, fx(m[i * c + j]));
        }
        strcat(o, " ]");
        out_line(o);
    }
}

/* ---------- polynomials (ascending coefficients) ---------- */
double peval_r(const double *a, int d, double x) {
    double r = 0;
    for (int k = d; k >= 0; k--) r = r * x + a[k];
    return r;
}

/* divide a (deg da) by b (deg db): q (deg da-db), rem (deg db-1) */
void pdiv(const double *a, int da, const double *b, int db, double *q, double *rem) {
    double r[16];
    memcpy(r, a, sizeof(double) * (da + 1));
    for (int k = da - db; k >= 0; k--) {
        q[k] = r[k + db] / b[db];
        for (int j = 0; j <= db; j++) r[k + j] -= q[k] * b[j];
    }
    if (rem) for (int j = 0; j < db; j++) rem[j] = r[j];
}

void pmul(const double *a, int da, const double *b, int db, double *o) {
    double t[24] = {0};
    for (int i = 0; i <= da; i++) for (int j = 0; j <= db; j++) t[i + j] += a[i] * b[j];
    memcpy(o, t, sizeof(double) * (da + db + 1));
}

/* scale polynomial to integer coefficients with positive leading coefficient, if possible */
int pintscale(double *a, int d) {
    long L = 1;
    for (int k = 0; k <= d; k++) {
        long p, q;
        if (fabs(a[k]) < 1e-12) { a[k] = 0; continue; }
        if (!ratapprox(a[k], 1000, &p, &q)) return 0;
        L = L / lgcd(L, q) * q;
        if (L > 1000000) return 0;
    }
    long g = 0;
    for (int k = 0; k <= d; k++) { a[k] = floor(a[k] * L + 0.5); g = lgcd(g, (long)fabs(a[k])); }
    if (g > 1) for (int k = 0; k <= d; k++) a[k] /= g;
    int top = d;
    while (top > 0 && a[top] == 0) top--;
    if (a[top] < 0) for (int k = 0; k <= d; k++) a[k] = -a[k];
    return 1;
}

/* factor integer-ish polynomial into small rational-root linear factors + remainder */
const char *pfactor(const double *a0, int d, const char *var) {
    double a[16];
    memcpy(a, a0, sizeof(double) * (d + 1));
    char *o = sbuf();
    double lead = 1;
    /* make integer */
    double sc[16];
    memcpy(sc, a, sizeof(double) * (d + 1));
    int top = d;
    while (top > 0 && fabs(a[top]) < 1e-12) top--;
    d = top;
    if (!pintscale(sc, d)) { strcpy(o, fpoly(a0, d, var)); return o; }
    /* overall constant = a[d]/sc[d] */
    lead = a[d] / sc[d];
    memcpy(a, sc, sizeof(double) * (d + 1));
    char fac[SBUF] = "";
    int zeros = 0;
    while (d > 0 && fabs(a[0]) < 1e-9) { memmove(a, a + 1, sizeof(double) * d); d--; zeros++; }
    if (zeros) { strcat(fac, var); if (zeros > 1) strcat(fac, sup(zeros)); }
    /* try roots p/q, q in 1..6, p in -30..30, with multiplicity */
    for (int q = 1; q <= 6 && d > 0; q++)
        for (int p = -30; p <= 30 && d > 0; p++) {
            if (p == 0 || lgcd(p, q) != 1) continue;
            double x = (double)p / q;
            int mult = 0;
            while (d > 0 && fabs(peval_r(a, d, x)) < 1e-7 * (1 + fabs(a[d]))) {
                double b[2] = {(double)-p, (double)q}, qt[16];
                pdiv(a, d, b, 1, qt, NULL);
                memcpy(a, qt, sizeof(double) * d);
                d--;
                mult++;
            }
            if (!mult) continue;
            char t[40];
            strcpy(t, "(");
            if (q != 1) strcat(t, fd(q));
            strcat(t, var);
            strcat(t, -p > 0 ? " + " : " - ");
            strcat(t, fd(labs(p)));
            strcat(t, ")");
            if (mult > 1) strcat(t, sup(mult));
            if (strlen(fac) + strlen(t) < SBUF - 20) strcat(fac, t);
        }
    /* remainder */
    double tot = lead;
    if (d == 0) tot *= a[0];
    if (d > 0) {
        /* pull out integer content */
        long g = 0;
        for (int k = 0; k <= d; k++) g = lgcd(g, (long)fabs(floor(a[k] + 0.5)));
        if (g > 1) { for (int k = 0; k <= d; k++) a[k] /= g; tot *= g; }
        if (a[d] < 0) { for (int k = 0; k <= d; k++) a[k] = -a[k]; tot = -tot; }
    }
    o[0] = 0;
    long p, q;
    if (ratapprox(tot, 10000, &p, &q)) {
        if (p == -1) strcat(o, "-");
        else if (p != 1) strcat(o, fd(p));
        if (q != 1 && !(fac[0] || d > 0)) { strcat(o, "/"); strcat(o, fd(q)); }
        strcat(o, fac);
        if (d > 0) {
            if (fac[0] || p != 1) strcat(o, "(");
            strcat(o, fpoly(a, d, var));
            if (fac[0] || p != 1) strcat(o, ")");
        }
        if (q != 1 && (fac[0] || d > 0)) { strcat(o, "/"); strcat(o, fd(q)); }
    } else {
        strcpy(o, fpoly(a0, top, var));
    }
    if (!o[0]) strcpy(o, "1");
    return o;
}

/* ---------- 3D line / plane input ---------- */
void in_line3(const char *name, double *a, double *d) {
    char lab[48];
    static const char *opt[] = {"Point + direction", "Two points"};
    strcpy(lab, name);
    strcat(lab, " given by");
    int c = in_choice(lab, opt, 2);
    if (c == 0) {
        strcpy(lab, name); strcat(lab, ": point a"); in_vec(lab, a, 3);
        strcpy(lab, name); strcat(lab, ": direction d"); in_vec(lab, d, 3);
    } else {
        double b[3];
        strcpy(lab, name); strcat(lab, ": point A"); in_vec(lab, a, 3);
        strcpy(lab, name); strcat(lab, ": point B"); in_vec(lab, b, 3);
        for (int i = 0; i < 3; i++) d[i] = b[i] - a[i];
        outf("%s: d = B - A = %s", name, fvec(d, 3));
    }
    if (norm3(d) < 1e-12) fail("Direction vector can't be zero");
}

void in_plane3(const char *name, double *n, double *dd) {
    char lab[48];
    static const char *opt[] = {"ax + by + cz = d", "Point + normal", "Three points", "Point + 2 directions"};
    strcpy(lab, name);
    strcat(lab, " given by");
    int c = in_choice(lab, opt, 4);
    if (c == 0) {
        double v[4];
        strcpy(lab, name); strcat(lab, ": a, b, c, d"); in_vec(lab, v, 4);
        n[0] = v[0]; n[1] = v[1]; n[2] = v[2]; *dd = v[3];
    } else if (c == 1) {
        double p[3];
        strcpy(lab, name); strcat(lab, ": point a"); in_vec(lab, p, 3);
        strcpy(lab, name); strcat(lab, ": normal n"); in_vec(lab, n, 3);
        *dd = dot3(p, n);
        outf("%s: d = a" S_DOT "n = %s", name, fx(*dd));
    } else if (c == 2) {
        double A[3], B[3], Cc[3], u[3], v[3];
        strcpy(lab, name); strcat(lab, ": point A"); in_vec(lab, A, 3);
        strcpy(lab, name); strcat(lab, ": point B"); in_vec(lab, B, 3);
        strcpy(lab, name); strcat(lab, ": point C"); in_vec(lab, Cc, 3);
        for (int i = 0; i < 3; i++) { u[i] = B[i] - A[i]; v[i] = Cc[i] - A[i]; }
        cross3(u, v, n);
        outf("%s: AB = %s, AC = %s", name, fvec(u, 3), fvec(v, 3));
        outf("n = AB " S_TIMES " AC = %s", fvec(n, 3));
        *dd = dot3(A, n);
        outf("d = A" S_DOT "n = %s", fx(*dd));
    } else {
        double A[3], u[3], v[3];
        strcpy(lab, name); strcat(lab, ": point a"); in_vec(lab, A, 3);
        strcpy(lab, name); strcat(lab, ": direction 1"); in_vec(lab, u, 3);
        strcpy(lab, name); strcat(lab, ": direction 2"); in_vec(lab, v, 3);
        cross3(u, v, n);
        outf("n = d1 " S_TIMES " d2 = %s", fvec(n, 3));
        *dd = dot3(A, n);
        outf("d = a" S_DOT "n = %s", fx(*dd));
    }
    if (norm3(n) < 1e-12) fail("Normal is zero: points/directions are collinear");
    /* simplify to integer normal if possible */
    double t[4] = {*dd, n[0], n[1], n[2]};
    double s[4];
    memcpy(s, t, sizeof s);
    if (pintscale(s, 3)) {
        /* pintscale makes 'leading' (n[2]) positive; keep sign of first nonzero normal comp instead */
        double ratio = 0;
        for (int i = 1; i < 4; i++) if (fabs(t[i]) > 1e-12) { ratio = s[i] / t[i]; break; }
        if (ratio != 0) {
            int sg = 1;
            for (int i = 1; i < 4; i++) if (fabs(s[i]) > 1e-12) { sg = s[i] < 0 ? -1 : 1; break; }
            if (sg < 0) for (int i = 0; i < 4; i++) s[i] = -s[i];
            *dd = s[0]; n[0] = s[1]; n[1] = s[2]; n[2] = s[3];
        }
    }
}

const char *fplane(const double *n, double d) {
    char *o = sbuf();
    static const char *v[] = {"x", "y", "z"};
    int first = 1;
    for (int i = 0; i < 3; i++) {
        if (fabs(n[i]) < 1e-12) continue;
        strcat(o, fcoef(n[i], v[i], first));
        first = 0;
    }
    strcat(o, " = ");
    strcat(o, fx(d));
    return o;
}

void out_line3(const char *name, const double *a, const double *d) {
    outf("%s: r = %s + " S_LA "%s", name, fvec(a, 3), fvec(d, 3));
    /* Cartesian */
    char *o = sbuf();
    static const char *v[] = {"x", "y", "z"};
    char extra[80] = "";
    int first = 1;
    for (int i = 0; i < 3; i++) {
        char t[64];
        if (fabs(d[i]) < 1e-12) {
            if (extra[0]) strcat(extra, ", ");
            strcat(extra, v[i]);
            strcat(extra, " = ");
            strcat(extra, fx(a[i]));
            continue;
        }
        if (fabs(a[i]) > 1e-12) strcpy(t, fshift(v[i], a[i]));
        else strcpy(t, v[i]);
        const char *ds = fx(d[i]);
        if (strcmp(ds, "1") != 0) {
            strcat(t, "/");
            if (strchr(ds, ' ') || strchr(ds, '/') || ds[0] == '-') { strcat(t, "("); strcat(t, ds); strcat(t, ")"); }
            else strcat(t, ds);
        }
        if (!first) strcat(o, " = ");
        strcat(o, t);
        first = 0;
    }
    if (extra[0]) { if (!first) strcat(o, ", "); strcat(o, extra); }
    outf("  Cartesian: %s", o);
}

static void help_keys(void) {
    outf(HEAD "Typing");
    outf("Numbers, + - " S_TIMES " / ( ) ^ , as normal");
    outf("x" S_SQ " key: " S_SQ "     SHIFT x" S_SQ ": " S_SQRT "(");
    outf("SHIFT " S_TIMES "10^x: " S_PI "     " S_TIMES "10^x: E (e.g. 3E-4)");
    outf("SHIFT 0: i (complex numbers)");
    outf("ln, log, sin, cos, tan keys type functions");
    outf("SHIFT ln: e^(   SHIFT sin: arcsin( etc.");
    outf("X," S_TH ",T key: the variable (x, t, " S_TH "...)");
    outf("ALPHA + key: letters (e.g. ALPHA (-) = y)");
    outf("F1-F5: e, sinh, cosh, tanh, abs; F6: more");
    outf(HEAD "Editing");
    outf("DEL: backspace   AC/ON: clear line");
    outf("Left/Right: move cursor   EXE: enter");
    outf("EXIT: back to the menu");
    outf("Vectors/lists: separate values with commas");
    outf(HEAD "Results");
    outf("Red lines are answers. Up/Down scroll,");
    outf("Left/Right page, F6/EXE: run again");
    outf(HEAD "Notes");
    outf("Exact forms (fractions, " S_SQRT ", " S_PI ", ln) shown");
    outf("when recognised, with the decimal.");
    outf("Disabled in exam mode: practice tool only.");
}
static const calc_t help_c[] = {{"Keys & how to use", help_keys}};
const topic_t help_topics[] = {{"Help", help_c, 1}};
