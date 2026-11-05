/* menus, input forms, output viewer */
#include "core.h"
#include "plat.h"
#include <setjmp.h>
#include <string.h>

#define TITLE_H 18
#define FKEY_H 15
#define LINE_H 14

static jmp_buf jb;
static const char *cur_title;

/* ---------------- drawing helpers ---------------- */
static void title_bar(const char *t) {
    p_rect(0, 0, SCR_W - 1, TITLE_H - 1, COL_TITLE);
    p_text(4, (TITLE_H - p_fonth()) / 2, t, COL_WHITE);
}

static void fkey_bar(const char **labels) {
    int y = SCR_H - FKEY_H;
    int w = SCR_W / 6;
    for (int i = 0; i < 6; i++) {
        if (!labels[i] || !labels[i][0]) continue;
        p_rect(i * w + 1, y, i * w + w - 2, SCR_H - 1, COL_FKEY);
        int tw = p_textw(labels[i]);
        p_text(i * w + (w - tw) / 2, y + (FKEY_H - p_fonth()) / 2, labels[i], COL_WHITE);
    }
}

/* draw text clipped to width with ellipsis */
static void text_clip(int x, int y, const char *s, int maxw, int col) {
    char b[256];
    strncpy(b, s, 255);
    b[255] = 0;
    if (p_textw(b) <= maxw) { p_text(x, y, b, col); return; }
    int L = strlen(b);
    while (L > 0) {
        L--;
        while (L > 0 && ((unsigned char)b[L] & 0xC0) == 0x80) L--;
        b[L] = 0;
        char t[260];
        strcpy(t, b);
        strcat(t, "..");
        if (p_textw(t) <= maxw) { p_text(x, y, t, col); return; }
    }
}

/* ---------------- menu ---------------- */
static int menu(const char *title, const char **items, int n, int *sel_io) {
    int sel = *sel_io, top = 0;
    int rows = (SCR_H - TITLE_H - FKEY_H - 4) / (LINE_H + 2);
    static const char *fk[6] = {"", "", "", "", "", ""};
    for (;;) {
        if (sel < top) top = sel;
        if (sel >= top + rows) top = sel - rows + 1;
        p_clear();
        title_bar(title);
        for (int i = 0; i < rows && top + i < n; i++) {
            int k = top + i;
            int y = TITLE_H + 3 + i * (LINE_H + 2);
            if (k == sel) p_rect(0, y - 1, SCR_W - 1, y + LINE_H, COL_SEL);
            char lab[200];
            char num[4];
            if (k < 9) { num[0] = '1' + k; num[1] = 0; }
            else if (k == 9) strcpy(num, "0");
            else { num[0] = 'A' + k - 10; num[1] = 0; }
            strcpy(lab, num);
            strcat(lab, ": ");
            strncat(lab, items[k], 190);
            text_clip(6, y + (LINE_H - p_fonth()) / 2, lab, SCR_W - 16, COL_BLACK);
        }
        if (n > rows) {
            /* scrollbar */
            int h = SCR_H - TITLE_H - FKEY_H - 4;
            int bh = h * rows / n, by = TITLE_H + 2 + h * top / n;
            p_rect(SCR_W - 4, by, SCR_W - 2, by + bh, COL_GREY);
        }
        fk[0] = "";
        fkey_bar(fk);
        p_text(4, SCR_H - FKEY_H + 2, "EXE:select  EXIT:back", COL_GREY);
        p_update();
        int k = p_key();
        if (k == K_UP) sel = (sel + n - 1) % n;
        else if (k == K_DOWN) sel = (sel + 1) % n;
        else if (k == K_EXE || k == K_RIGHT) { *sel_io = sel; return sel; }
        else if (k == K_EXIT || k == K_LEFT) { *sel_io = sel; return -1; }
        else if (k == K_INS && p_digit >= 0) {
            int d = p_digit == 0 ? 9 : p_digit - 1;
            if (d < n) { *sel_io = d; return d; }
        }
    }
}

/* ---------------- output viewer ---------------- */
#define WR_MAX 1200
static short wr_line[WR_MAX], wr_off[WR_MAX], wr_len[WR_MAX];
static int wr_n;

static int line_kind(const char *s) {
    if (s[0] == '\x01') return 1;
    if (s[0] == '\x02') return 2;
    if (s[0] == '-' && s[1] == '-' && s[2] == 0) return 3;
    return 0;
}

