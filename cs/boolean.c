/* Boolean algebra calculator: truth table, step-by-step simplification, answer */
#include "../common/core.h"
#include <string.h>

#define OB "\x09"   /* overbar start */
#define OE "\x0d"   /* overbar end */
#define XORS "\x7f" /* circled plus glyph */

enum { BV, BC, BN, BA, BO, BX };
#define MAXC 16
typedef struct bn { int t, v, n; struct bn *c[MAXC]; } bn;
#define BPOOL 3000
static bn bpool[BPOOL];
static int bnp;
static bn *nb(int t) { if (bnp >= BPOOL) return NULL; bn *n = &bpool[bnp++]; memset(n, 0, sizeof *n); n->t = t; return n; }

/* ---------------- parser ---------------- */
static const char *bp;
static const char *berr;
static int used[26], lower[26];

static void bskip(void) { while (*bp == ' ') bp++; }
static int word(const char *w) {
    int L = strlen(w);
    for (int i = 0; i < L; i++) { char c = bp[i]; if (c >= 'a' && c <= 'z') c -= 32; if (c != w[i]) return 0; }
    char nx = bp[L];
    if ((nx >= 'a' && nx <= 'z') || (nx >= 'A' && nx <= 'Z')) return 0;
    bp += L;
    return 1;
}
static bn *p_or(void);
static bn *p_not(void);
static bn *p_prim(void) {
    bskip();
    if (berr) return NULL;
    char c = *bp;
    if (c == '(') {
        bp++;
        bn *n = p_or();
        bskip();
        if (*bp == ')') bp++;
        else if (*bp) { berr = "Missing )"; return NULL; }
        return n;
    }
    if (c == '0' || c == '1') { bp++; bn *n = nb(BC); n->v = c - '0'; return n; }
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        bp++;
        bn *n = nb(BV);
        n->v = (c >= 'a' ? c - 'a' : c - 'A');
        used[n->v] = 1;
        lower[n->v] = c >= 'a';
        return n;
    }
    berr = c ? "Unexpected symbol" : "Incomplete expression";
    return NULL;
}
static bn *p_post(void) {
    bn *a = p_prim();
    bskip();
    while (a && *bp == '\'') { bp++; bn *n = nb(BN); n->c[0] = a; n->n = 1; a = n; bskip(); }
    return a;
}
static bn *p_not(void) {
    bskip();
    if (*bp == '!' || *bp == '~' || word("NOT")) {
        if (*bp == '!' || *bp == '~') bp++;
        bn *n = nb(BN);
        n->c[0] = p_not();
        n->n = 1;
        if (!n->c[0] && !berr) berr = "NOT needs something after it";
        return n;
    }
    return p_post();
}
static int at_and(void) {
    bskip();
    if (*bp == '.' || *bp == '*' || *bp == '&' || *bp == 0x17) { bp++; return 1; }
    if (word("AND")) return 1;
    /* implicit AND: next token starts a primary (not an operator word) */
    const char *s = bp;
    if (word("OR") || word("XOR") || word("AND")) { bp = s; return 0; }
    char c = *bp;
    if (c == '(' || c == '!' || c == '~' || c == '0' || c == '1' || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return 1;
    return 0;
}
static bn *p_xor(void) {
    bn *a = p_not();
    for (;;) {
        bskip();
        if (!a || berr) return a;
        if (*bp == '^' || *bp == 0x7f || word("XOR")) {
            if (*bp == '^' || *bp == 0x7f) bp++;
            bn *b = p_not();
            if (!b) { if (!berr) berr = "Missing term after XOR"; return NULL; }
            bn *n = nb(BX); n->c[0] = a; n->c[1] = b; n->n = 2; a = n;
        } else return a;
    }
}
static bn *p_and(void) {
    bn *a = p_xor();
    while (a && !berr && at_and()) {
        bn *b = p_xor();
        if (!b) { if (!berr) berr = "Missing term after AND"; return NULL; }
        if (a->t == BA && a->n < MAXC) a->c[a->n++] = b;
        else { bn *n = nb(BA); n->c[0] = a; n->c[1] = b; n->n = 2; a = n; }
    }
    return a;
}
static bn *p_or(void) {
    bn *a = p_and();
    for (;;) {
        bskip();
        if (!a || berr) return a;
        if (*bp == '+' || *bp == '|' || word("OR")) {
            if (*bp == '+' || *bp == '|') bp++;
            bn *b = p_and();
            if (!b) { if (!berr) berr = "Missing term after OR"; return NULL; }
            if (a->t == BO && a->n < MAXC) a->c[a->n++] = b;
            else { bn *n = nb(BO); n->c[0] = a; n->c[1] = b; n->n = 2; a = n; }
        } else return a;
    }
}
static bn *bparse(const char *s, const char **err) {
    bnp = 0;
    memset(used, 0, sizeof used);
    memset(lower, 0, sizeof lower);
    bp = s; berr = NULL;
    bn *n = p_or();
    bskip();
    if (!berr && *bp) berr = *bp == ')' ? "Extra )" : "Unexpected symbol";
    if (!berr && !n) berr = "Empty";
    if (!berr) { int k = 0; for (int i = 0; i < 26; i++) k += used[i]; if (k > 8) berr = "Max 8 variables"; }
    *err = berr;
    return berr ? NULL : n;
}
static const char *check_expr(const char *s) { const char *e; bparse(s, &e); return e; }

/* ---------------- printing ---------------- */
static int bprec(bn *n) { return n->t == BO ? 1 : n->t == BA ? 2 : n->t == BX ? 3 : 4; }
static char *ob;
static int ocap;
static void op(const char *s) { int L = strlen(ob), k = strlen(s); if (L + k < ocap - 1) strcat(ob, s); }
static void bpr(bn *n);
static void bprp(bn *n, int need) { if (bprec(n) < need) { op("("); bpr(n); op(")"); } else bpr(n); }
static void bpr(bn *n) {
    char t[2] = {0, 0};
    switch (n->t) {
    case BV: t[0] = (lower[n->v] ? 'a' : 'A') + n->v; op(t); return;
    case BC: op(n->v ? "1" : "0"); return;
    case BN: op(OB); bpr(n->c[0]); op(OE); return;
    case BA: for (int i = 0; i < n->n; i++) { if (i) op("."); bprp(n->c[i], 3); } return;
    case BX: bprp(n->c[0], 4); op(" " XORS " "); bprp(n->c[1], 4); return;
    case BO: for (int i = 0; i < n->n; i++) { if (i) op(" + "); bprp(n->c[i], 2); } return;
    }
}
static char bigbuf[8][400];
static int bbi;
static const char *bstr(bn *n) { bbi = (bbi + 1) % 8; ob = bigbuf[bbi]; ocap = 400; ob[0] = 0; bpr(n); return ob; }

/* ---------------- evaluation ---------------- */
static int nv, vidx[26], vlist[8];
static int beval(bn *n, int asg) {
    switch (n->t) {
    case BV: return asg >> vidx[n->v] & 1;
    case BC: return n->v;
    case BN: return !beval(n->c[0], asg);
    case BA: for (int i = 0; i < n->n; i++) if (!beval(n->c[i], asg)) return 0; return 1;
    case BO: for (int i = 0; i < n->n; i++) if (beval(n->c[i], asg)) return 1; return 0;
    case BX: return beval(n->c[0], asg) ^ beval(n->c[1], asg);
    }
    return 0;
}

/* ---------------- tree rewriting ---------------- */
static bn *copy(bn *n) { bn *m = nb(n->t); *m = *n; for (int i = 0; i < n->n; i++) m->c[i] = copy(n->c[i]); return m; }
static bn *mknot(bn *a) { bn *n = nb(BN); n->c[0] = a; n->n = 1; return n; }

static void flatten(bn *n) {
    for (int i = 0; i < n->n; i++) flatten(n->c[i]);
    if (n->t != BA && n->t != BO) return;
    bn *k[MAXC * 4];
    int m = 0;
    for (int i = 0; i < n->n; i++) {
        if (n->c[i]->t == n->t) for (int j = 0; j < n->c[i]->n; j++) k[m++] = n->c[i]->c[j];
        else k[m++] = n->c[i];
    }
    if (m > MAXC) m = MAXC;
    for (int i = 0; i < m; i++) n->c[i] = k[i];
    n->n = m;
}

static int has_xor(bn *n) { if (n->t == BX) return 1; for (int i = 0; i < n->n; i++) if (has_xor(n->c[i])) return 1; return 0; }
static bn *rm_xor(bn *n) {
    for (int i = 0; i < n->n; i++) n->c[i] = rm_xor(n->c[i]);
    if (n->t != BX) return n;
    bn *a = n->c[0], *b = n->c[1];
    bn *t1 = nb(BA); t1->c[0] = a; t1->c[1] = mknot(copy(b)); t1->n = 2;
    bn *t2 = nb(BA); t2->c[0] = mknot(copy(a)); t2->c[1] = copy(b); t2->n = 2;
    bn *o = nb(BO); o->c[0] = t1; o->c[1] = t2; o->n = 2;
    return o;
}

/* apply one NOT-pushing rewrite (outermost first); returns law name or NULL */
static const char *law_used;
static int push_once(bn **pn) {
    bn *n = *pn;
    if (n->t == BN) {
        bn *c = n->c[0];
        if (c->t == BN) { *pn = c->c[0]; law_used = "Double negation"; return 1; }
        if (c->t == BC) { bn *k = nb(BC); k->v = !c->v; *pn = k; law_used = "NOT 0 = 1, NOT 1 = 0"; return 1; }
        if (c->t == BA || c->t == BO) {
            bn *m = nb(c->t == BA ? BO : BA);
            m->n = c->n;
            for (int i = 0; i < c->n; i++) m->c[i] = mknot(c->c[i]);
            *pn = m;
            law_used = "De Morgan's law";
            return 1;
        }
    }
    for (int i = 0; i < n->n; i++) if (push_once(&n->c[i])) return 1;
    return 0;
}

static int bneq(bn *a, bn *b) {
    if (a->t != b->t || a->v != b->v || a->n != b->n) return 0;
    for (int i = 0; i < a->n; i++) if (!bneq(a->c[i], b->c[i])) return 0;
    return 1;
}
static bn *mkc(int v) { bn *k = nb(BC); k->v = v; return k; }
/* one constant/idempotent rewrite in the tree */
static int tidy_once(bn **pn) {
    bn *n = *pn;
    if (n->t == BA || n->t == BO) {
        int isand = n->t == BA;
        for (int i = 0; i < n->n; i++) {
            bn *c = n->c[i];
            if (c->t != BC) continue;
            if (isand && c->v == 0) { *pn = mkc(0); law_used = "Annulment law: X.0 = 0"; return 1; }
            if (!isand && c->v == 1) { *pn = mkc(1); law_used = "Annulment law: X + 1 = 1"; return 1; }
            /* identity: X.1 = X, X + 0 = X */
            for (int j = i; j < n->n - 1; j++) n->c[j] = n->c[j + 1];
            n->n--;
            law_used = isand ? "Identity law: X.1 = X" : "Identity law: X + 0 = X";
            if (n->n == 1) *pn = n->c[0];
            else if (n->n == 0) *pn = mkc(isand ? 1 : 0);
            return 1;
        }
        for (int i = 0; i < n->n; i++)
            for (int j = i + 1; j < n->n; j++)
                if (bneq(n->c[i], n->c[j])) {
                    for (int k = j; k < n->n - 1; k++) n->c[k] = n->c[k + 1];
                    n->n--;
                    law_used = isand ? "Idempotent law: X.X = X" : "Idempotent law: X + X = X";
                    if (n->n == 1) *pn = n->c[0];
                    return 1;
                }
        /* complement in a flat AND/OR of literals: X.~X = 0, X + ~X = 1 */
        for (int i = 0; i < n->n; i++)
            for (int j = 0; j < n->n; j++)
                if (n->c[j]->t == BN && bneq(n->c[j]->c[0], n->c[i])) {
                    *pn = mkc(isand ? 0 : 1);
                    law_used = isand ? "Complement law: X." OB "X" OE " = 0" : "Complement law: X + " OB "X" OE " = 1";
                    return 1;
                }
    }
    for (int i = 0; i < n->n; i++) if (tidy_once(&n->c[i])) return 1;
    return 0;
}

/* ---------------- sum of products ---------------- */
typedef struct { int pos, neg, dpos, dneg, dead; } term;
#define MAXT 128
typedef struct { term t[MAXT]; int n; int overflow; } sop;

static int is_sop(bn *n) {
    /* OR of ANDs of literals (or a single such) */
    if (n->t == BV || n->t == BC) return 1;
    if (n->t == BN) return n->c[0]->t == BV;
    if (n->t == BA) { for (int i = 0; i < n->n; i++) if (!is_sop(n->c[i]) || n->c[i]->t == BA || n->c[i]->t == BO) return 0; return 1; }
    if (n->t == BO) { for (int i = 0; i < n->n; i++) { bn *c = n->c[i]; if (c->t == BO) return 0; if (!is_sop(c)) return 0; } return 1; }
    return 0;
}

static void expand(bn *n, sop *s) {
    s->n = 0; s->overflow = 0;
    switch (n->t) {
    case BV: s->t[0] = (term){1 << vidx[n->v], 0, 0}; s->n = 1; return;
    case BC: if (n->v) { s->t[0] = (term){0, 0, 0}; s->n = 1; } return;
    case BN: s->t[0] = (term){0, 1 << vidx[n->c[0]->v], 0}; s->n = 1; return; /* NOTs are on variables now */
    case BO: {
        static sop tmp[6];
        static int depth;
        sop *k = &tmp[depth++ % 6];
        for (int i = 0; i < n->n; i++) {
            expand(n->c[i], k);
            for (int j = 0; j < k->n; j++) { if (s->n < MAXT) s->t[s->n++] = k->t[j]; else s->overflow = 1; }
            if (k->overflow) s->overflow = 1;
        }
        depth--;
        return;
    }
    case BA: {
        static sop tmp2[6], acc2[6];
        static int depth2;
        int d = depth2++ % 6;
        sop *k = &tmp2[d], *a = &acc2[d];
        a->n = 1; a->t[0] = (term){0, 0, 0}; a->overflow = 0;
        for (int i = 0; i < n->n; i++) {
            expand(n->c[i], k);
            sop r; r.n = 0; r.overflow = a->overflow || k->overflow;
            for (int x = 0; x < a->n; x++)
                for (int y = 0; y < k->n; y++) {
                    if (r.n >= MAXT) { r.overflow = 1; continue; }
                    {
                        term A_ = a->t[x], B_ = k->t[y];
                        r.t[r.n++] = (term){A_.pos | B_.pos, A_.neg | B_.neg,
                                            A_.dpos | B_.dpos | (A_.pos & B_.pos), A_.dneg | B_.dneg | (A_.neg & B_.neg), 0};
                    }
                }
            *a = r;
        }
        *s = *a;
        depth2--;
        return;
    }
    }
}

static const char *tstr(term t) {
    char *o = sbuf();
    if (!t.pos && !t.neg) { strcpy(o, "1"); return o; }
    int first = 1;
    for (int i = 0; i < nv; i++) {
        int b = 1 << i;
        char L[2] = {(char)((lower[vlist[i]] ? 'a' : 'A') + vlist[i]), 0};
        for (int r = 0; r < ((t.dpos & b) ? 2 : 1); r++)
            if (t.pos & b) { if (!first) strcat(o, "."); strcat(o, L); first = 0; }
        for (int r = 0; r < ((t.dneg & b) ? 2 : 1); r++)
            if (t.neg & b) { if (!first) strcat(o, "."); strcat(o, OB); strcat(o, L); strcat(o, OE); first = 0; }
    }
    return o;
}
static const char *sstr(sop *s) {
    bbi = (bbi + 1) % 8;
    char *o = bigbuf[bbi];
    o[0] = 0;
    int k = 0;
    for (int i = 0; i < s->n; i++) {
        if (s->t[i].dead) continue;
        if (k++) strcat(o, " + ");
        const char *t = tstr(s->t[i]);
        if (strlen(o) + strlen(t) < 390) strcat(o, t);
    }
    if (!k) strcpy(o, "0");
    return o;
}
static int lits(term t) { return __builtin_popcount(t.pos) + __builtin_popcount(t.neg); }
static int s_eval(sop *s, int asg) {
    for (int i = 0; i < s->n; i++) {
        term t = s->t[i];
        if (t.dead) continue;
        if ((asg & t.pos) == t.pos && (asg & t.neg) == 0) return 1;
    }
    return 0;
}
static void compact(sop *s) { int m = 0; for (int i = 0; i < s->n; i++) if (!s->t[i].dead) s->t[m++] = s->t[i]; s->n = m; }

static int show;
static void step(const char *law, const char *detail, sop *s) {
    if (!show) return;
    if (detail && detail[0]) outf(HEAD "%s: %s", law, detail);
    else outf(HEAD "%s", law);
    outf("= %s", sstr(s));
}
/* print the sum with term i shown as repl and term j left out */
static const char *sstr_mod(sop *s, int i, const char *repl, int j) {
    bbi = (bbi + 1) % 8;
    char *o = bigbuf[bbi];
    o[0] = 0;
    int k = 0;
    for (int x = 0; x < s->n; x++) {
        if (s->t[x].dead || x == j) continue;
        if (k++) strcat(o, " + ");
        const char *t = x == i ? repl : tstr(s->t[x]);
        if (strlen(o) + strlen(t) < 390) strcat(o, t);
    }
    if (!k) strcpy(o, "0");
    return o;
}
static int red_explained;
static void line(const char *law, const char *expr) {
    if (!show) return;
    outf(HEAD "%s", law);
    if (strncmp(law, "Redundancy", 10) == 0 && !red_explained) {
        outf("(A + " OB "A" OE ".B = (A + " OB "A" OE ").(A + B) = 1.(A + B))");
        red_explained = 1;
    }
    outf("= %s", expr);
}
static const char *lit(int v, int neg) {
    char *o = sbuf();
    char L[2] = {(char)((lower[vlist[v]] ? 'a' : 'A') + vlist[v]), 0};
    if (neg) { strcpy(o, OB); strcat(o, L); strcat(o, OE); } else strcpy(o, L);
    return o;
}
/* "X." prefix or "" when X is empty */
static const char *pre(term x) { char *o = sbuf(); if (x.pos || x.neg) { strcpy(o, tstr(x)); strcat(o, "."); } return o; }

static int simplify_once(sop *s) {
    /* idempotent inside terms: A.A = A */
    for (int i = 0; i < s->n; i++)
        if (s->t[i].dpos || s->t[i].dneg) {
            for (int k = 0; k < s->n; k++) s->t[k].dpos = s->t[k].dneg = 0;
            step("Idempotent law", "A.A = A", s);
            return 1;
        }
    /* complement inside a term: A.NOT A = 0 */
    for (int i = 0; i < s->n; i++)
        if (s->t[i].pos & s->t[i].neg) {
            int b = s->t[i].pos & s->t[i].neg, v = 0;
            while (!(b >> v & 1)) v++;
            char d[160];
            strcpy(d, lit(v, 0)); strcat(d, "."); strcat(d, lit(v, 1)); strcat(d, " = 0, and X.0 = 0, X + 0 = X");
            s->t[i].dead = 1; compact(s);
            step("Complement & annulment laws", d, s);
            return 1;
        }
    for (int i = 0; i < s->n; i++)
        if (!s->t[i].pos && !s->t[i].neg && s->n > 1) {
            s->t[0] = s->t[i]; s->n = 1;
            step("Annulment law", "X + 1 = 1", s);
            return 1;
        }
    for (int i = 0; i < s->n; i++)
        for (int j = i + 1; j < s->n; j++)
            if (s->t[i].pos == s->t[j].pos && s->t[i].neg == s->t[j].neg) {
                s->t[j].dead = 1; compact(s);
                step("Idempotent law", "X + X = X", s);
                return 1;
            }
    /* absorption: X + X.Y = X */
    for (int i = 0; i < s->n; i++)
        for (int j = 0; j < s->n; j++) {
            if (i == j) continue;
            term a = s->t[i], b = s->t[j];
            if ((a.pos & b.pos) == a.pos && (a.neg & b.neg) == a.neg) {
                term y = {b.pos & ~a.pos, b.neg & ~a.neg, 0, 0, 0};
                char d[300];
                strcpy(d, tstr(a)); strcat(d, " + "); strcat(d, tstr(b)); strcat(d, " = ");
                strcat(d, pre(a)); strcat(d, "(1 + "); strcat(d, tstr(y)); strcat(d, ") = ");
                if (a.pos || a.neg) { strcat(d, pre(a)); strcat(d, "1 = "); }
                strcat(d, tstr(a));
                s->t[j].dead = 1; compact(s);
                step("Absorption law (1 + Y = 1)", d, s);
                return 1;
            }
        }
    /* combine X.B + X.NOT B = X in three written steps */
    for (int i = 0; i < s->n; i++)
        for (int j = i + 1; j < s->n; j++) {
            term a = s->t[i], b = s->t[j];
            if ((a.pos | a.neg) != (b.pos | b.neg)) continue;
            int diff = a.pos ^ b.pos;
            if (__builtin_popcount(diff) != 1 || (a.neg ^ b.neg) != diff) continue;
            term c = {a.pos & ~diff, a.neg & ~diff, 0, 0, 0};
            int v = 0;
            while (!(diff >> v & 1)) v++;
            char f[200];
            strcpy(f, pre(c)); strcat(f, "("); strcat(f, lit(v, 0)); strcat(f, " + "); strcat(f, lit(v, 1)); strcat(f, ")");
            if (show) {
                char law[200];
                strcpy(law, "Distributive law (factorise out ");
                strcat(law, (c.pos || c.neg) ? tstr(c) : "nothing");
                strcat(law, ")");
                line(law, sstr_mod(s, i, f, j));
                char law2[120];
                strcpy(law2, "Complement law: "); strcat(law2, lit(v, 0)); strcat(law2, " + "); strcat(law2, lit(v, 1)); strcat(law2, " = 1");
                char g[160];
                strcpy(g, pre(c)); strcat(g, "1");
                if (!(c.pos || c.neg)) strcpy(g, "1");
                line(law2, sstr_mod(s, i, g, j));
            }
            s->t[i] = c; s->t[j].dead = 1; compact(s);
            if (c.pos || c.neg) step("Identity law", "X.1 = X", s);
            return 1;
        }
    /* redundancy: X.l + X.NOT l.Y = X.l + X.Y */
    for (int i = 0; i < s->n; i++)
        for (int j = 0; j < s->n; j++) {
            if (i == j) continue;
            term a = s->t[i], b = s->t[j];
            for (int v = 0; v < nv; v++) {
                int bit = 1 << v;
                int has = (a.pos & bit) ? 1 : (a.neg & bit) ? 2 : 0;
                if (!has) continue;
                int opp = has == 1 ? (b.neg & bit) : (b.pos & bit);
                if (!opp) continue;
                term x = {a.pos & ~bit, a.neg & ~bit, 0, 0, 0};
                if ((x.pos & b.pos) != x.pos || (x.neg & b.neg) != x.neg) continue;
                term y = {b.pos & ~x.pos & ~bit, b.neg & ~x.neg & ~bit, 0, 0, 0};
                term nbt = b; nbt.pos &= ~bit; nbt.neg &= ~bit;
                const char *l = lit(v, has == 2), *ln = lit(v, has == 1);
                int hasx = x.pos || x.neg;
                if (show && hasx) {
                    char f[300], law[160];
                    strcpy(f, pre(x)); strcat(f, "("); strcat(f, l); strcat(f, " + "); strcat(f, ln); strcat(f, "."); strcat(f, tstr(y)); strcat(f, ")");
                    strcpy(law, "Distributive law (factorise out "); strcat(law, tstr(x)); strcat(law, ")");
                    line(law, sstr_mod(s, i, f, j));
                    strcpy(f, pre(x)); strcat(f, "("); strcat(f, l); strcat(f, " + "); strcat(f, tstr(y)); strcat(f, ")");
                    line("Redundancy law: A + " OB "A" OE ".B = A + B", sstr_mod(s, i, f, j));
                }
                s->t[j] = nbt;
                if (hasx) step("Distributive law (expand)", "", s);
                else {
                    if (show && !red_explained) { outf(HEAD "Redundancy law: A + " OB "A" OE ".B = A + B"); outf("(A + " OB "A" OE ".B = (A + " OB "A" OE ").(A + B) = 1.(A + B))"); outf("= %s", sstr(s)); red_explained = 1; }
                    else step("Redundancy law", "A + " OB "A" OE ".B = A + B", s);
                }
                return 1;
            }
        }
    /* consensus: written out with (V + NOT V) = 1 */
    for (int i = 0; i < s->n; i++)
        for (int j = i + 1; j < s->n; j++) {
            term a = s->t[i], b = s->t[j];
            int opp = (a.pos & b.neg) | (a.neg & b.pos);
            if (__builtin_popcount(opp) != 1) continue;
            term cons = {(a.pos | b.pos) & ~opp, (a.neg | b.neg) & ~opp, 0, 0, 0};
            if (cons.pos & cons.neg) continue;
            for (int k = 0; k < s->n; k++) {
                if (k == i || k == j) continue;
                term t = s->t[k];
                if ((cons.pos & t.pos) != cons.pos || (cons.neg & t.neg) != cons.neg) continue;
                int v = 0;
                while (!(opp >> v & 1)) v++;
                if (show) {
                    char f[200];
                    strcpy(f, tstr(t)); strcat(f, ".("); strcat(f, lit(v, 0)); strcat(f, " + "); strcat(f, lit(v, 1)); strcat(f, ")");
                    line("Complement + identity laws: X = X.(V + " OB "V" OE ")", sstr_mod(s, k, f, -1));
                    term t1 = t, t2 = t;
                    t1.pos |= opp; t2.neg |= opp;
                    char g[260];
                    strcpy(g, tstr(t1)); strcat(g, " + "); strcat(g, tstr(t2));
                    line("Distributive law (expand)", sstr_mod(s, k, g, -1));
                    outf(HEAD "Absorption law: each new term is absorbed");
                    outf("by %s and %s", tstr(a), tstr(b));
                }
                s->t[k].dead = 1; compact(s);
                if (show) outf("= %s", sstr(s));
                return 1;
            }
        }
    return 0;
}

static int total_lits_(sop *s) { int k = 0; for (int i = 0; i < s->n; i++) k += __builtin_popcount(s->t[i].pos) + __builtin_popcount(s->t[i].neg); return k; }

/* fallback: expand to full terms (minterms) and combine pairs algebraically */
static void minterm_route(int *tt, sop *best) {
    int full = 1 << nv;
    sop m;
    m.n = 0;
    for (int a = 0; a < full && m.n < MAXT; a++)
        if (tt[a]) m.t[m.n++] = (term){a, (full - 1) & ~a, 0, 0, 0};
    outf(HEAD "Complement + identity laws: multiply each term by (V + " OB "V" OE ") = 1 for missing variables, then expand");
    outf("= %s", sstr(&m));
    outf(HEAD "Idempotent law: a term can be repeated (X = X + X), so terms can be grouped more than once");
    for (int i = 0; i < best->n; i++) {
        term p = best->t[i];
        int fixed = p.pos | p.neg;
        char *o = sbuf();
        int k = 0;
        for (int a = 0; a < full; a++) {
            if ((a & fixed) != p.pos) continue;
            if (k++) strcat(o, " + ");
            term mt = {a, (full - 1) & ~a, 0, 0, 0};
            const char *ts = tstr(mt);
            if (strlen(o) + strlen(ts) < SBUF - 30) strcat(o, ts); else { strcat(o, "..."); break; }
        }
        if (k > 1) {
            outf("%s", o);
            outf("  = %s  (factorise, V + " OB "V" OE " = 1, X.1 = X)", tstr(p));
        }
    }
    outf(HEAD "Result");
    outf("= %s", sstr(best));
}

/* ---------------- Quine-McCluskey (minimal SOP) ---------------- */
static int qm_best_n, qm_best_l, qm_pick[64], qm_cur[64];
static int pi_v[256], pi_m[256], npi;
static int need[256], nneed;
static void cover_rec(int k, int nsel, int lit, unsigned char *covered) {
    if (nsel > qm_best_n || (nsel == qm_best_n && lit >= qm_best_l)) return;
    int first = -1;
    for (int i = 0; i < nneed; i++) if (!covered[i]) { first = i; break; }
    if (first < 0) { qm_best_n = nsel; qm_best_l = lit; memcpy(qm_pick, qm_cur, sizeof(int) * nsel); return; }
    if (nsel >= 63) return;
    int m = need[first];
    for (int p = 0; p < npi; p++) {
        if ((m & ~pi_m[p]) != (pi_v[p] & ~pi_m[p])) continue;
        unsigned char c2[256];
        memcpy(c2, covered, nneed);
        for (int i = 0; i < nneed; i++) if ((need[i] & ~pi_m[p]) == (pi_v[p] & ~pi_m[p])) c2[i] = 1;
        qm_cur[nsel] = p;
        cover_rec(k + 1, nsel + 1, lit + nv - __builtin_popcount(pi_m[p]), c2);
    }
}
static void qm(int *tt, sop *out) {
    int full = 1 << nv;
    int v[1024], m[1024], used_[1024], n = 0;
    nneed = 0;
    for (int a = 0; a < full; a++) if (tt[a]) { v[n] = a; m[n] = 0; n++; need[nneed++] = a; }
    out->n = 0;
    if (!nneed) return;
    if (nneed == full) { out->t[0] = (term){0, 0, 0}; out->n = 1; return; }
    npi = 0;
    int start = 0, end = n;
    while (start < end) {
        for (int i = start; i < end; i++) used_[i] = 0;
        int nn = end;
        for (int i = start; i < end; i++)
            for (int j = i + 1; j < end; j++) {
                if (m[i] != m[j]) continue;
                int d = v[i] ^ v[j];
                if (__builtin_popcount(d) != 1) continue;
                used_[i] = used_[j] = 1;
                int nvv = v[i] & ~d, nm = m[i] | d, dup = 0;
                for (int k = end; k < nn; k++) if (v[k] == nvv && m[k] == nm) dup = 1;
                if (!dup && nn < 1000) { v[nn] = nvv; m[nn] = nm; used_[nn] = 0; nn++; }
            }
        for (int i = start; i < end; i++) if (!used_[i] && npi < 256) {
            int dup = 0;
            for (int k = 0; k < npi; k++) if (pi_v[k] == v[i] && pi_m[k] == m[i]) dup = 1;
            if (!dup) { pi_v[npi] = v[i]; pi_m[npi] = m[i]; npi++; }
        }
        start = end; end = nn;
    }
    qm_best_n = 99; qm_best_l = 999;
    unsigned char cov[256] = {0};
    cover_rec(0, 0, 0, cov);
    for (int i = 0; i < qm_best_n && i < 64; i++) {
        int p = qm_pick[i];
        term t = {0, 0, 0};
        for (int b = 0; b < nv; b++) {
            if (pi_m[p] >> b & 1) continue;
            if (pi_v[p] >> b & 1) t.pos |= 1 << b; else t.neg |= 1 << b;
        }
        out->t[out->n++] = t;
    }
}

static int total_lits(sop *s) { int k = 0; for (int i = 0; i < s->n; i++) k += lits(s->t[i]); return k; }

/* factorised form if all terms share literals */
static const char *factored(sop *s) {
    if (s->n < 2) return NULL;
    int cp = ~0, cn = ~0;
    for (int i = 0; i < s->n; i++) { cp &= s->t[i].pos; cn &= s->t[i].neg; }
    if (!cp && !cn) return NULL;
    term c = {cp, cn, 0};
    bbi = (bbi + 1) % 8;
    char *o = bigbuf[bbi];
    strcpy(o, tstr(c));
    strcat(o, ".(");
    for (int i = 0; i < s->n; i++) {
        term r = {s->t[i].pos & ~cp, s->t[i].neg & ~cn, 0};
        if (i) strcat(o, " + ");
        strcat(o, tstr(r));
    }
    strcat(o, ")");
    return o;
}

/* ---------------- calculator ---------------- */
static const char *bk_lab[3][6] = {
    {"A", "B", "C", "D", "NOT", ">>"},
    {".", "+", "XOR", "(", ")", ">>"},
    {"E", "F", "0", "1", "'", ">>"},
};
static const char *bk_ins[3][5] = {
    {"A", "B", "C", "D", "!"},
    {".", "+", "^", "(", ")"},
    {"E", "F", "0", "1", "'"},
};

void bool_calc(void) {
    ui_set_fkeys(bk_lab, bk_ins);
    in_note("AND . or *   OR +   NOT ! or '   XOR ^");
    const char *src = in_text_err("Expression", "e.g. !(A.B) + A.!B   (F1-F5 keys help)", check_expr);
    static const char *opts[] = {"Truth table", "Workings (step by step)", "Just the answer"};
    int mode = in_choice("Show", opts, 3);
    const char *err;
    bn *root = bparse(src, &err);
    if (!root) fail(err);
    nv = 0;
    for (int i = 0; i < 26; i++) if (used[i]) { vidx[i] = nv; vlist[nv++] = i; }
    int full = 1 << nv;
    static int tt[256];
    /* note: bit i of the assignment = variable i */
    for (int a = 0; a < full; a++) tt[a] = beval(root, a);

    if (mode == 0) {
        char *h = sbuf();
        for (int i = 0; i < nv; i++) { char t[4] = {(char)((lower[vlist[i]] ? 'a' : 'A') + vlist[i]), ' ', ' ', 0}; strcat(h, t); }
        strcat(h, "| Q");
        outf(HEAD "Q = %s", bstr(root));
        out_line(h);
        for (int r = 0; r < full; r++) {
            /* rows count up in binary with the first variable as the most significant bit */
            int a = 0;
            for (int i = 0; i < nv; i++) if (r >> (nv - 1 - i) & 1) a |= 1 << i;
            char *o = sbuf();
            for (int i = 0; i < nv; i++) strcat(o, (a >> i & 1) ? "1  " : "0  ");
            strcat(o, tt[a] ? "| 1" : "| 0");
            if (tt[a]) { bbi = (bbi + 1) % 8; char *x = bigbuf[bbi]; strcpy(x, ANS); strcat(x, o); out_line(x); }
            else out_line(o);
        }
        return;
    }

    show = (mode == 1);
    red_explained = 0;
    if (show) outf(HEAD "Start: %s", bstr(root));
    flatten(root);
    if (has_xor(root)) {
        root = rm_xor(root);
        flatten(root);
        if (show) { outf(HEAD "Expand XOR: A " XORS " B = A." OB "B" OE " + " OB "A" OE ".B"); outf("= %s", bstr(root)); }
    }
    for (int k = 0; k < 60; k++) {
        if (!tidy_once(&root) && !push_once(&root)) break;
        flatten(root);
        if (show) { outf(HEAD "%s", law_used); outf("= %s", bstr(root)); }
    }
    static sop s;
    expand(root, &s);
    if (s.overflow) {
        /* too big to expand by hand: go straight to K-map result */
        qm(tt, &s);
        if (show) outf(HEAD "Too many terms to expand directly; working from the truth table");
    } else if (show && !is_sop(root)) {
        outf(HEAD "Expand brackets (distributive law)");
        outf("= %s", sstr(&s));
    }
    for (int k = 0; k < 60; k++) if (!simplify_once(&s)) break;
    static sop best;
    qm(tt, &best);
    if (total_lits(&best) < total_lits(&s) || best.n < s.n) {
        if (show) minterm_route(tt, &best);
        s = best;
    }
    const char *f = factored(&s);
    if (show && f) { outf(HEAD "Factorise (distributive law)"); outf("= %s", f); }
    /* spot XOR / XNOR pairs among the terms */
    const char *xr = NULL;
    for (int i = 0; i < s.n && !xr; i++)
        for (int j = i + 1; j < s.n && !xr; j++) {
            term t0 = s.t[i], t1 = s.t[j];
            if (lits(t0) != 2 || lits(t1) != 2) continue;
            int vs = t0.pos | t0.neg;
            if (vs != (t1.pos | t1.neg)) continue;
            int p0 = __builtin_popcount(t0.pos), p1 = __builtin_popcount(t1.pos);
            int a = 0; while (!(vs >> a & 1)) a++;
            int b = a + 1; while (!(vs >> b & 1)) b++;
            char x[80];
            if (p0 == 1 && p1 == 1 && t0.pos != t1.pos) { strcpy(x, lit(a, 0)); strcat(x, " " XORS " "); strcat(x, lit(b, 0)); }
            else if ((p0 == 2 && p1 == 0) || (p0 == 0 && p1 == 2)) { strcpy(x, OB); strcat(x, lit(a, 0)); strcat(x, " " XORS " "); strcat(x, lit(b, 0)); strcat(x, OE); }
            else continue;
            if (s.n == 2) { char *o = sbuf(); strcpy(o, x); xr = o; }
            else {
                char g[120];
                strcpy(g, x);
                if (strchr(x, ' ') && s.n > 2) { strcpy(g, "("); strcat(g, x); strcat(g, ")"); }
                const char *full = sstr_mod(&s, i, g, j);
                char *o = sbuf(); strncpy(o, full, SBUF - 1); o[SBUF - 1] = 0; xr = o;
            }
            if (show) { outf(HEAD "XOR: A." OB "B" OE " + " OB "A" OE ".B = A " XORS " B"); outf("= %s", xr); }
        }
    if (show) out_hr();
    outf(ANS "Simplified: %s", sstr(&s));
    if (xr) outf(ANS "= %s", xr);
    if (f) outf(ANS "Factorised: %s", f);
    if (show) outf(OB "X" OE " means NOT X");
}
