/* calculator platform layer (gint) */
#include "plat.h"
#include "core.h"
#include <gint/display.h>
#include <gint/keyboard.h>
#include <string.h>

extern font_t calcfont;

const char *p_ins;
int p_digit = -1;
char p_varname[8] = "x";

void p_init(void) { dfont(&calcfont); }
void p_clear(void) { dclear(C_WHITE); }
void p_rect(int x1, int y1, int x2, int y2, int col) { drect(x1, y1, x2, y2, col); }
static void strip_marks(const char *s, char *o, int cap) {
    int n = 0;
    for (; *s && n < cap - 1; s++) if (*s != 9 && *s != 13) o[n++] = *s;
    o[n] = 0;
}
void p_text(int x, int y, const char *s, int col) { char b[300]; strip_marks(s, b, 300); dtext(x, y, col, b); }
int p_textw(const char *s) { char b[300]; int w, h; strip_marks(s, b, 300); if (!b[0]) return 0; dsize(b, NULL, &w, &h); return w; }
int p_fonth(void) { return 9; }
void p_update(void) { dupdate(); }

static const char *alpha_char(int k) {
    switch (k) {
    case KEY_XOT: return "a"; case KEY_LOG: return "b"; case KEY_LN: return "c";
    case KEY_SIN: return "d"; case KEY_COS: return "e"; case KEY_TAN: return "f";
    case KEY_FRAC: return "g"; case KEY_FD: return "h"; case KEY_LEFTP: return "i";
    case KEY_RIGHTP: return "j"; case KEY_COMMA: return "k"; case KEY_ARROW: return "l";
    case KEY_7: return "m"; case KEY_8: return "n"; case KEY_9: return "o";
    case KEY_4: return "p"; case KEY_5: return "q"; case KEY_6: return "r";
    case KEY_MUL: return "s"; case KEY_DIV: return "t"; case KEY_1: return "u";
    case KEY_2: return "v"; case KEY_3: return "w"; case KEY_PLUS: return "x";
    case KEY_MINUS: return "y"; case KEY_0: return "z"; case KEY_DOT: return " ";
    case KEY_SQUARE: return S_TH; case KEY_POWER: return S_LA;
    }
    return NULL;
}

int p_key(void) {
    for (;;) {
        p_digit = -1;
        key_event_t e = getkey();
        int k = e.key;
        switch (k) {
        case KEY_EXE: return K_EXE;
        case KEY_EXIT: return K_EXIT;
        case KEY_DEL: return K_DEL;
        case KEY_ACON: return K_AC;
        case KEY_UP: return K_UP;
        case KEY_DOWN: return K_DOWN;
        case KEY_LEFT: return K_LEFT;
        case KEY_RIGHT: return K_RIGHT;
        case KEY_F1: return K_F1;
        case KEY_F2: return K_F2;
        case KEY_F3: return K_F3;
        case KEY_F4: return K_F4;
        case KEY_F5: return K_F5;
        case KEY_F6: return K_F6;
        }
        if (e.alpha) {
            const char *c = alpha_char(k);
            if (c) { p_ins = c; return K_INS; }
            continue;
        }
        if (k == KEY_XOT) return K_VAR;
        static const int digits[10] = {KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9};
        static const char *dstr[10] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
        for (int i = 0; i < 10; i++)
            if (k == digits[i]) {
                if (e.shift && i == 0) { p_ins = "i"; return K_INS; }
                p_digit = i;
                p_ins = dstr[i];
                return K_INS;
            }
        const char *s = NULL;
        if (e.shift) {
            switch (k) {
            case KEY_SQUARE: s = S_SQRT "("; break;
            case KEY_POWER: s = "^(1/"; break;
            case KEY_LOG: s = "10^("; break;
            case KEY_LN: s = "e^("; break;
            case KEY_SIN: s = "arcsin("; break;
            case KEY_COS: s = "arccos("; break;
            case KEY_TAN: s = "arctan("; break;
            case KEY_EXP: s = S_PI; break;
            case KEY_COMMA: s = ","; break;
            }
        } else {
            switch (k) {
            case KEY_DOT: s = "."; break;
            case KEY_EXP: s = "E"; break;
            case KEY_NEG: s = "-"; break;
            case KEY_PLUS: s = "+"; break;
            case KEY_MINUS: s = "-"; break;
            case KEY_MUL: s = S_TIMES; break;
            case KEY_DIV: s = "/"; break;
            case KEY_FRAC: s = "/"; break;
            case KEY_LEFTP: s = "("; break;
            case KEY_RIGHTP: s = ")"; break;
            case KEY_COMMA: s = ","; break;
            case KEY_POWER: s = "^"; break;
            case KEY_SQUARE: s = S_SQ; break;
            case KEY_LOG: s = "log("; break;
            case KEY_LN: s = "ln("; break;
            case KEY_SIN: s = "sin("; break;
            case KEY_COS: s = "cos("; break;
            case KEY_TAN: s = "tan("; break;
            }
        }
        if (s) { p_ins = s; return K_INS; }
    }
}
