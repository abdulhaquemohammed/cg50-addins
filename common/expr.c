/* expression parser, complex evaluation, power series, numeric calculus */
#include "core.h"
#include <string.h>
#include <stdlib.h>

enum { N_NUM, N_VAR, N_ADD, N_SUB, N_MUL, N_DIV, N_POW, N_NEG, N_FN, N_FACT };
enum { F_SIN, F_COS, F_TAN, F_ASIN, F_ACOS, F_ATAN, F_SINH, F_COSH, F_TANH, F_ASINH, F_ACOSH,
       F_ATANH, F_SEC, F_CSC, F_COT, F_LN, F_LOG, F_EXP, F_SQRT, F_ABS, F_ARG, F_CONJ, F_RE, F_IM,
       F_SECH, F_CSCH, F_COTH };
static const char *fnames[] = {"sin", "cos", "tan", "asin", "acos", "atan", "sinh", "cosh", "tanh",
                               "asinh", "acosh", "atanh", "sec", "cosec", "cot", "ln", "log", "exp",
                               "sqrt", "abs", "arg", "conj", "re", "im", "sech", "cosech", "coth"};

struct node { int t; int f; cx v; char var; node *a, *b; const char *src; };

#define POOL 1200
static node pool[POOL];
static int np;
static char srcs[4096];
static int sp;
static cx vars[128];

void ex_reset(void) { np = 0; sp = 0; }
void ex_setvar(char v, cx val) { vars[(unsigned char)v] = val; }

static node *mk(int t) {
    if (np >= POOL) return NULL;
    node *n = &pool[np++];
    memset(n, 0, sizeof *n);
    n->t = t;
    return n;
}

/* ---- tokenizer over a normalised string ---- */
static const char *ps;
static const char *perr;

static void skip(void) { while (*ps == ' ') ps++; }

static node *p_expr(void);
static node *p_postfix(void);
static node *p_postfix_noexp(void) { return p_postfix(); }
static int has_null(node *n);

static int starts_primary(void) {
    skip();
    unsigned char c = *ps;
    return (c >= '0' && c <= '9') || c == '.' || c == '(' || (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') || c == 0x03 || c == 0x04 || c == 0x05 || c == 0x1c;
}

static node *p_primary(void) {
    skip();
    if (perr) return NULL;
    unsigned char c = *ps;
    if ((c >= '0' && c <= '9') || c == '.') {
        double v = 0, sc = 0;
        int any = 0;
        while ((*ps >= '0' && *ps <= '9') || *ps == '.') {
            if (*ps == '.') { if (sc) { perr = "bad number"; return NULL; } sc = 1; }
            else { v = v * 10 + (*ps - '0'); if (sc) sc *= 10; any = 1; }
            ps++;
        }
        if (!any) { perr = "bad number"; return NULL; }
        if (sc) v /= sc;
        if (*ps == 'E' && (ps[1] == '-' || (ps[1] >= '0' && ps[1] <= '9'))) {
            ps++;
            int s = 1, e = 0;
            if (*ps == '-') { s = -1; ps++; }
            while (*ps >= '0' && *ps <= '9') e = e * 10 + (*ps++ - '0');
            v *= pow(10, s * e);
        }
        node *n = mk(N_NUM);
        if (!n) { perr = "too long"; return NULL; }
        n->v = R(v);
        return n;
    }
    if (c == '(') {
        ps++;
        node *n = p_expr();
        skip();
        if (*ps == ')') ps++;  /* allow missing close at end */
        else if (*ps) { perr = "missing )"; return NULL; }
        return n;
    }
    if (c == 0x04) { ps++; node *n = mk(N_NUM); n->v = R(M_PI); return n; }
    if (c == 0x05) { ps++; node *n = mk(N_VAR); n->var = 'T'; return n; }
    if (c == 0x03) {
        ps++;
        node *n = mk(N_FN);
        n->f = F_SQRT;
        skip();
        n->a = p_postfix_noexp();
        return n;
    }
    if (c == 0x1c) { ps++; node *n = mk(N_NUM); n->v = R(INFINITY); return n; }
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        /* longest function name match */
        int best = -1, bl = 0;
        for (int i = 0; i < NEL(fnames); i++) {
            int L = strlen(fnames[i]);
            if (L > bl && strncmp(ps, fnames[i], L) == 0) { best = i; bl = L; }
        }
        /* aliases */
        if (strncmp(ps, "arsinh", 6) == 0) { best = F_ASINH; bl = 6; }
        else if (strncmp(ps, "arcosh", 6) == 0) { best = F_ACOSH; bl = 6; }
        else if (strncmp(ps, "artanh", 6) == 0) { best = F_ATANH; bl = 6; }
        else if (strncmp(ps, "arcsin", 6) == 0) { best = F_ASIN; bl = 6; }
        else if (strncmp(ps, "arccos", 6) == 0) { best = F_ACOS; bl = 6; }
        else if (strncmp(ps, "arctan", 6) == 0) { best = F_ATAN; bl = 6; }
        else if (strncmp(ps, "csc", 3) == 0 && bl < 3) { best = F_CSC; bl = 3; }
        if (best >= 0) {
            ps += bl;
            node *n = mk(N_FN);
            if (!n) { perr = "too long"; return NULL; }
            n->f = best;
            skip();
            /* sin^2(x) style power */
            node *pw = NULL;
            if (*ps == '^') {
                ps++;
                pw = p_primary();
            }
            skip();
            n->a = p_primary();
            if (!n->a) { if (!perr) perr = "missing argument"; return NULL; }
            if (pw) { node *p = mk(N_POW); p->a = n; p->b = pw; return p; }
            return n;
        }
        if (strncmp(ps, "pi", 2) == 0) { ps += 2; node *n = mk(N_NUM); n->v = R(M_PI); return n; }
        if (strncmp(ps, "inf", 3) == 0) { ps += 3; node *n = mk(N_NUM); n->v = R(INFINITY); return n; }
        if (strncmp(ps, "th", 2) == 0) { ps += 2; node *n = mk(N_VAR); n->var = 'T'; return n; }
        if (c == 'e') { ps++; node *n = mk(N_NUM); n->v = R(M_E); return n; }
        if (c == 'i') { ps++; node *n = mk(N_NUM); n->v = C(0, 1); return n; }
        ps++;
        node *n = mk(N_VAR);
        if (!n) { perr = "too long"; return NULL; }
        n->var = c;
        return n;
    }
    perr = "syntax error";
    return NULL;
}