/* draw text with overbars: 0x09 starts a bar, 0x0D ends it (nestable) */
static void draw_rich(int x, int y, const char *s, int depth, int col) {
    char one[2] = {0, 0};
    for (; *s; s++) {
        if (*s == 9) { depth++; continue; }
        if (*s == 13) { if (depth) depth--; continue; }
        one[0] = *s;
        int w = p_textw(one);
        p_text(x, y, one, col);
        for (int k = 1; k <= depth; k++) p_rect(x, y - 2 * k, x + w, y - 2 * k, col);
        x += w + 1;
    }
}
static int depth_at(const char *s, int off) {
    int d = 0;
    for (int i = 0; i < off && s[i]; i++) { if (s[i] == 9) d++; else if (s[i] == 13 && d) d--; }
    return d;
}

static void wrap_all(int maxw) {
    wr_n = 0;
    for (int i = 0; i < out_count() && wr_n < WR_MAX; i++) {
        const char *s = out_get(i);
        int k = line_kind(s);
        int start = (k == 1 || k == 2) ? 1 : 0;
        int L = strlen(s);
        if (k == 3 || L == start) { wr_line[wr_n] = i; wr_off[wr_n] = 0; wr_len[wr_n] = L; wr_n++; continue; }
        int pos = start;
        int first = 1;
        while (pos < L && wr_n < WR_MAX) {
            /* find longest prefix that fits, prefer breaking at space */
            int ind = first ? 0 : 12;
            int end = pos, lastsp = -1;
            char tmp[300];
            while (end < L) {
                int nx = end + 1;
                while (nx < L && ((unsigned char)s[nx] & 0xC0) == 0x80) nx++;
                int len = nx - pos;
                if (len > 290) break;
                memcpy(tmp, s + pos, len);
                tmp[len] = 0;
                if (p_textw(tmp) > maxw - ind) break;
                if (s[end] == ' ') lastsp = end;
                end = nx;
            }
            if (end < L && lastsp > pos) end = lastsp + 1;
            if (end == pos) end = pos + 1;
            wr_line[wr_n] = i;
            wr_off[wr_n] = pos;
            wr_len[wr_n] = end - pos;
            wr_n++;
            pos = end;
            first = 0;
        }
    }
}

int ui_notes_mode;
/* returns 1 = run again / continue, 0 = back */
static int view_output_from(const char *title, int from_line, int cont) {
    int maxw = SCR_W - 12;
    wrap_all(maxw);
    int rows = (SCR_H - TITLE_H - FKEY_H - 2) / LINE_H;
    int top = 0;
    for (int w = 0; w < wr_n; w++) if (wr_line[w] >= from_line) { top = w; break; }
    if (wr_n > rows && top > wr_n - rows) top = wr_n - rows;
    if (top < 0) top = 0;
    static const char *fk1[6] = {"Top", "End", "", "", "", "Again"};
    static const char *fk2[6] = {"Top", "End", "", "", "", "Next"};
    static const char *fk3[6] = {"Top", "End", "", "", "", ""};
    const char **fk = cont ? fk2 : ui_notes_mode ? fk3 : fk1;
    for (;;) {
        p_clear();
        title_bar(title);
        for (int r = 0; r < rows && top + r < wr_n; r++) {
            int w = top + r;
            const char *s = out_get(wr_line[w]);
            int k = line_kind(s);
            int y = TITLE_H + 2 + r * LINE_H;
            if (k == 3) { p_rect(4, y + LINE_H / 2, SCR_W - 10, y + LINE_H / 2, COL_GREY); continue; }
            char t[300];
            memcpy(t, s + wr_off[w], wr_len[w]);
            t[wr_len[w]] = 0;
            int first = (wr_off[w] <= ((k == 1 || k == 2) ? 1 : 0));
            int x = first ? 4 : 16;
            int col = k == 1 ? COL_HEAD : k == 2 ? COL_ANS : COL_BLACK;
            draw_rich(x, y + (LINE_H - p_fonth()) / 2 + 1, t, depth_at(s, wr_off[w]), col);
        }
        if (wr_n > rows) {
            int h = rows * LINE_H;
            int bh = h * rows / wr_n;
            if (bh < 6) bh = 6;
            int by = TITLE_H + 2 + (h - bh) * top / (wr_n - rows);
            p_rect(SCR_W - 4, TITLE_H + 2, SCR_W - 2, TITLE_H + 2 + h, COL_LGREY);
            p_rect(SCR_W - 4, by, SCR_W - 2, by + bh, COL_GREY);
        }
        fkey_bar(fk);
        p_update();
        int k = p_key();
        int maxtop = wr_n > rows ? wr_n - rows : 0;
        if (k == K_DOWN) { if (top < maxtop) top++; }
        else if (k == K_UP) { if (top > 0) top--; }
        else if (k == K_RIGHT) { top += rows - 1; if (top > maxtop) top = maxtop; }
        else if (k == K_LEFT) { top -= rows - 1; if (top < 0) top = 0; }
        else if (k == K_F1) top = 0;
        else if (k == K_F2) top = maxtop;
        else if ((k == K_F6 || k == K_EXE) && !ui_notes_mode) return 1;
        else if (k == K_EXIT || (k == K_EXE && ui_notes_mode)) return 0;
    }
}
static int view_output(const char *title) { return view_output_from(title, 0, 0); }

