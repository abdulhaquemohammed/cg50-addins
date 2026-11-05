/* complex arithmetic, output buffer, number formatting */
#include "core.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

/* ================= complex ================= */
cx cadd(cx a, cx b) { return C(a.re + b.re, a.im + b.im); }
cx csub(cx a, cx b) { return C(a.re - b.re, a.im - b.im); }
cx cmul(cx a, cx b) { return C(a.re * b.re - a.im * b.im, a.re * b.im + a.im * b.re); }
cx cdiv(cx a, cx b) {
    double d = b.re * b.re + b.im * b.im;
    return C((a.re * b.re + a.im * b.im) / d, (a.im * b.re - a.re * b.im) / d);
}
cx cneg(cx a) { return C(-a.re, -a.im); }
cx cconj(cx a) { return C(a.re, -a.im); }
double cabs_(cx a) { return hypot(a.re, a.im); }
double carg_(cx a) { return atan2(a.im, a.re); }
cx cpolar(double r, double t) { return C(r * cos(t), r * sin(t)); }
cx cexp_(cx a) { return cpolar(exp(a.re), a.im); }
cx clog_(cx a) { return C(log(cabs_(a)), carg_(a)); }
cx csqrt_(cx a) {
    if (a.im == 0 && a.re >= 0) return R(sqrt(a.re));
    return cpolar(sqrt(cabs_(a)), carg_(a) / 2);
}
cx cpowi(cx a, int n) {
    cx r = R(1);
    int neg = n < 0;
    if (neg) n = -n;
    while (n) { if (n & 1) r = cmul(r, a); a = cmul(a, a); n >>= 1; }
    return neg ? cdiv(R(1), r) : r;
}
cx cpow_(cx a, cx b) {
    if (b.im == 0 && b.re == floor(b.re) && fabs(b.re) < 1e6) return cpowi(a, (int)b.re);
    if (a.im == 0 && b.im == 0 && a.re >= 0) return R(pow(a.re, b.re));
    if (a.im == 0 && b.im == 0 && a.re < 0) {
        /* real odd root of negative, e.g. (-8)^(1/3) */
        long p, q;
        if (ratapprox(b.re, 50, &p, &q) && (q % 2 == 1))
            return R((p % 2 ? -1 : 1) * pow(-a.re, b.re));
    }
    if (a.re == 0 && a.im == 0) return R(0);
    return cexp_(cmul(b, clog_(a)));
}

/* ================= output buffer ================= */
#define OUT_LINES 600
#define OUT_CHARS 40000
static char obuf[OUT_CHARS];
static int ooff[OUT_LINES];
static int on, opos;

void out_reset(void) { on = 0; opos = 0; }
int out_count(void) { return on; }
const char *out_get(int i) { return obuf + ooff[i]; }

void out_line(const char *s) {
    int L = strlen(s);
    if (on >= OUT_LINES || opos + L + 1 >= OUT_CHARS) return;
    ooff[on++] = opos;
    memcpy(obuf + opos, s, L + 1);
    opos += L + 1;
}

void outf(const char *fmt, ...) {
    char line[400];
    int n = 0;
    va_list ap;
    va_start(ap, fmt);
    for (const char *p = fmt; *p; p++) {
        if (*p == '\n') { line[n] = 0; out_line(line); n = 0; continue; }
        if (*p == '%' && p[1]) {
            p++;
            const char *s = NULL;
            char tmp[24];
            if (*p == 's') s = va_arg(ap, const char *);
            else if (*p == 'd') {
                long v = va_arg(ap, int);
                int k = 0, neg = v < 0;
                unsigned long u = neg ? -v : v;
                char r[22];
                do { r[k++] = '0' + u % 10; u /= 10; } while (u);
                int j = 0;
                if (neg) tmp[j++] = '-';
                while (k) tmp[j++] = r[--k];
                tmp[j] = 0;
                s = tmp;
            } else if (*p == 'c') { tmp[0] = (char)va_arg(ap, int); tmp[1] = 0; s = tmp; }
            else if (*p == '%') { tmp[0] = '%'; tmp[1] = 0; s = tmp; }
            if (s) while (*s && n < 398) line[n++] = *s++;
            continue;
        }
        if (n < 398) line[n++] = *p;
    }
    va_end(ap);
    line[n] = 0;
    if (n) out_line(line);
}