static node *p_postfix(void) {
    node *a = p_primary();
    skip();
    while (a && *ps == '!') { ps++; node *n = mk(N_FACT); n->a = a; a = n; skip(); }
    /* superscript ² ³ */
    while (a && (ps[0] == 0x15 || ps[0] == 0x16)) {
        node *n = mk(N_POW);
        n->a = a;
        n->b = mk(N_NUM);
        n->b->v = R(ps[0] == 0x15 ? 2 : 3);
        ps += 1;
        a = n;
        skip();
    }
    return a;
}

static node *p_unary(void);
static node *p_pow(void) {
    node *a = p_postfix();
    skip();
    if (a && *ps == '^') {
        ps++;
        node *n = mk(N_POW);
        n->a = a;
        n->b = p_unary();  /* right assoc, allows 2^-x */
        return n;
    }
    return a;
}

static node *p_unary(void) {
    skip();
    if (*ps == '-') { ps++; node *n = mk(N_NEG); n->a = p_unary(); return n; }
    if (*ps == '+') { ps++; return p_unary(); }
    return p_pow();
}

static node *p_term(void) {
    node *a = p_unary();
    for (;;) {
        skip();
        if (perr || !a) return a;
        if (*ps == '*' || *ps == '/') {
            int t = *ps == '*' ? N_MUL : N_DIV;
            ps++;
            node *n = mk(t);
            n->a = a;
            n->b = p_unary();
            a = n;
        } else if (ps[0] == 0x17) {
            ps += 1;
            node *n = mk(N_MUL); n->a = a; n->b = p_unary(); a = n;
        } else if (starts_primary()) {
            node *n = mk(N_MUL);
            n->a = a;
            n->b = p_pow();
            a = n;
        } else return a;
    }
}

static node *p_expr(void) {
    node *a = p_term();
    for (;;) {
        skip();
        if (perr || !a) return a;
        if (*ps == '+' || *ps == '-') {
            int t = *ps == '+' ? N_ADD : N_SUB;
            ps++;
            node *n = mk(t);
            n->a = a;
            n->b = p_term();
            a = n;
        } else return a;
    }
}

node *ex_parse(const char *s, const char **err) {
    perr = NULL;
    ps = s;
    skip();
    if (!*ps) { *err = "empty"; return NULL; }
    node *n = p_expr();
    skip();
    if (!perr && *ps) perr = (*ps == ')') ? "extra )" : "syntax error";
    if (!perr && (!n || has_null(n))) perr = "syntax error";
    /* check nodes for NULL children */
    if (!perr) {
        int L = strlen(s);
        if (sp + L + 1 < (int)sizeof srcs) {
            memcpy(srcs + sp, s, L + 1);
            n->src = srcs + sp;
            sp += L + 1;
        }
    }
    *err = perr;
    return perr ? NULL : n;
}

const char *ex_src(node *n) { return n && n->src ? n->src : ""; }

static int has_null(node *n) {
    if (!n) return 1;
    switch (n->t) {
    case N_NUM: case N_VAR: return 0;
    case N_NEG: case N_FN: case N_FACT: return has_null(n->a);
    default: return has_null(n->a) || has_null(n->b);
    }
}

static cx cfn(int f, cx z) {
    int real = z.im == 0;
    double x = z.re;
    switch (f) {
    case F_SIN: if (real) return R(sin(x)); return C(sin(x) * cosh(z.im), cos(x) * sinh(z.im));
    case F_COS: if (real) return R(cos(x)); return C(cos(x) * cosh(z.im), -sin(x) * sinh(z.im));
    case F_TAN: if (real) return R(tan(x)); return cdiv(cfn(F_SIN, z), cfn(F_COS, z));
    case F_SEC: return cdiv(R(1), cfn(F_COS, z));
    case F_CSC: return cdiv(R(1), cfn(F_SIN, z));
    case F_COT: return cdiv(cfn(F_COS, z), cfn(F_SIN, z));
    case F_ASIN: if (real && fabs(x) <= 1) return R(asin(x)); return R(NAN);
    case F_ACOS: if (real && fabs(x) <= 1) return R(acos(x)); return R(NAN);
    case F_ATAN: if (real) return R(atan(x)); return R(NAN);
    case F_SINH: if (real) return R(sinh(x)); return cmul(R(0.5), csub(cexp_(z), cexp_(cneg(z))));
    case F_COSH: if (real) return R(cosh(x)); return cmul(R(0.5), cadd(cexp_(z), cexp_(cneg(z))));
    case F_TANH: if (real) return R(tanh(x)); return cdiv(cfn(F_SINH, z), cfn(F_COSH, z));
    case F_SECH: return cdiv(R(1), cfn(F_COSH, z));
    case F_CSCH: return cdiv(R(1), cfn(F_SINH, z));
    case F_COTH: return cdiv(cfn(F_COSH, z), cfn(F_SINH, z));
    case F_ASINH: if (real) return R(log(x + sqrt(x * x + 1))); return R(NAN);
    case F_ACOSH: if (real && x >= 1) return R(log(x + sqrt(x * x - 1))); return R(NAN);
    case F_ATANH: if (real && fabs(x) < 1) return R(0.5 * log((1 + x) / (1 - x))); return R(NAN);
    case F_LN: if (real && x > 0) return R(log(x)); if (real && x == 0) return R(-INFINITY); return clog_(z);
    case F_LOG: if (real && x > 0) return R(log10(x)); return cdiv(clog_(z), R(log(10)));
    case F_EXP: return real ? R(exp(x)) : cexp_(z);
    case F_SQRT: return csqrt_(z);
    case F_ABS: return R(cabs_(z));
    case F_ARG: return R(carg_(z));
    case F_CONJ: return cconj(z);
    case F_RE: return R(z.re);
    case F_IM: return R(z.im);
    }
    return R(NAN);
}

