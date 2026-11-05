#include "../common/plat.h"
#include "../common/core.h"
#include "font_desk.h"
#include <emscripten.h>
#include <string.h>

const char *p_ins;
int p_digit = -1;
char p_varname[8] = "x";
static unsigned short fb[SCR_H][SCR_W];

typedef struct { int code; char s[24]; } kev;
static kev q[64];
static int qh, qt;
static char insbuf[24];

EMSCRIPTEN_KEEPALIVE void web_push(int code, const char *s) {
    int n = (qt + 1) % 64;
    if (n == qh) return;
    q[qt].code = code;
    strncpy(q[qt].s, s ? s : "", 23);
    q[qt].s[23] = 0;
    qt = n;
}
EMSCRIPTEN_KEEPALIVE const char *web_var(void) { return p_varname; }

EM_JS(void, js_blit, (void *ptr, int w, int h), { if (Module.blit) Module.blit(ptr, w, h); });

void p_init(void) {}
void p_clear(void) { for (int y = 0; y < SCR_H; y++) for (int x = 0; x < SCR_W; x++) fb[y][x] = COL_WHITE; }
void p_rect(int x1, int y1, int x2, int y2, int col) {
    for (int y = y1; y <= y2; y++) for (int x = x1; x <= x2; x++)
        if (x >= 0 && y >= 0 && x < SCR_W && y < SCR_H) fb[y][x] = col;
}
int p_fonth(void) { return 9; }
int p_textw(const char *s) {
    int w = 0, n = 0;
    for (; *s; s++) { unsigned c = (unsigned char)*s; if (c == 9 || c == 13) continue; if (c > 127) c = '?'; w += FW[c] + 1; n++; }
    return n ? w - 1 : 0;
}
void p_text(int x, int y, const char *s, int col) {
    for (; *s; s++) {
        unsigned c = (unsigned char)*s;
        if (c == 9 || c == 13) continue;
        if (c > 127) c = '?';
        for (int r = 0; r < 11; r++)
            for (int b = 0; b < FW[c]; b++)
                if (FB[c][r] >> b & 1) {
                    int X = x + b, Y = y + r;
                    if (X >= 0 && Y >= 0 && X < SCR_W && Y < SCR_H) fb[Y][X] = col;
                }
        x += FW[c] + 1;
    }
}
void p_update(void) { js_blit(fb, SCR_W, SCR_H); }

int p_key(void) {
    p_digit = -1;
    while (qh == qt) emscripten_sleep(25);
    kev e = q[qh];
    qh = (qh + 1) % 64;
    if (e.code == K_INS) {
        strcpy(insbuf, e.s);
        p_ins = insbuf;
        if (insbuf[0] >= '0' && insbuf[0] <= '9' && !insbuf[1]) p_digit = insbuf[0] - '0';
    }
    return e.code;
}

void app_main(void);
int main(void) { app_main(); return 0; }