void out_hr(void) { out_line("--"); }

/* ================= formatting ================= */
static char sbufs[48][SBUF];
static int sidx;
char *sbuf(void) { sidx = (sidx + 1) % 48; sbufs[sidx][0] = 0; return sbufs[sidx]; }

long lgcd(long a, long b) {
    if (a < 0) a = -a;
    if (b < 0) b = -b;
    while (b) { long t = a % b; a = b; b = t; }
    return a;
}

static void itoa_(long v, char *o) {
    char r[24];
    int k = 0, neg = v < 0;
    unsigned long u = neg ? -(unsigned long)v : (unsigned long)v;
    do { r[k++] = '0' + u % 10; u /= 10; } while (u);
    if (neg) *o++ = '-';
    while (k) *o++ = r[--k];
    *o = 0;
}

const char *fdn(double x, int sig) {
    char *o = sbuf();
    if (x != x) { strcpy(o, "undefined"); return o; }
    if (isinf(x)) { strcpy(o, x > 0 ? S_INF : "-" S_INF); return o; }
    if (fabs(x) < 1e-13) { strcpy(o, "0"); return o; }
    char *p = o;
    if (x < 0) { *p++ = '-'; x = -x; }
    int e = (int)floor(log10(x));
    double m = x / pow(10, e);
    long long digits = (long long)floor(m * pow(10, sig - 1) + 0.5);
    long long lim = 1;
    for (int i = 0; i < sig; i++) lim *= 10;
    if (digits >= lim) { digits /= 10; e++; }
    char d[24];
    for (int i = sig - 1; i >= 0; i--) { d[i] = '0' + digits % 10; digits /= 10; }
    d[sig] = 0;
    int last = sig - 1;
    while (last > 0 && d[last] == '0') last--;
    if (e >= -4 && e < 10) {
        if (e >= 0) {
            for (int i = 0; i <= e; i++) *p++ = i < sig ? d[i] : '0';
            if (last > e) { *p++ = '.'; for (int i = e + 1; i <= last; i++) *p++ = d[i]; }
        } else {
            *p++ = '0'; *p++ = '.';
            for (int i = 0; i < -e - 1; i++) *p++ = '0';
            for (int i = 0; i <= last; i++) *p++ = d[i];
        }
        *p = 0;
    } else {
        *p++ = d[0];
        if (last > 0) { *p++ = '.'; for (int i = 1; i <= last; i++) *p++ = d[i]; }
        *p++ = 'E';
        itoa_(e, p);
    }
    return o;
}
const char *fd(double x) { return fdn(x, 6); }

const char *fdp(double x, int dp) {
    char *o = sbuf();
    if (x != x || isinf(x)) return fd(x);
    double s = pow(10, dp);
    long long v = (long long)llround(fabs(x) * s);
    char *p = o;
    if (x < 0 && v) *p++ = '-';
    long long ip = v / (long long)s, fp = v % (long long)s;
    itoa_((long)ip, p);
    p += strlen(p);
    if (dp > 0) {
        *p++ = '.';
        for (int i = dp - 1; i >= 0; i--) { p[i] = '0' + fp % 10; fp /= 10; }
        p += dp;
    }
    *p = 0;
    return o;
}

static double rat_tol = 1e-9;
int ratapprox(double x, long maxq, long *pp, long *qq) {
    if (x != x || isinf(x) || fabs(x) > 1e9) return 0;
    double tol = rat_tol * (1 + fabs(x));
    long p0 = 0, q0 = 1, p1 = 1, q1 = 0;
    double y = x;
    for (int it = 0; it < 40; it++) {
        double a = floor(y);
        long p2 = (long)a * p1 + p0, q2 = (long)a * q1 + q0;
        if (q2 > maxq || q2 <= 0) break;
        p0 = p1; q0 = q1; p1 = p2; q1 = q2;
        if (fabs(x - (double)p1 / q1) < tol) { *pp = p1; *qq = q1; return 1; }
        double f = y - a;
        if (f < 1e-15) break;
        y = 1 / f;
    }
    return 0;
}