cx ex_eval(node *n) {
    if (!n) return R(NAN);
    switch (n->t) {
    case N_NUM: return n->v;
    case N_VAR: return vars[(unsigned char)n->var];
    case N_ADD: return cadd(ex_eval(n->a), ex_eval(n->b));
    case N_SUB: return csub(ex_eval(n->a), ex_eval(n->b));
    case N_MUL: return cmul(ex_eval(n->a), ex_eval(n->b));
    case N_DIV: {
        cx a = ex_eval(n->a), b = ex_eval(n->b);
        if (b.re == 0 && b.im == 0) return R(a.re == 0 && a.im == 0 ? NAN : (a.re >= 0 ? INFINITY : -INFINITY));
        return cdiv(a, b);
    }
    case N_POW: return cpow_(ex_eval(n->a), ex_eval(n->b));
    case N_NEG: return cneg(ex_eval(n->a));
    case N_FN: return cfn(n->f, ex_eval(n->a));
    case N_FACT: { cx a = ex_eval(n->a); return R(tgamma(a.re + 1)); }
    }
    return R(NAN);
}

double ex_evalr(node *n, char var, double x) {
    ex_setvar(var, R(x));
    return ex_eval(n).re;
}

/* ================= power series ================= */
#define N SER_N
static void s_const(ser *s, double v) { memset(s, 0, sizeof *s); s->c[0] = v; }
static void s_mul(const ser *a, const ser *b, ser *o) {
    ser t;
    for (int k = 0; k <= N; k++) { double s = 0; for (int j = 0; j <= k; j++) s += a->c[j] * b->c[k - j]; t.c[k] = s; }
    *o = t;
}
static int s_div(const ser *a, const ser *b, ser *o) {
    if (fabs(b->c[0]) < 1e-14) return 0;
    ser t;
    for (int k = 0; k <= N; k++) {
        double s = a->c[k];
        for (int j = 1; j <= k; j++) s -= b->c[j] * t.c[k - j];
        t.c[k] = s / b->c[0];
    }
    *o = t;
    return 1;
}
static void s_exp(const ser *f, ser *g) {
    ser t;
    t.c[0] = exp(f->c[0]);
    for (int k = 1; k <= N; k++) { double s = 0; for (int j = 1; j <= k; j++) s += j * f->c[j] * t.c[k - j]; t.c[k] = s / k; }
    *g = t;
}
static int s_log(const ser *f, ser *g) {
    if (f->c[0] <= 0) return 0;
    ser t;
    t.c[0] = log(f->c[0]);
    for (int k = 1; k <= N; k++) {
        double s = 0;
        for (int j = 1; j < k; j++) s += j * t.c[j] * f->c[k - j];
        t.c[k] = (f->c[k] - s / k) / f->c[0];
    }
    *g = t;
    return 1;
}
static int s_powc(const ser *f, double a, ser *g) {
    double f0 = f->c[0];
    if (fabs(f0) < 1e-14) {
        /* allow integer powers of series with zero constant */
        if (a == floor(a) && a >= 0 && a <= N) {
            ser r; s_const(&r, 1);
            for (int i = 0; i < (int)a; i++) s_mul(&r, f, &r);
            *g = r; return 1;
        }
        return 0;
    }
    if (f0 < 0 && a != floor(a)) return 0;
    ser t;
    t.c[0] = pow(f0, a);
    for (int k = 1; k <= N; k++) {
        double s = 0;
        for (int j = 1; j <= k; j++) s += ((a + 1) * j - k) * f->c[j] * t.c[k - j];
        t.c[k] = s / (k * f0);
    }
    *g = t;
    return 1;
}
static void s_sincos(const ser *f, ser *s, ser *c, int hyp) {
    ser S, Cc;
    S.c[0] = hyp ? sinh(f->c[0]) : sin(f->c[0]);
    Cc.c[0] = hyp ? cosh(f->c[0]) : cos(f->c[0]);
    for (int k = 1; k <= N; k++) {
        double a = 0, b = 0;
        for (int j = 1; j <= k; j++) { a += j * f->c[j] * Cc.c[k - j]; b += j * f->c[j] * S.c[k - j]; }
        S.c[k] = a / k;
        Cc.c[k] = (hyp ? b : -b) / k;
    }
    if (s) *s = S;
    if (c) *c = Cc;
}
/* g = g0 + ∫ f' h */
static void s_intprod(const ser *f, const ser *h, double g0, ser *g) {
    ser d, p;
    memset(&d, 0, sizeof d);
    for (int k = 0; k < N; k++) d.c[k] = (k + 1) * f->c[k + 1];
    s_mul(&d, h, &p);
    g->c[0] = g0;
    for (int k = 1; k <= N; k++) g->c[k] = p.c[k - 1] / k;
}

static char svar;
static double sa;
static const char *serr;

