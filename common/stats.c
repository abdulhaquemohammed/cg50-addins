/* probability distributions & special functions */
#include "core.h"
#include "stats.h"
#include <string.h>

double lgam(double x) { return lgamma(x); }

double norm_cdf(double z) { return 0.5 * erfc(-z / sqrt(2)); }
double norm_pdf(double z) { return exp(-z * z / 2) / sqrt(2 * M_PI); }

/* Acklam's inverse normal + one Newton step */
double norm_inv(double p) {
    if (p <= 0) return -INFINITY;
    if (p >= 1) return INFINITY;
    static const double a[] = {-3.969683028665376e+01, 2.209460984245205e+02, -2.759285104469687e+02, 1.383577518672690e+02, -3.066479806614716e+01, 2.506628277459239e+00};
    static const double b[] = {-5.447609879822406e+01, 1.615858368580409e+02, -1.556989798598866e+02, 6.680131188771972e+01, -1.328068155288572e+01};
    static const double c[] = {-7.784894002430293e-03, -3.223964580411365e-01, -2.400758277161838e+00, -2.549732539343734e+00, 4.374664141464968e+00, 2.938163982698783e+00};
    static const double d[] = {7.784695709041462e-03, 3.224671290700398e-01, 2.445134137142996e+00, 3.754408661907416e+00};
    double q, r, x;
    if (p < 0.02425) { q = sqrt(-2 * log(p)); x = (((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1); }
    else if (p > 1 - 0.02425) { q = sqrt(-2 * log(1 - p)); x = -(((((c[0] * q + c[1]) * q + c[2]) * q + c[3]) * q + c[4]) * q + c[5]) / ((((d[0] * q + d[1]) * q + d[2]) * q + d[3]) * q + 1); }
    else { q = p - 0.5; r = q * q; x = (((((a[0] * r + a[1]) * r + a[2]) * r + a[3]) * r + a[4]) * r + a[5]) * q / (((((b[0] * r + b[1]) * r + b[2]) * r + b[3]) * r + b[4]) * r + 1); }
    for (int i = 0; i < 2; i++) { double e = norm_cdf(x) - p; x -= e / norm_pdf(x); }
    return x;
}

/* regularized lower incomplete gamma P(a, x) */
double gammap(double a, double x) {
    if (x <= 0) return 0;
    if (x < a + 1) {
        double sum = 1 / a, del = sum, ap = a;
        for (int n = 0; n < 500; n++) { ap += 1; del *= x / ap; sum += del; if (fabs(del) < fabs(sum) * 1e-15) break; }
        return sum * exp(-x + a * log(x) - lgam(a));
    }
    double b = x + 1 - a, c = 1e300, d = 1 / b, h = d;
    for (int i = 1; i < 500; i++) {
        double an = -i * (i - a);
        b += 2;
        d = an * d + b; if (fabs(d) < 1e-300) d = 1e-300;
        c = b + an / c; if (fabs(c) < 1e-300) c = 1e-300;
        d = 1 / d;
        double del = d * c;
        h *= del;
        if (fabs(del - 1) < 1e-15) break;
    }
    return 1 - exp(-x + a * log(x) - lgam(a)) * h;
}

double chi2_cdf(double x, int k) { return gammap(k / 2.0, x / 2); }

static double bisect_inv(double (*cdf)(double, int), int k, double p, double lo, double hi) {
    for (int i = 0; i < 200; i++) { double m = (lo + hi) / 2; if (cdf(m, k) < p) lo = m; else hi = m; }
    return (lo + hi) / 2;
}
double chi2_inv(double p, int k) { return bisect_inv(chi2_cdf, k, p, 0, 1000); }

/* regularized incomplete beta via continued fraction */
static double betacf(double a, double b, double x) {
    double qab = a + b, qap = a + 1, qam = a - 1, c = 1, d = 1 - qab * x / qap;
    if (fabs(d) < 1e-300) d = 1e-300;
    d = 1 / d;
    double h = d;
    for (int m = 1; m <= 300; m++) {
        int m2 = 2 * m;
        double aa = m * (b - m) * x / ((qam + m2) * (a + m2));
        d = 1 + aa * d; if (fabs(d) < 1e-300) d = 1e-300;
        c = 1 + aa / c; if (fabs(c) < 1e-300) c = 1e-300;
        d = 1 / d; h *= d * c;
        aa = -(a + m) * (qab + m) * x / ((a + m2) * (qap + m2));
        d = 1 + aa * d; if (fabs(d) < 1e-300) d = 1e-300;
        c = 1 + aa / c; if (fabs(c) < 1e-300) c = 1e-300;
        d = 1 / d;
        double del = d * c;
        h *= del;
        if (fabs(del - 1) < 1e-15) break;
    }
    return h;
}
double betai(double a, double b, double x) {
    if (x <= 0) return 0;
    if (x >= 1) return 1;
    double bt = exp(lgam(a + b) - lgam(a) - lgam(b) + a * log(x) + b * log(1 - x));
    if (x < (a + 1) / (a + b + 2)) return bt * betacf(a, b, x) / a;
    return 1 - bt * betacf(b, a, 1 - x) / b;
}

double t_cdf(double t, int v) {
    double x = v / (v + t * t);
    double p = 0.5 * betai(v / 2.0, 0.5, x);
    return t > 0 ? 1 - p : p;
}
double t_inv(double p, int v) { return bisect_inv(t_cdf, v, p, -1000, 1000); }

double pois_pmf(int r, double l) { if (r < 0) return 0; return exp(-l + r * log(l) - lgam(r + 1)); }
double pois_cdf(int r, double l) { double s = 0; for (int i = 0; i <= r; i++) s += pois_pmf(i, l); return s > 1 ? 1 : s; }
double binom_pmf(int r, int n, double p) {
    if (r < 0 || r > n) return 0;
    if (p == 0) return r == 0;
    if (p == 1) return r == n;
    return exp(lgam(n + 1) - lgam(r + 1) - lgam(n - r + 1) + r * log(p) + (n - r) * log(1 - p));
}
double binom_cdf(int r, int n, double p) { double s = 0; for (int i = 0; i <= r && i <= n; i++) s += binom_pmf(i, n, p); return s > 1 ? 1 : s; }

/* PMCC critical value for one-tailed level alpha */
double pmcc_crit(int n, double alpha) {
    double t = t_inv(1 - alpha, n - 2);
    return t / sqrt(n - 2 + t * t);
}

/* exact Spearman: P(rs >= r) for n <= 9 by enumeration; normal approx otherwise */
static int perm[12], used[12], gn;
static long cnt_ge, cnt_tot;
static int target_d2;
static void rec(int i, int d2) {
    if (i == gn) { cnt_tot++; if (d2 <= target_d2) cnt_ge++; return; }
    for (int v = 0; v < gn; v++) if (!used[v]) { used[v] = 1; rec(i + 1, d2 + (v - i) * (v - i)); used[v] = 0; }
}
double spearman_p_upper(int n, double rs, int *exact) {
    /* P(R >= rs) under H0 */
    if (n <= 9) {
        gn = n; cnt_ge = cnt_tot = 0;
        double d2 = (1 - rs) * n * (n * (double)n - 1) / 6;
        target_d2 = (int)floor(d2 + 1e-6);
        memset(used, 0, sizeof used);
        rec(0, 0);
        *exact = 1;
        (void)perm;
        return (double)cnt_ge / cnt_tot;
    }
    *exact = 0;
    double t = rs * sqrt((n - 2) / (1 - rs * rs + 1e-300));
    return 1 - t_cdf(t, n - 2);
}

/* data list parsing: "1,2,3" into array (max) */
int parse_list(const char *s, double *v, int max) {
    char tmp[512];
    strncpy(tmp, s, 511);
    tmp[511] = 0;
    int n = 0;
    char *st = tmp;
    for (char *p = tmp;; p++) {
        if (*p == ',' || *p == 0) {
            char e = *p;
            *p = 0;
            const char *err;
            node *nd = ex_parse(st, &err);
            if (!nd) return -1;
            if (n >= max) return -2;
            v[n++] = ex_eval(nd).re;
            st = p + 1;
            if (!e) break;
        }
    }
    return n;
}