static void frac_str(long p, long q, char *o) {
    if (q == 1) itoa_(p, o);
    else { itoa_(p, o); strcat(o, "/"); itoa_(q, o + strlen(o)); }
}

/* coefficient (num/den) times symbol, e.g. 3√2/4, -π/6 */
static void coefsym(long num, long den, const char *sym, char *o) {
    o[0] = 0;
    if (num < 0) { strcat(o, "-"); num = -num; }
    if (num != 1) itoa_(num, o + strlen(o));
    strcat(o, sym);
    if (den != 1) { strcat(o, "/"); itoa_(den, o + strlen(o)); }
}

static int squarefree_part(long n, long *k, long *m) {
    long kk = 1;
    for (long f = 2; f * f <= n; f++)
        while (n % (f * f) == 0) { n /= f * f; kk *= f; }
    *k = kk; *m = n;
    return 1;
}

static int exact_str(double x, char *o, int allow_ln);

static int exact_str(double x, char *o, int allow_ln) {
    long p, q;
    if (x != x || isinf(x)) return 0;
    if (fabs(x) < 1e-11) { strcpy(o, "0"); return 1; }
    if (ratapprox(x, 1000, &p, &q)) { frac_str(p, q, o); return 1; }
    /* k√m / q */
    double x2 = x * x;
    if (ratapprox(x2, 1000, &p, &q) && p > 0 && p * q < 1000000) {
        long k, m;
        squarefree_part(p * q, &k, &m);
        if (m > 1 && m <= 300) {
            long g = lgcd(k, q);
            long num = k / g, den = q / g;
            char sym[16];
            strcpy(sym, S_SQRT);
            itoa_(m, sym + strlen(sym));
            coefsym(x < 0 ? -num : num, den, sym, o);
            return 1;
        }
    }
    /* multiple of π */
    if (ratapprox(x / M_PI, 36, &p, &q) && labs(p) <= 720) { coefsym(p, q, S_PI, o); return 1; }
    /* (a + k√m)/q */
    static const int sf[] = {2, 3, 5, 6, 7, 10, 11, 13, 14, 15, 17, 19, 21, 22, 23, 26, 29, 30};
    for (int qq = 1; qq <= 8; qq++)
        for (int i = 0; i < NEL(sf); i++) {
            double r = sqrt((double)sf[i]);
            double kr = floor(qq * x / r + 0.5);
            for (int k = -24; k <= 24; k++) {
                if (k == 0) continue;
                double y = qq * x - k * r;
                double a = floor(y + 0.5);
                if (fabs(y - a) < 2e-11 * (1 + fabs(qq * x))) {
                    long g = lgcd(lgcd((long)a, k), qq);
                    long A = (long)a / g, K = k / g, Q = qq / g;
                    char t[48];
                    t[0] = 0;
                    if (A) itoa_(A, t);
                    if (A) strcat(t, K < 0 ? " - " : " + ");
                    else if (K < 0) strcat(t, "-");
                    if (labs(K) != 1) itoa_(labs(K), t + strlen(t));
                    strcat(t, S_SQRT);
                    itoa_(sf[i], t + strlen(t));
                    if (Q == 1) strcpy(o, t);
                    else if (A) { strcpy(o, "("); strcat(o, t); strcat(o, ")/"); itoa_(Q, o + strlen(o)); }
                    else { strcpy(o, t); strcat(o, "/"); itoa_(Q, o + strlen(o)); }
                    return 1;
                }
            }
            (void)kr;
        }
    /* A + Bπ with A rational or k√m/q (tight tolerance: heuristic) */
    rat_tol = 2e-11;
    for (int qb = 1; qb <= 12; qb++)
        for (int kb = -24; kb <= 24; kb++) {
            if (kb == 0 || lgcd(kb, qb) != 1) continue;
            double y = x - (double)kb / qb * M_PI;
            long pa, qa;
            char t[64];
            int okA = 0;
            if (ratapprox(y, 100, &pa, &qa)) { frac_str(pa, qa, t); okA = 1; }
            else if (ratapprox(y * y, 100, &pa, &qa) && pa > 0 && pa * qa < 10000) {
                long k, m;
                squarefree_part(pa * qa, &k, &m);
                if (m > 1 && m <= 30) {
                    long g = lgcd(k, qa);
                    char sym[16];
                    strcpy(sym, S_SQRT);
                    itoa_(m, sym + strlen(sym));
                    coefsym(y < 0 ? -(k / g) : k / g, qa / g, sym, t);
                    okA = 1;
                }
            }
            if (okA) {
                char pb[32];
                coefsym(labs(kb), qb, S_PI, pb);
                strcpy(o, t);
                strcat(o, kb < 0 ? " - " : " + ");
                strcat(o, pb);
                rat_tol = 1e-9;
                return 1;
            }
        }
    rat_tol = 1e-9;
    if (allow_ln && x > -30 && x < 30 && fabs(x) > 1e-6) {
        double ex = exp(x);
        char t[64];
        if (ratapprox(ex, 20, &p, &q) && p > 0 && p <= 2000) {
            frac_str(p, q, t);
            strcpy(o, "ln(");
            strcat(o, t);
            strcat(o, ")");
            return 1;
        }
        /* ln(a + k√m) with small integers, tight tolerance */
        static const int sm[] = {2, 3, 5, 6, 7, 10};
        for (int i = 0; i < 6; i++)
            for (int kk = 1; kk <= 4; kk++)
                for (int aa = -6; aa <= 12; aa++) {
                    double v = aa + kk * sqrt((double)sm[i]);
                    if (v <= 0) continue;
                    if (fabs(v - ex) < 2e-11 * ex) {
                        char u[40] = "ln(";
                        if (aa) { itoa_(aa, u + 3); strcat(u, " + "); }
                        if (kk != 1) itoa_(kk, u + strlen(u));
                        strcat(u, S_SQRT);
                        itoa_(sm[i], u + strlen(u));
                        strcat(u, ")");
                        strcpy(o, u);
                        return 1;
                    }
                }
    }
    return 0;
}