static int ser_eval(node *n, ser *o) {
    ser a, b, t;
    switch (n->t) {
    case N_NUM:
        if (n->v.im != 0 || isinf(n->v.re)) { serr = "real only"; return 0; }
        s_const(o, n->v.re); return 1;
    case N_VAR:
        if (n->var == svar) { s_const(o, sa); o->c[1] = 1; return 1; }
        serr = "unknown variable"; return 0;
    case N_ADD: case N_SUB:
        if (!ser_eval(n->a, &a) || !ser_eval(n->b, &b)) return 0;
        for (int k = 0; k <= N; k++) o->c[k] = n->t == N_ADD ? a.c[k] + b.c[k] : a.c[k] - b.c[k];
        return 1;
    case N_NEG:
        if (!ser_eval(n->a, &a)) return 0;
        for (int k = 0; k <= N; k++) o->c[k] = -a.c[k];
        return 1;
    case N_MUL:
        if (!ser_eval(n->a, &a) || !ser_eval(n->b, &b)) return 0;
        s_mul(&a, &b, o); return 1;
    case N_DIV:
        if (!ser_eval(n->a, &a) || !ser_eval(n->b, &b)) return 0;
        if (!s_div(&a, &b, o)) { serr = "division by 0 at point"; return 0; }
        return 1;
    case N_POW: {
        if (!ser_eval(n->a, &a) || !ser_eval(n->b, &b)) return 0;
        int bconst = 1;
        for (int k = 1; k <= N; k++) if (fabs(b.c[k]) > 1e-15) bconst = 0;
        if (bconst) { if (!s_powc(&a, b.c[0], o)) { serr = "power undefined at point"; return 0; } return 1; }
        if (!s_log(&a, &t)) { serr = "undefined at point"; return 0; }
        s_mul(&t, &b, &t);
        s_exp(&t, o);
        return 1;
    }
    case N_FACT: serr = "! not allowed"; return 0;
    case N_FN: {
        if (!ser_eval(n->a, &a)) return 0;
        ser s, c, h;
        switch (n->f) {
        case F_SIN: s_sincos(&a, o, NULL, 0); return 1;
        case F_COS: s_sincos(&a, NULL, o, 0); return 1;
        case F_TAN: s_sincos(&a, &s, &c, 0); if (!s_div(&s, &c, o)) { serr = "tan undefined"; return 0; } return 1;
        case F_SEC: s_sincos(&a, &s, &c, 0); s_const(&t, 1); if (!s_div(&t, &c, o)) { serr = "sec undefined"; return 0; } return 1;
        case F_CSC: s_sincos(&a, &s, &c, 0); s_const(&t, 1); if (!s_div(&t, &s, o)) { serr = "cosec undefined"; return 0; } return 1;
        case F_COT: s_sincos(&a, &s, &c, 0); if (!s_div(&c, &s, o)) { serr = "cot undefined"; return 0; } return 1;
        case F_SINH: s_sincos(&a, o, NULL, 1); return 1;
        case F_COSH: s_sincos(&a, NULL, o, 1); return 1;
        case F_TANH: s_sincos(&a, &s, &c, 1); s_div(&s, &c, o); return 1;
        case F_SECH: s_sincos(&a, &s, &c, 1); s_const(&t, 1); s_div(&t, &c, o); return 1;
        case F_CSCH: s_sincos(&a, &s, &c, 1); s_const(&t, 1); if (!s_div(&t, &s, o)) { serr = "cosech undefined"; return 0; } return 1;
        case F_COTH: s_sincos(&a, &s, &c, 1); if (!s_div(&c, &s, o)) { serr = "coth undefined"; return 0; } return 1;
        case F_EXP: s_exp(&a, o); return 1;
        case F_LN: if (!s_log(&a, o)) { serr = "ln undefined at point"; return 0; } return 1;
        case F_LOG: if (!s_log(&a, o)) { serr = "log undefined"; return 0; } for (int k = 0; k <= N; k++) o->c[k] /= log(10); return 1;
        case F_SQRT: if (!s_powc(&a, 0.5, o)) { serr = "sqrt undefined at point"; return 0; } return 1;
        case F_ATAN: case F_ATANH: case F_ASIN: case F_ACOS: case F_ASINH: case F_ACOSH: {
            ser sq;
            s_mul(&a, &a, &sq);
            int sg = (n->f == F_ATAN || n->f == F_ASINH) ? 1 : -1;
            if (n->f == F_ACOSH) { for (int k = 0; k <= N; k++) sq.c[k] = sq.c[k]; sq.c[0] -= 1; }
            else { for (int k = 0; k <= N; k++) sq.c[k] *= sg; sq.c[0] += 1; }
            double a0 = a.c[0];
            double g0;
            if (n->f == F_ATAN) g0 = atan(a0);
            else if (n->f == F_ATANH) { if (fabs(a0) >= 1) { serr = "artanh undefined"; return 0; } g0 = atanh(a0); }
            else if (n->f == F_ASIN) { if (fabs(a0) >= 1) { serr = "arcsin: |x|<1 needed"; return 0; } g0 = asin(a0); }
            else if (n->f == F_ACOS) { if (fabs(a0) >= 1) { serr = "arccos: |x|<1 needed"; return 0; } g0 = acos(a0); }
            else if (n->f == F_ASINH) g0 = asinh(a0);
            else { if (a0 <= 1) { serr = "arcosh: x>1 needed"; return 0; } g0 = acosh(a0); }
            if (n->f == F_ATAN || n->f == F_ATANH) { ser one; s_const(&one, 1); s_div(&one, &sq, &h); }
            else { if (!s_powc(&sq, -0.5, &h)) { serr = "undefined"; return 0; } }
            if (n->f == F_ACOS) for (int k = 0; k <= N; k++) h.c[k] = -h.c[k];
            s_intprod(&a, &h, g0, o);
            return 1;
        }
        default: serr = "function not supported in series"; return 0;
        }
    }
    }
    serr = "error";
    return 0;
}

int ex_series(node *n, char var, double a, ser *out, const char **err) {
    svar = var; sa = a; serr = NULL;
    if (!n || has_null(n)) { *err = "syntax error"; return 0; }
    int ok = ser_eval(n, out);
    *err = serr;
    return ok;
}
#undef N

/* ================= numeric calculus ================= */
static node *gf; static char gv;
static double F(double x) { return ex_evalr(gf, gv, x); }

/* adaptive Gauss-Kronrod-ish: adaptive Simpson with tanh-sinh fallback for infinite */
static double simpson(double a, double b, double fa, double fm, double fb, double whole, double eps, int depth, int *bad) {
    double m = (a + b) / 2, lm = (a + m) / 2, rm = (m + b) / 2;
    double flm = F(lm), frm = F(rm);
    double left = (b - a) / 12 * (fa + 4 * flm + fm), right = (b - a) / 12 * (fm + 4 * frm + fb);
    double d = left + right - whole;
    if (depth <= 0) { if (fabs(d) > 1e-6 * (1 + fabs(whole))) *bad = 1; return left + right + d / 15; }
    if (fabs(d) <= 15 * eps) return left + right + d / 15;
    return simpson(a, m, fa, flm, fm, left, eps / 2, depth - 1, bad) + simpson(m, b, fm, frm, fb, right, eps / 2, depth - 1, bad);
}

/* tanh-sinh quadrature on [a,b] handles endpoint singularities */
static double ts(double a, double b, int *bad) {
    double c = (a + b) / 2, h2 = (b - a) / 2;
    double sum = 0, prev = 0;
    double h = 1.0 / 8;
    for (int level = 0; level < 8; level++) {
        sum = 0;
        for (double t = -4; t <= 4.0001; t += h) {
            double u = M_PI / 2 * sinh(t);
            double x = tanh(u);
            double w = M_PI / 2 * cosh(t) / (cosh(u) * cosh(u));
            double xx = c + h2 * x;
            if (xx <= a || xx >= b) continue;
            double fx_ = F(xx);
            if (fx_ != fx_ || isinf(fx_)) { *bad = 1; continue; }
            sum += w * fx_;
        }
        sum *= h * h2;
        if (level > 2 && fabs(sum - prev) < 1e-11 * (1 + fabs(sum))) return sum;
        prev = sum;
        h /= 2;
    }
    if (fabs(sum - prev) > 1e-6 * (1 + fabs(sum))) *bad = 1;
    return sum;
}