/* show results produced so far before asking a follow-up question */
static int f_seen;
static const char *(*fk_lab)[6];
static const char *(*fk_ins)[5];
static void flush_output(void) {
    if (out_count() > f_seen) {
        int from = f_seen;
        f_seen = out_count();
        if (!view_output_from(cur_title, from, 1)) longjmp(jb, 1);
    }
}

/* ---------------- forms ---------------- */
#define FMAX 24
static char f_lab[FMAX][48], f_val[FMAX][64];
static int f_n;
static char f_note[120];

void form_begin(const char *title) {
    cur_title = title;
    f_seen = 0;
    fk_lab = NULL; fk_ins = NULL;
    f_n = 0;
    f_note[0] = 0;
    ex_reset();
}

void in_note(const char *text) { strncpy(f_note, text, 119); f_note[119] = 0; }

static void form_push(const char *lab, const char *val) {
    if (f_n >= FMAX) { for (int i = 1; i < FMAX; i++) { strcpy(f_lab[i - 1], f_lab[i]); strcpy(f_val[i - 1], f_val[i]); } f_n--; }
    strncpy(f_lab[f_n], lab, 47); f_lab[f_n][47] = 0;
    strncpy(f_val[f_n], val, 63); f_val[f_n][63] = 0;
    f_n++;
}

void ui_set_fkeys(const char *(*lab)[6], const char *(*ins)[5]) { fk_lab = lab; fk_ins = ins; }
static const char *fpages[3][6] = {
    {"e", "sinh", "cosh", "tanh", "abs", ">>"},
    {"arsinh", "arcosh", "artanh", "sec", "cosec", ">>"},
    {"cot", "arg", "conj", S_INF, "!", ">>"},
};
static const char *fins[3][5] = {
    {"e", "sinh(", "cosh(", "tanh(", "abs("},
    {"arsinh(", "arcosh(", "artanh(", "sec(", "cosec("},
    {"cot(", "arg(", "conj(", S_INF, "!"},
};