/* small cache: exact_str is slow on the calculator (soft float) */
#define XC 24
static double xc_v[XC];
static char xc_s[XC][64];
static signed char xc_ok[XC];
static int xc_n, xc_i;
static int exact_cached(double x, char *o) {
    for (int i = 0; i < xc_n; i++)
        if (xc_v[i] == x) { if (xc_ok[i]) strcpy(o, xc_s[i]); return xc_ok[i]; }
    char t[200];
    int ok = exact_str(x, t, 1);
    if (ok && strlen(t) >= 64) ok = 0;
    xc_v[xc_i] = x; xc_ok[xc_i] = ok;
    if (ok) strcpy(xc_s[xc_i], t);
    xc_i = (xc_i + 1) % XC;
    if (xc_n < XC) xc_n++;
    if (ok) strcpy(o, t);
    return ok;
}

int is_exact(double x) { char t[200]; return exact_cached(x, t); }

const char *fx(double x) {
    char *o = sbuf();
    if (exact_cached(x, o)) return o;
    return fd(x);
}

const char *fxd(double x) {
    char *o = sbuf();
    char t[200];
    if (exact_cached(x, t)) {
        long p, q;
        if (ratapprox(x, 1000, &p, &q) && q == 1) { strcpy(o, t); return o; }
        strcpy(o, t);
        strcat(o, " " S_APX " ");
        strcat(o, fd(x));
        return o;
    }
    return fd(x);
}