double integrate(node *f, char var, double a, double b, int *ok) {
    gf = f; gv = var;
    int bad = 0;
    double sgn = 1;
    if (a > b) { double t = a; a = b; b = t; sgn = -1; }
    double r;
    if (isinf(a) || isinf(b)) {
        /* substitution x = t/(1-t^2) maps (-1,1) -> R; handle half-infinite with x = a + t/(1-t) */
        double s = 0;
        int N = 4000;
        if (!isinf(a)) {
            /* [a, inf): x = a + t/(1-t), t in [0,1) */
            double tsum = 0;
            /* tanh-sinh on t */
            double h = 1.0 / 16, prev = 0;
            for (int level = 0; level < 7; level++) {
                tsum = 0;
                for (double t = -4; t <= 4.0001; t += h) {
                    double u = M_PI / 2 * sinh(t);
                    double tt = (tanh(u) + 1) / 2;
                    double w = M_PI / 4 * cosh(t) / (cosh(u) * cosh(u));
                    if (tt <= 0 || tt >= 1) continue;
                    double x = a + tt / (1 - tt);
                    double fx_ = F(x) / ((1 - tt) * (1 - tt));
                    if (fx_ != fx_ || isinf(fx_)) continue;
                    tsum += w * fx_;
                }
                tsum *= h;
                if (level > 2 && fabs(tsum - prev) < 1e-10 * (1 + fabs(tsum))) break;
                prev = tsum;
                h /= 2;
            }
            if (fabs(tsum - prev) > 1e-5 * (1 + fabs(tsum))) bad = 1;
            s = tsum;
        } else if (!isinf(b)) {
            /* (-inf, b]: x = b - t/(1-t) */
            double tsum = 0, h = 1.0 / 16, prev = 0;
            for (int level = 0; level < 7; level++) {
                tsum = 0;
                for (double t = -4; t <= 4.0001; t += h) {
                    double u = M_PI / 2 * sinh(t);
                    double tt = (tanh(u) + 1) / 2;
                    double w = M_PI / 4 * cosh(t) / (cosh(u) * cosh(u));
                    if (tt <= 0 || tt >= 1) continue;
                    double x = b - tt / (1 - tt);
                    double fx_ = F(x) / ((1 - tt) * (1 - tt));
                    if (fx_ != fx_ || isinf(fx_)) continue;
                    tsum += w * fx_;
                }
                tsum *= h;
                if (level > 2 && fabs(tsum - prev) < 1e-10 * (1 + fabs(tsum))) break;
                prev = tsum;
                h /= 2;
            }
            if (fabs(tsum - prev) > 1e-5 * (1 + fabs(tsum))) bad = 1;
            s = tsum;
        } else {
            int ok1, ok2;
            s = integrate(f, var, -INFINITY, 0, &ok1) + integrate(f, var, 0, INFINITY, &ok2);
            if (!ok1 || !ok2) bad = 1;
            gf = f; gv = var;
        }
        (void)N;
        r = s;
    } else {
        double fa = F(a), fb = F(b);
        if (fa != fa || fb != fb || isinf(fa) || isinf(fb)) r = ts(a, b, &bad);
        else {
            int b1 = 0;
            r = 0;
            int NP = 24;
            double h = (b - a) / NP, xa = a, fxa = fa;
            for (int i = 0; i < NP; i++) {
                double xb = (i == NP - 1) ? b : a + (i + 1) * h;
                double fxb = (i == NP - 1) ? fb : F(xb);
                double fm = F((xa + xb) / 2);
                if (fxb != fxb || fm != fm || isinf(fxb) || isinf(fm)) { b1 = 1; break; }
                r += simpson(xa, xb, fxa, fm, fxb, (xb - xa) / 6 * (fxa + 4 * fm + fxb), 1e-12 / NP + 1e-13, 30, &b1);
                xa = xb; fxa = fxb;
            }
            if (b1 || r != r) { bad = 0; r = ts(a, b, &bad); }
        }
    }
    *ok = !bad && r == r && !isinf(r) && fabs(r) < 1e12;
    return sgn * r;
}

double nderiv(node *f, char var, double x) {
    gf = f; gv = var;
    double h = 1e-4 * (1 + fabs(x));
    return (-F(x + 2 * h) + 8 * F(x + h) - 8 * F(x - h) + F(x - 2 * h)) / (12 * h);
}

/* all roots in [lo,hi] of a C function by scanning + bisection */
static double (*gfn)(double);
static double FN(double x) { return gfn(x); }
int nsolve_fn(double (*f)(double), double lo, double hi, double *roots, int maxr) {
    gfn = f;
    int n = 0;
    int steps = 1500;
    double dx = (hi - lo) / steps;
    double xp = lo, fp = FN(lo);
    for (int i = 1; i <= steps && n < maxr; i++) {
        double x = lo + i * dx, fx_ = FN(x);
        if (fp == 0) {
            if (n == 0 || fabs(roots[n - 1] - xp) > dx / 2) roots[n++] = xp;
        } else if (fp == fp && fx_ == fx_ && !isinf(fp) && !isinf(fx_) && ((fp < 0) != (fx_ < 0)) && fx_ != 0) {
            double a = xp, b = x, fa = fp;
            for (int k = 0; k < 100; k++) {
                double m = (a + b) / 2, fm = FN(m);
                if ((fa < 0) == (fm < 0)) { a = m; fa = fm; } else b = m;
            }
            double r = (a + b) / 2;
            double fr = FN(r);
            if (fabs(fr) < 1e-6 * (1 + fabs(fp) + fabs(fx_))) roots[n++] = r;
        } else if (i >= 2 && fp == fp && fx_ == fx_) {
            double xm = xp - dx, fm = FN(xm);
            if (fm == fm && fabs(fp) < fabs(fm) && fabs(fp) < fabs(fx_) && (fm < 0) == (fp < 0) && (fx_ < 0) == (fp < 0)) {
                double a = xm, b = x;
                for (int k = 0; k < 80; k++) {
                    double m1 = a + (b - a) * 0.382, m2 = a + (b - a) * 0.618;
                    if (fabs(FN(m1)) < fabs(FN(m2))) b = m2; else a = m1;
                }
                double r = (a + b) / 2;
                if (fabs(FN(r)) < 1e-9 && (n == 0 || fabs(roots[n - 1] - r) > dx)) roots[n++] = r;
            }
        }
        if (n && fx_ == 0 && fabs(roots[n - 1] - x) > dx / 2 && i == steps) roots[n++] = x;
        xp = x; fp = fx_;
    }
    return n;
}