/* edit a line of text; returns 1 on EXE, longjmps on EXIT */
static void flush_output(void);
static void edit_line(const char *label, char *buf, int cap, const char *err, const char *hint) {
    flush_output();
    int cur = strlen(buf);
    int page = 0;
    for (;;) {
        p_clear();
        title_bar(cur_title);
        int y = TITLE_H + 2;
        int avail = SCR_H - FKEY_H - 3 * LINE_H - 6;
        int shown = avail / LINE_H;
        if (f_note[0]) shown--;
        int start = f_n > shown ? f_n - shown : 0;
        if (f_note[0]) { text_clip(4, y, f_note, SCR_W - 8, COL_HEAD); y += LINE_H; }
        for (int i = start; i < f_n; i++) {
            char t[140];
            strcpy(t, f_lab[i]);
            if (f_val[i][0] == '\x01') { strcat(t, ": "); strcat(t, f_val[i] + 1); }
            else { int L = strlen(t); strcat(t, (L && t[L - 1] == '=') ? " " : " = "); strcat(t, f_val[i]); }
            text_clip(4, y, t, SCR_W - 8, COL_GREY);
            y += LINE_H;
        }
        /* current prompt */
        int py = SCR_H - FKEY_H - 3 * LINE_H - 4;
        text_clip(4, py, label, SCR_W - 8, COL_BLACK);
        int by = py + LINE_H;
        p_rect(2, by - 1, SCR_W - 3, by + LINE_H + 1, COL_BLACK);
        p_rect(3, by, SCR_W - 4, by + LINE_H, COL_WHITE);
        /* horizontal scroll so cursor is visible */
        int off = 0;
        char pre[260];
        for (;;) {
            memcpy(pre, buf + off, cur - off);
            pre[cur - off] = 0;
            if (p_textw(pre) < SCR_W - 16) break;
            off++;
            while (off < cur && ((unsigned char)buf[off] & 0xC0) == 0x80) off++;
        }
        char vis[260];
        strcpy(vis, buf + off);
        text_clip(6, by + (LINE_H - p_fonth()) / 2 + 1, vis, SCR_W - 12, COL_BLACK);
        int cx_ = 6 + p_textw(pre);
        p_rect(cx_, by + 2, cx_ + 1, by + LINE_H - 2, COL_ANS);
        if (err) text_clip(4, by + LINE_H + 3, err, SCR_W - 8, COL_ANS);
        else if (hint) text_clip(4, by + LINE_H + 3, hint, SCR_W - 8, COL_GREY);
        fkey_bar(fk_lab ? fk_lab[page] : fpages[page]);
        p_update();

        int k = p_key();
        const char *ins = NULL;
        if (k == K_EXE) { if (buf[0]) return; continue; }
        if (k == K_EXIT) longjmp(jb, 1);
        if (k == K_AC) { buf[0] = 0; cur = 0; err = NULL; continue; }
        if (k == K_DEL) {
            if (cur > 0) {
                int p = cur - 1;
                while (p > 0 && ((unsigned char)buf[p] & 0xC0) == 0x80) p--;
                memmove(buf + p, buf + cur, strlen(buf + cur) + 1);
                cur = p;
            }
            err = NULL;
            continue;
        }
        if (k == K_LEFT) { if (cur > 0) { cur--; while (cur > 0 && ((unsigned char)buf[cur] & 0xC0) == 0x80) cur--; } continue; }
        if (k == K_RIGHT) { if (buf[cur]) { cur++; while (buf[cur] && ((unsigned char)buf[cur] & 0xC0) == 0x80) cur++; } continue; }
        if (k == K_UP) { cur = 0; continue; }
        if (k == K_DOWN) { cur = strlen(buf); continue; }
        if (k == K_F6) { page = (page + 1) % 3; continue; }
        if (k >= K_F1 && k <= K_F5) ins = fk_ins ? fk_ins[page][k - K_F1] : fins[page][k - K_F1];
        if (k == K_VAR) ins = p_varname;
        if (k == K_INS) ins = p_ins;
        if (ins) {
            int L = strlen(ins);
            if ((int)strlen(buf) + L < cap - 1) {
                memmove(buf + cur + L, buf + cur, strlen(buf + cur) + 1);
                memcpy(buf + cur, ins, L);
                cur += L;
            }
            err = NULL;
        }
    }
}

static void set_var(char v) {
    if (v == 'T') strcpy(p_varname, S_TH);
    else if (v) { p_varname[0] = v; p_varname[1] = 0; }
    else strcpy(p_varname, "x");
}

static cx eval_text(const char *label, char *buf, const char **err) {
    node *n = ex_parse(buf, err);
    if (!n) return R(NAN);
    return ex_eval(n);
}

static cx ask_cx(const char *label, int real, int positive) {
    char buf[200] = "";
    const char *err = NULL;
    set_var(0);
    for (;;) {
        edit_line(label, buf, sizeof buf, err, NULL);
        cx v = eval_text(label, buf, &err);
        if (err) continue;
        if (v.re != v.re || v.im != v.im) { err = "Math error"; continue; }
        if (real && fabs(v.im) > 1e-12 * (1 + fabs(v.re))) { err = "Must be a real number"; continue; }
        if (real) v.im = 0;
        v.re += 0.0; v.im += 0.0;
        if (v.im == 0) v.im = 0;
        if (positive && !(v.re > 0)) { err = "Must be > 0"; continue; }
        char shown[64];
        strncpy(shown, buf, 40);
        shown[40] = 0;
        const char *val = real ? fx(v.re) : fc(v);
        if (strcmp(shown, val) != 0 && strlen(shown) + strlen(val) < 56) { strcat(shown, " = "); strcat(shown, val); }
        form_push(label, shown);
        return v;
    }
}