const char *fc(cx z) {
    char *o = sbuf();
    double re = z.re, im = z.im;
    double sc = 1 + fabs(re) + fabs(im);
    if (fabs(im) < 1e-11 * sc) im = 0;
    if (fabs(re) < 1e-11 * sc) re = 0;
    if (im == 0) { strcpy(o, fx(re)); return o; }
    const char *is = fx(fabs(im));
    char ip[80];
    if (strcmp(is, "1") == 0) strcpy(ip, "i");
    else if (strchr(is, '/') || strchr(is, ' ') || strchr(is, 'E')) { strcpy(ip, "("); strcat(ip, is); strcat(ip, ")i"); }
    else { strcpy(ip, is); strcat(ip, "i"); }
    if (re == 0) { if (im < 0) { strcpy(o, "-"); strcat(o, ip); } else strcpy(o, ip); return o; }
    strcpy(o, fx(re));
    strcat(o, im < 0 ? " - " : " + ");
    strcat(o, ip);
    return o;
}

const char *fdeg(double t) {
    char *o = sbuf();
    strcpy(o, fd(t * 180 / M_PI));
    strcat(o, S_DEG);
    return o;
}

const char *fang(double t) {
    char *o = sbuf();
    char e[96];
    long p, q;
    if (ratapprox(t / M_PI, 36, &p, &q) && labs(p) <= 720) {
        if (p == 0) strcpy(e, "0");
        else coefsym(p, q, S_PI, e);
        strcpy(o, e);
    } else strcpy(o, fd(t));
    strcat(o, " rad (");
    strcat(o, fdeg(t));
    strcat(o, ")");
    return o;
}

/* term like " + 3x²" ; first term has no leading " + " */
const char *fcoef(double c, const char *term, int first) {
    char *o = sbuf();
    if (!first && fabs(c) < 1e-12) return o;
    const char *cs = fx(fabs(c));
    int one = strcmp(cs, "1") == 0 && term && term[0];
    int paren = term && term[0] && (strchr(cs, ' ') != NULL || strchr(cs, '/') != NULL);
    if (first) { if (c < 0) strcpy(o, "-"); }
    else strcpy(o, c < 0 ? " - " : " + ");
    if (!one) {
        if (paren) strcat(o, "(");
        strcat(o, cs);
        if (paren) strcat(o, ")");
    }
    if (term) strcat(o, term);
    return o;
}

static const char *supnum(int k) {
    static const char *s[] = {"", "", S_SQ, S_CU};
    if (k >= 0 && k <= 3) return s[k];
    char *o = sbuf();
    strcpy(o, "^");
    itoa_(k, o + 1);
    return o;
}
const char *sup(int k) { return supnum(k); }

const char *fpoly(const double *c, int deg, const char *var) {
    char *o = sbuf();
    int first = 1;
    for (int k = deg; k >= 0; k--) {
        if (fabs(c[k]) < 1e-11) continue;
        char term[24] = "";
        if (k >= 1) { strcpy(term, var); if (k > 1) strcat(term, supnum(k)); }
        const char *t = fcoef(c[k], term, first);
        if (strlen(o) + strlen(t) < SBUF - 1) strcat(o, t);
        first = 0;
    }
    if (first) strcpy(o, "0");
    return o;
}

/* exact, wrapped in brackets if negative or compound */
const char *fxp(double x) {
    const char *s = fx(x);
    if (x < 0 || strchr(s, ' ') || strchr(s, '/')) {
        char *o = sbuf();
        strcpy(o, "(");
        strcat(o, s);
        strcat(o, ")");
        return o;
    }
    return s;
}

/* "(x - a)" with sign handling; "x" if a = 0 */
const char *fshift(const char *var, double a) {
    char *o = sbuf();
    if (fabs(a) < 1e-12) { strcpy(o, var); return o; }
    strcpy(o, "(");
    strcat(o, var);
    strcat(o, a > 0 ? " - " : " + ");
    strcat(o, fx(fabs(a)));
    strcat(o, ")");
    return o;
}
/* complex in brackets if compound: "(1 + 2i)" */
const char *fcp(cx z) {
    const char *s = fc(z);
    if (strchr(s, ' ') || s[0] == '-') { char *o = sbuf(); strcpy(o, "("); strcat(o, s); strcat(o, ")"); return o; }
    return s;
}