int nsolve(node *f, char var, double lo, double hi, double *roots, int maxr) {
    gf = f; gv = var;
    return nsolve_fn(F, lo, hi, roots, maxr);
}

/* ================= symbolic differentiation & printing ================= */
static node *num(double v) { node *n = mk(N_NUM); if (n) n->v = R(v); return n; }
static node *bin(int t, node *a, node *b) { node *n = mk(t); if (n) { n->a = a; n->b = b; } return n; }
static node *fn1(int f, node *a) { node *n = mk(N_FN); if (n) { n->f = f; n->a = a; } return n; }
static int isnum(node *n, double v) { return n && n->t == N_NUM && n->v.im == 0 && fabs(n->v.re - v) < 1e-15; }
static int isconst(node *n) {
    if (!n) return 1;
    switch (n->t) {
    case N_NUM: return 1;
    case N_VAR: return 0;
    case N_NEG: case N_FN: case N_FACT: return isconst(n->a);
    default: return isconst(n->a) && isconst(n->b);
    }
}
static int depends(node *n, char v) {
    if (!n) return 0;
    switch (n->t) {
    case N_NUM: return 0;
    case N_VAR: return n->var == v;
    case N_NEG: case N_FN: case N_FACT: return depends(n->a, v);
    default: return depends(n->a, v) || depends(n->b, v);
    }
}

static int eqn(node *a, node *b) {
    if (!a || !b) return a == b;
    if (a->t != b->t) return 0;
    switch (a->t) {
    case N_NUM: return a->v.re == b->v.re && a->v.im == b->v.im;
    case N_VAR: return a->var == b->var;
    case N_FN: return a->f == b->f && eqn(a->a, b->a);
    case N_NEG: case N_FACT: return eqn(a->a, b->a);
    default: return eqn(a->a, b->a) && eqn(a->b, b->b);
    }
}
static node *simp(node *n);
static node *S(node *n) { return simp(n); }

static node *simp(node *n) {
    if (!n) return n;
    if (n->t == N_NUM || n->t == N_VAR) return n;
    if (n->a) n->a = simp(n->a);
    if (n->b) n->b = simp(n->b);
    node *a = n->a, *b = n->b;
    /* constant folding for pure numbers */
    if (n->t != N_FN && n->t != N_FACT && a && a->t == N_NUM && (!b || b->t == N_NUM) && n->t != N_POW) {
        cx v = ex_eval(n);
        if (v.im == 0 && v.re == v.re && !isinf(v.re)) return num(v.re);
    }
    if (n->t == N_POW && a->t == N_NUM && b->t == N_NUM && b->v.re == floor(b->v.re) && fabs(b->v.re) < 10 && a->v.re != M_E) {
        cx v = ex_eval(n);
        if (v.im == 0) return num(v.re);
    }
    switch (n->t) {
    case N_ADD:
        if (isnum(a, 0)) return b;
        if (isnum(b, 0)) return a;
        if (b->t == N_NEG) return S(bin(N_SUB, a, b->a));
        if (b->t == N_NUM && b->v.re < 0) return bin(N_SUB, a, num(-b->v.re));
        if (b->t == N_MUL && b->a->t == N_NUM && b->a->v.re < 0) return S(bin(N_SUB, a, bin(N_MUL, num(-b->a->v.re), b->b)));
        break;
    case N_SUB:
        if (isnum(b, 0)) return a;
        if (isnum(a, 0)) return S(mk(N_NEG) ? (n->t = N_NEG, n->a = b, n->b = NULL, n) : n);
        if (b->t == N_NEG) return S(bin(N_ADD, a, b->a));
        if (b->t == N_NUM && b->v.re < 0) return bin(N_ADD, a, num(-b->v.re));
        if (b->t == N_MUL && b->a->t == N_NUM && b->a->v.re < 0) return S(bin(N_ADD, a, bin(N_MUL, num(-b->a->v.re), b->b)));
        break;
    case N_MUL:
        if (isnum(a, 0) || isnum(b, 0)) return num(0);
        if (isnum(a, 1)) return b;
        if (isnum(b, 1)) return a;
        if (isnum(a, -1)) { node *m = mk(N_NEG); m->a = b; return S(m); }
        if (b->t == N_NUM && a->t != N_NUM) return S(bin(N_MUL, b, a));
        if (a->t == N_NUM && b->t == N_MUL && b->a->t == N_NUM) return S(bin(N_MUL, num(a->v.re * b->a->v.re), b->b));
        if (a->t == N_NEG) { node *m = mk(N_NEG); m->a = bin(N_MUL, a->a, b); return S(m); }
        if (b->t == N_NEG) { node *m = mk(N_NEG); m->a = bin(N_MUL, a, b->a); return S(m); }
        if (a->t == N_NUM && a->v.re < 0) { node *m = mk(N_NEG); m->a = bin(N_MUL, num(-a->v.re), b); return S(m); }
        /* move numeric factor in b to front: x*(3y) -> 3(xy) */
        if (a->t != N_NUM && b->t == N_MUL && b->a->t == N_NUM) return S(bin(N_MUL, b->a, bin(N_MUL, a, b->b)));
        if (a->t == N_DIV && isnum(a->a, 1)) return S(bin(N_DIV, b, a->b));
        if ((a->t == N_FN || (a->t == N_POW && a->a->t == N_NUM)) && (b->t == N_VAR || (b->t == N_POW && b->a->t == N_VAR && b->b->t == N_NUM))) return bin(N_MUL, b, a);
        if (a->t == N_FN && b->t == N_POW && b->a->t == N_NUM) return bin(N_MUL, b, a);
        if (a->t == N_FN && b->t == N_POW && b->a->t == N_NUM && fabs(b->a->v.re - M_E) < 1e-15) return bin(N_MUL, b, a);
        if (b->t == N_DIV && isnum(b->a, 1)) return S(bin(N_DIV, a, b->b));
        break;
    case N_DIV:
        if (isnum(a, 0)) return num(0);
        if (isnum(b, 1)) return a;
        if (eqn(a, b)) return num(1);
        if (a->t == N_MUL && a->a->t == N_NUM && b->t == N_MUL && b->a->t == N_NUM && b->a->v.re != 0)
            return S(bin(N_MUL, num(a->a->v.re / b->a->v.re), bin(N_DIV, a->b, b->b)));
        if (a->t == N_POW && a->b->t == N_NUM && eqn(a->a, b)) return S(bin(N_POW, b, num(a->b->v.re - 1)));
        if (a->t == N_POW && b->t == N_POW && a->b->t == N_NUM && b->b->t == N_NUM && eqn(a->a, b->a))
            return S(bin(N_POW, a->a, num(a->b->v.re - b->b->v.re)));
        if (a->t == N_MUL && a->b->t == N_POW && a->b->b->t == N_NUM && eqn(a->b->a, b))
            return S(bin(N_MUL, a->a, bin(N_POW, b, num(a->b->b->v.re - 1))));
        if (b->t == N_NUM && a->t == N_MUL && a->a->t == N_NUM) return S(bin(N_MUL, num(a->a->v.re / b->v.re), a->b));
        if (a->t == N_NEG) { node *m = mk(N_NEG); m->a = bin(N_DIV, a->a, b); return S(m); }
        break;
    case N_POW:
        if (isnum(b, 0)) return num(1);
        if (isnum(b, 1)) return a;
        if (a->t == N_POW && a->b->t == N_NUM && b->t == N_NUM) return S(bin(N_POW, a->a, num(a->b->v.re * b->v.re)));
        break;
    case N_NEG:
        if (a->t == N_NEG) return a->a;
        if (a->t == N_NUM) return num(-a->v.re);
        break;
    case N_FN:
        if (n->f == F_LN && a->t == N_NUM && fabs(a->v.re - M_E) < 1e-15) return num(1);
        if (n->f == F_LN && isnum(a, 1)) return num(0);
        break;
    }
    return n;
}

