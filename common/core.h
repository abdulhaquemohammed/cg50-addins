#ifndef CORE_H
#define CORE_H
#include <math.h>
#include <stddef.h>

#ifndef M_E
#define M_E 2.71828182845904523536
#endif
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* ---------- symbols (custom glyphs in calcfont, single bytes) ---------- */
#define S_SQRT  "\x03"
#define S_PI    "\x04"
#define S_TH    "\x05"
#define S_AL    "\x06"
#define S_BE    "\x07"
#define S_GA    "\x08"
#define S_DE    "\x0b"
#define S_LA    "\x0c"
#define S_MU    "\x0e"
#define S_SIG   "\x0f"
#define S_SG    "\x10"
#define S_OM    "\x11"
#define S_RHO   "\x12"
#define S_CHI   "\x13"
#define S_DEG   "\x14"
#define S_SQ    "\x15"
#define S_CU    "\x16"
#define S_TIMES "\x17"
#define S_PM    "\x18"
#define S_LE    "\x19"
#define S_GE    "\x1a"
#define S_NE    "\x1b"
#define S_INF   "\x1c"
#define S_INT   "\x1d"
#define S_ARR   "\x1e"
#define S_APX   "\x1f"
#define S_DOT   "."
#define HEAD    "\x01"   /* line prefix: heading (blue) */
#define ANS     "\x02"   /* line prefix: answer (red) */

/* ---------- complex ---------- */
typedef struct { double re, im; } cx;
static inline cx C(double r, double i) { cx z = {r, i}; return z; }
static inline cx R(double r) { cx z = {r, 0}; return z; }
cx cadd(cx a, cx b); cx csub(cx a, cx b); cx cmul(cx a, cx b); cx cdiv(cx a, cx b);
cx cneg(cx a); cx cconj(cx a); double cabs_(cx a); double carg_(cx a);
cx cexp_(cx a); cx clog_(cx a); cx cpow_(cx a, cx b); cx csqrt_(cx a);
cx cpolar(double r, double t);
cx cpowi(cx a, int n);

/* ---------- output buffer ---------- */
void out_reset(void);
void outf(const char *fmt, ...);     /* supports %s %d %c only; '\n' splits lines */
void out_line(const char *s);
int out_count(void);
const char *out_get(int i);
void out_hr(void);                   /* separator line */

/* ---------- number formatting ---------- */
const char *fd(double x);            /* decimal, 6 s.f. */
const char *fdn(double x, int sig);  /* decimal, sig s.f. */
const char *fdp(double x, int dp);   /* fixed dp */
const char *fx(double x);            /* exact if recognisable, else decimal */
const char *fxd(double x);
const char *fxp(double x);
const char *fshift(const char *var, double a);
const char *fcp(cx z);            /* fx with brackets if negative/compound */           /* exact (≈ decimal) */
const char *fc(cx z);                /* complex a + bi, exact parts */
const char *fang(double t);          /* angle: exact rad + degrees */
const char *fdeg(double t);          /* degrees only */
int is_exact(double x);              /* fx() found an exact form */
const char *fcoef(double c, const char *term, int first); /* "+ 3x" style term */
const char *fpoly(const double *c, int deg, const char *var); /* c[0..deg] ascending */
const char *sup(int k);              /* superscript digits */
long lgcd(long a, long b);
int ratapprox(double x, long maxq, long *p, long *q);
char *sbuf(void);                    /* rotating scratch buffer, 96 bytes */
#define SBUF 200

/* ---------- expressions ---------- */
typedef struct node node;
node *ex_parse(const char *s, const char **err);
void ex_reset(void);
void ex_setvar(char v, cx val);
cx ex_eval(node *n);
double ex_evalr(node *n, char var, double x);
const char *ex_src(node *n);         /* original text */
node *ex_diff(node *n, char var);    /* symbolic derivative (simplified) */
const char *ex_str(node *n);         /* pretty print */
node *ex_simplify(node *n);
node *ex_num(double v);
node *ex_bin(char op, node *a, node *b);
int ex_const(node *n);
/* power series in one variable about 0 */
#define SER_N 10
typedef struct { double c[SER_N + 1]; } ser;
int ex_series(node *n, char var, double a, ser *out, const char **err);

/* ---------- numeric tools ---------- */
double integrate(node *f, char var, double a, double b, int *ok);
double nderiv(node *f, char var, double x);
int nsolve(node *f, char var, double lo, double hi, double *roots, int maxr);
int nsolve_fn(double (*f)(double), double lo, double hi, double *roots, int maxr);
void in_vecfn(const char *label, node **v, int n, char var);
int lsolve(double *A, double *b, int n); /* Gaussian elimination, A n*n row-major; b gets x */

/* ---------- linear algebra ---------- */
double det2(const double m[2][2]);
double det3(const double m[3][3]);
int inv3(const double m[3][3], double o[3][3]);
void matmul(const double *a, const double *b, double *o, int n, int k, int m);
int rank_n(const double *a, int r, int c);
void cross3(const double *a, const double *b, double *o);
double dot3(const double *a, const double *b);
double norm3(const double *a);

/* ---------- polynomials ---------- */
int proots(const cx *a, int deg, cx *roots);          /* a[0..deg] ascending */
int proots_r(const double *a, int deg, cx *roots);
void clean_roots(cx *r, int n);

/* ---------- UI (platform independent) ---------- */
typedef struct { const char *name; void (*run)(void); } calc_t;
typedef struct { const char *name; const calc_t *calcs; int n; } topic_t;
typedef struct { const char *name; const topic_t *topics; int n; } unit_t;

void app_run(const char *appname, const unit_t *units, int nunits);
void form_begin(const char *title);
double in_real(const char *label);
double in_pos(const char *label);    /* > 0 */
double in_maybe(const char *label, int *known); /* x = unknown */
int in_int(const char *label, int lo, int hi);
cx in_cx(const char *label);
node *in_fn(const char *label, char var);
int in_choice(const char *label, const char **opts, int n);
void in_vec(const char *label, double *v, int n);
void in_mat(const char *label, double *m, int r, int c);
void in_note(const char *text);      /* show a hint line in the form */
void fail(const char *msg);
void ui_set_fkeys(const char *(*lab)[6], const char *(*ins)[5]);
const char *in_text(const char *label, const char *hint);
const char *in_text_err(const char *label, const char *hint, const char *(*check)(const char *));          /* abort calculation with message */

#define NEL(a) ((int)(sizeof(a) / sizeof((a)[0])))
#endif