double in_real(const char *label) { return ask_cx(label, 1, 0).re; }

/* real value or unknown (user types x via the X,θ,T key) */
double in_maybe(const char *label, int *known) {
    char buf[200] = "";
    const char *err = NULL;
    set_var(0);
    for (;;) {
        edit_line(label, buf, sizeof buf, err, "X,\x05,T key (x) = unknown");
        if (strcmp(buf, "x") == 0) { *known = 0; form_push(label, "?"); return NAN; }
        cx v = eval_text(label, buf, &err);
        if (err) continue;
        if (v.re != v.re || fabs(v.im) > 1e-12) { err = "Must be a real number"; continue; }
        *known = 1;
        form_push(label, fx(v.re));
        return v.re;
    }
}
double in_pos(const char *label) { return ask_cx(label, 1, 1).re; }
cx in_cx(const char *label) { return ask_cx(label, 0, 0); }

int in_int(const char *label, int lo, int hi) {
    char buf[64] = "";
    const char *err = NULL;
    set_var(0);
    for (;;) {
        edit_line(label, buf, sizeof buf, err, NULL);
        cx v = eval_text(label, buf, &err);
        if (err) continue;
        if (v.im != 0 || v.re != floor(v.re)) { err = "Must be a whole number"; continue; }
        if (v.re < lo || v.re > hi) {
            static char e[64];
            strcpy(e, "Must be from ");
            strcat(e, fd(lo));
            strcat(e, " to ");
            strcat(e, fd(hi));
            err = e;
            continue;
        }
        form_push(label, buf);
        return (int)v.re;
    }
}

node *in_fn(const char *label, char var) {
    static char bufs[6][200];
    static int bi;
    bi = (bi + 1) % 6;
    char *buf = bufs[bi];
    buf[0] = 0;
    const char *err = NULL;
    set_var(var);
    char hint[64];
    strcpy(hint, "X,");
    strcat(hint, S_TH);
    strcat(hint, ",T key types ");
    strcat(hint, p_varname);
    for (;;) {
        edit_line(label, buf, 200, err, hint);
        node *n = ex_parse(buf, &err);
        if (!n) continue;
        /* test evaluation */
        form_push(label, buf);
        return n;
    }
}

void in_vec(const char *label, double *v, int n) {
    char buf[200] = "";
    const char *err = NULL;
    set_var(0);
    char hint[48];
    strcpy(hint, "Enter ");
    strcat(hint, fd(n));
    strcat(hint, " values separated by commas");
    for (;;) {
        edit_line(label, buf, sizeof buf, err, hint);
        /* split on commas at depth 0 */
        char tmp[200];
        strcpy(tmp, buf);
        int cnt = 0, depth = 0;
        char *st = tmp;
        err = NULL;
        for (char *p = tmp;; p++) {
            if (*p == '(') depth++;
            if (*p == ')') depth--;
            if ((*p == ',' && depth == 0) || *p == 0) {
                char end = *p;
                *p = 0;
                if (cnt >= n) { err = "Too many values"; break; }
                cx val = eval_text(label, st, &err);
                if (err) break;
                if (fabs(val.im) > 1e-12 || val.re != val.re) { err = "Real values only"; break; }
                v[cnt++] = val.re;
                st = p + 1;
                if (!end) break;
            }
        }
        if (err) continue;
        if (cnt != n) { err = "Wrong number of values"; continue; }
        char shown[64];
        strcpy(shown, "(");
        for (int i = 0; i < n; i++) {
            if (i) strcat(shown, ", ");
            if (strlen(shown) > 50) { strcat(shown, ".."); break; }
            strcat(shown, fx(v[i]));
        }
        strcat(shown, ")");
        form_push(label, shown);
        return;
    }
}