static node *D(node *n, char v) {
    if (!n) return NULL;
    if (!depends(n, v)) return num(0);
    node *a = n->a, *b = n->b;
    switch (n->t) {
    case N_VAR: return num(1);
    case N_ADD: return bin(N_ADD, D(a, v), D(b, v));
    case N_SUB: return bin(N_SUB, D(a, v), D(b, v));
    case N_NEG: { node *m = mk(N_NEG); m->a = D(a, v); return m; }
    case N_MUL:
        if (!depends(a, v)) return bin(N_MUL, a, D(b, v));
        if (!depends(b, v)) return bin(N_MUL, b, D(a, v));
        return bin(N_ADD, bin(N_MUL, D(a, v), b), bin(N_MUL, a, D(b, v)));
    case N_DIV:
        if (!depends(b, v)) return bin(N_DIV, D(a, v), b);
        if (!depends(a, v) && b->t == N_POW && b->b->t == N_NUM) {
            node *m = mk(N_NEG);
            m->a = bin(N_DIV, bin(N_MUL, bin(N_MUL, a, num(b->b->v.re)), D(b->a, v)), bin(N_POW, b->a, num(b->b->v.re + 1)));
            return m;
        }
        if (!depends(a, v)) { node *m = mk(N_NEG); m->a = bin(N_DIV, bin(N_MUL, a, D(b, v)), bin(N_POW, b, num(2))); return m; }
        return bin(N_DIV, bin(N_SUB, bin(N_MUL, D(a, v), b), bin(N_MUL, a, D(b, v))), bin(N_POW, b, num(2)));
    case N_POW:
        if (!depends(b, v)) {
            if (b->t == N_NUM) return bin(N_MUL, bin(N_MUL, num(b->v.re), bin(N_POW, a, num(b->v.re - 1))), D(a, v));
            return bin(N_MUL, bin(N_MUL, b, bin(N_POW, a, bin(N_SUB, b, num(1)))), D(a, v));
        }
        if (!depends(a, v)) {
            node *ln = (a->t == N_NUM && fabs(a->v.re - M_E) < 1e-15) ? num(1) : fn1(F_LN, a);
            return bin(N_MUL, bin(N_MUL, ln, n), D(b, v));
        }
        return bin(N_MUL, n, bin(N_ADD, bin(N_MUL, D(b, v), fn1(F_LN, a)), bin(N_DIV, bin(N_MUL, b, D(a, v)), a)));
    case N_FN: {
        node *da = D(a, v), *r;
        switch (n->f) {
        case F_SIN: r = fn1(F_COS, a); break;
        case F_COS: { r = mk(N_NEG); r->a = fn1(F_SIN, a); break; }
        case F_TAN: r = bin(N_POW, fn1(F_SEC, a), num(2)); break;
        case F_SEC: r = bin(N_MUL, fn1(F_SEC, a), fn1(F_TAN, a)); break;
        case F_CSC: { r = mk(N_NEG); r->a = bin(N_MUL, fn1(F_CSC, a), fn1(F_COT, a)); break; }
        case F_COT: { r = mk(N_NEG); r->a = bin(N_POW, fn1(F_CSC, a), num(2)); break; }
        case F_LN: return bin(N_DIV, da, a);
        case F_LOG: return bin(N_DIV, da, bin(N_MUL, a, fn1(F_LN, num(10))));
        case F_EXP: r = n; break;
        case F_SQRT: return bin(N_DIV, da, bin(N_MUL, num(2), n));
        case F_ASIN: return bin(N_DIV, da, fn1(F_SQRT, bin(N_SUB, num(1), bin(N_POW, a, num(2)))));
        case F_ACOS: { node *m = mk(N_NEG); m->a = bin(N_DIV, da, fn1(F_SQRT, bin(N_SUB, num(1), bin(N_POW, a, num(2))))); return m; }
        case F_ATAN: return bin(N_DIV, da, bin(N_ADD, num(1), bin(N_POW, a, num(2))));
        case F_SINH: r = fn1(F_COSH, a); break;
        case F_COSH: r = fn1(F_SINH, a); break;
        case F_TANH: r = bin(N_POW, fn1(F_SECH, a), num(2)); break;
        case F_SECH: { r = mk(N_NEG); r->a = bin(N_MUL, fn1(F_SECH, a), fn1(F_TANH, a)); break; }
        case F_CSCH: { r = mk(N_NEG); r->a = bin(N_MUL, fn1(F_CSCH, a), fn1(F_COTH, a)); break; }
        case F_COTH: { r = mk(N_NEG); r->a = bin(N_POW, fn1(F_CSCH, a), num(2)); break; }
        case F_ASINH: return bin(N_DIV, da, fn1(F_SQRT, bin(N_ADD, bin(N_POW, a, num(2)), num(1))));
        case F_ACOSH: return bin(N_DIV, da, fn1(F_SQRT, bin(N_SUB, bin(N_POW, a, num(2)), num(1))));
        case F_ATANH: return bin(N_DIV, da, bin(N_SUB, num(1), bin(N_POW, a, num(2))));
        case F_ABS: return bin(N_DIV, bin(N_MUL, a, da), n);
        default: return NULL;
        }
        return bin(N_MUL, da, r);
    }
    }
    return NULL;
}