void in_vecfn(const char *label, node **v, int n, char var) {
    static char bufs[8][200];
    static int bi;
    bi = (bi + 1) % 8;
    char *buf = bufs[bi];
    buf[0] = 0;
    const char *err = NULL;
    set_var(var);
    for (;;) {
        edit_line(label, buf, 200, err, "Values separated by commas");
        char tmp[200];
        strcpy(tmp, buf);
        int cnt = 0, depth = 0;
        char *st = tmp;
        err = NULL;
        for (char *p = tmp;; p++) {
            if (*p == '(') depth++;
            if (*p == ')') depth--;
            if ((*p == ',' && depth == 0) || *p == 0) {
                char end = *p;
                *p = 0;
                if (cnt >= n) { err = "Too many values"; break; }
                node *nd = ex_parse(st, &err);
                if (err) break;
                v[cnt++] = nd;
                st = p + 1;
                if (!end) break;
            }
        }
        if (err) continue;
        if (cnt != n) { err = "Wrong number of values"; continue; }
        form_push(label, buf);
        return;
    }
}

const char *in_text(const char *label, const char *hint) {
    static char bufs[4][200];
    static int bi;
    bi = (bi + 1) % 4;
    char *buf = bufs[bi];
    buf[0] = 0;
    edit_line(label, buf, 200, NULL, hint);
    form_push(label, buf);
    return buf;
}
const char *in_text_err(const char *label, const char *hint, const char *(*check)(const char *)) {
    static char bufs[4][200];
    static int bi;
    bi = (bi + 1) % 4;
    char *buf = bufs[bi];
    buf[0] = 0;
    const char *err = NULL;
    for (;;) {
        edit_line(label, buf, 200, err, hint);
        err = check(buf);
        if (!err) break;
    }
    form_push(label, buf);
    return buf;
}

int parse_list(const char *s, double *v, int max);
int in_list(const char *label, double *v, int max) {
    static char buf[400];
    buf[0] = 0;
    const char *err = NULL;
    set_var(0);
    for (;;) {
        edit_line(label, buf, sizeof buf, err, "Values separated by commas");
        int n = parse_list(buf, v, max);
        if (n == -1) { err = "Syntax error in list"; continue; }
        if (n == -2) { err = "Too many values"; continue; }
        if (n < 1) { err = "Enter at least one value"; continue; }
        for (int i = 0; i < n; i++) if (v[i] != v[i]) { err = "Math error in list"; n = -3; break; }
        if (n < 0) continue;
        char shown[64];
        strncpy(shown, buf, 50);
        shown[50] = 0;
        if (strlen(buf) > 50) strcat(shown, "..");
        form_push(label, shown);
        return n;
    }
}

void in_mat(const char *label, double *m, int r, int c) {
    for (int i = 0; i < r; i++) {
        char lab[64];
        strcpy(lab, label);
        strcat(lab, " row ");
        strcat(lab, fd(i + 1));
        in_vec(lab, m + i * c, c);
    }
}

int in_choice(const char *label, const char **opts, int n) {
    flush_output();
    int sel = 0;
    int r = menu(label, opts, n, &sel);
    if (r < 0) longjmp(jb, 1);
    char v[64] = "\x01";
    strncat(v, opts[r], 60);
    form_push(label, v);
    return r;
}

void fail(const char *msg) {
    outf("\x02%s", msg);
    longjmp(jb, 2);
}

/* ---------------- app ---------------- */
void app_run(const char *appname, const unit_t *units, int nunits) {
    p_init();
    const char *un[16];
    for (int i = 0; i < nunits; i++) un[i] = units[i].name;
    int su = 0;
    for (;;) {
        int u = menu(appname, un, nunits, &su);
        if (u < 0) { su = 0; continue; }
        const unit_t *U = &units[u];
        const char *tn[32];
        for (int i = 0; i < U->n; i++) tn[i] = U->topics[i].name;
        int st = 0;
        for (;;) {
            int t = U->n == 1 ? 0 : menu(U->name, tn, U->n, &st);
            if (t < 0) break;
            const topic_t *T = &U->topics[t];
            const char *cn[32];
            for (int i = 0; i < T->n; i++) cn[i] = T->calcs[i].name;
            int sc = 0;
            for (;;) {
                int c;
                if (T->n == 1) c = 0;
                else c = menu(T->name, cn, T->n, &sc);
                if (c < 0) break;
                const calc_t *K = &T->calcs[c];
                int again = 1;
                while (again) {
                    form_begin(K->name);
                    out_reset();
                    int r = setjmp(jb);
                    if (r == 0) { K->run(); r = 2; }
                    if (r == 1) { again = 0; break; }
                    again = view_output_from(K->name, f_seen < out_count() ? f_seen : 0, 0);
                }
                if (T->n == 1) break;
            }
            if (U->n == 1) break;
        }
    }
}