node *ex_diff(node *n, char v) {
    if (!n || has_null(n)) return NULL;
    node *d = D(n, v);
    if (!d || has_null(d)) return NULL;
    d = simp(d);
    d = simp(d);
    return d;
}

/* ---- printer ---- */
static int prec(node *n) {
    switch (n->t) {
    case N_ADD: case N_SUB: return 1;
    case N_NEG: return 2;
    case N_MUL: case N_DIV: return 3;
    case N_POW: return 5;
    case N_FN: return 6;
    default: return 7;
    }
}
static char *pb;
static int pcap;
static void put(const char *s) { int L = strlen(pb); int k = strlen(s); if (L + k < pcap - 1) strcat(pb, s); }
static void pr(node *n, int ctx);
static void prp(node *n, int need) { if (prec(n) < need) { put("("); pr(n, 0); put(")"); } else pr(n, need); }
static int starts_alpha(node *n) {
    while (n->t == N_MUL || n->t == N_POW) n = n->a;
    return n->t == N_VAR || n->t == N_FN || (n->t == N_NUM && fabs(n->v.re - M_E) < 1e-15) || (n->t == N_NUM && fabs(n->v.re - M_PI) < 1e-15);
}
static int starts_digit(node *n) {
    for (;;) {
        if (n->t == N_MUL || n->t == N_DIV || n->t == N_ADD || n->t == N_SUB) { n = n->a; continue; }
        if (n->t == N_POW) { if (n->a->t == N_NUM && fabs(n->a->v.re - M_E) < 1e-15) return 0; if (prec(n->a) < 6) return 0; n = n->a; continue; }
        if (n->t == N_NEG) return 1;
        if (n->t == N_NUM) return !(fabs(n->v.re - M_E) < 1e-15 || fabs(n->v.re - M_PI) < 1e-15);
        return 0;
    }
}
static void pr(node *n, int ctx) {
    switch (n->t) {
    case N_NUM:
        if (n->v.im == 0 && fabs(n->v.re - M_E) < 1e-15) { put("e"); return; }
        if (n->v.im != 0) { put("("); put(fc(n->v)); put(")"); return; }
        { const char *s = fx(n->v.re);
          if ((ctx > 1 && (strchr(s, '/') || strchr(s, ' '))) || (ctx > 2 && s[0] == '-')) { put("("); put(s); put(")"); }
          else put(s); }
        return;
    case N_VAR:
        if (n->var == 'T') put(S_TH); else { char c[2] = {n->var, 0}; put(c); }
        return;
    case N_ADD: pr(n->a, 1); put(" + "); prp(n->b, 2); return;
    case N_SUB: pr(n->a, 1); put(" - "); prp(n->b, 2); return;
    case N_NEG: put("-"); prp(n->a, 3); return;
    case N_MUL: {
        if (n->a->t == N_NUM && n->a->v.im == 0 && fabs(n->a->v.re - M_E) > 1e-15 && n->b->t != N_DIV) {
            const char *s = fx(n->a->v.re);
            if (strchr(s, '/') || strchr(s, ' ')) { put("("); put(s); put(")"); } else put(s);
            if (prec(n->b) < 3) { put("("); pr(n->b, 0); put(")"); return; }
            if (starts_digit(n->b)) put(S_TIMES);
            prp(n->b, 3);
            return;
        }
        prp(n->a, 3);
        int bp = prec(n->b) < 3;
        if (bp) { put("("); pr(n->b, 0); put(")"); return; }
        if (starts_digit(n->b)) put(S_TIMES);
        else if (n->a->t != N_NUM) put(" ");
        prp(n->b, 4);
        return;
    }
    case N_DIV: prp(n->a, 3); put("/"); prp(n->b, 4); return;
    case N_POW:
        if (n->a->t == N_NUM && fabs(n->a->v.re - M_E) < 1e-15) {
            if (n->b->t == N_VAR) { put("e^"); pr(n->b, 7); return; }
            put("e^("); pr(n->b, 0); put(")"); return;
        }
        if (n->b->t == N_NUM && (n->b->v.re == 2 || n->b->v.re == 3)) {
            if (n->a->t == N_FN || prec(n->a) < 6 || (n->a->t == N_NUM && n->a->v.re < 0)) { put("("); pr(n->a, 0); put(")"); }
            else prp(n->a, 6);
            put(n->b->v.re == 2 ? S_SQ : S_CU);
            return;
        }
        prp(n->a, 6);
        put("^");
        if ((n->b->t == N_NUM && n->b->v.re >= 0 && n->b->v.re == floor(n->b->v.re)) || n->b->t == N_VAR) pr(n->b, 7);
        else { put("("); pr(n->b, 0); put(")"); }
        return;
    case N_FN:
        put(n->f == F_ASIN ? "sin^-1" : n->f == F_ACOS ? "cos^-1" : n->f == F_ATAN ? "tan^-1" :
            n->f == F_ASINH ? "sinh^-1" : n->f == F_ACOSH ? "cosh^-1" : n->f == F_ATANH ? "tanh^-1" : fnames[n->f]);
        if (n->f == F_SQRT) { pb[strlen(pb) - 4] = 0; put(S_SQRT); }
        put("(");
        pr(n->a, 0);
        put(")");
        return;
    case N_FACT: prp(n->a, 7); put("!"); return;
    }
}

const char *ex_str(node *n) {
    char *o = sbuf();
    pb = o; pcap = SBUF;
    if (!n) { strcpy(o, "?"); return o; }
    pr(n, 0);
    return o;
}

node *ex_simplify(node *n) { return simp(n); }
node *ex_num(double v) { return num(v); }
node *ex_bin(char op, node *a, node *b) {
    int t = op == '+' ? N_ADD : op == '-' ? N_SUB : op == '*' ? N_MUL : op == '/' ? N_DIV : N_POW;
    return bin(t, a, b);
}
int ex_const(node *n) { return isconst(n); }
