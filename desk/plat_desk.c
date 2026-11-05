/* desktop platform: renders to framebuffer, keys from argv script, snapshots to PNG via PPM */
#include "../common/plat.h"
#include "../common/core.h"
#include "font_desk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *p_ins;
int p_digit = -1;
char p_varname[8] = "x";

static unsigned short fb[SCR_H][SCR_W];
static char **keys;
static int nkeys, ki;
static char insbuf[256];

void desk_set_keys(char **k, int n) { keys = k; nkeys = n; }

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
void p_update(void) {}

static void snap(const char *name) {
    char fn[256];
    snprintf(fn, sizeof fn, "%s.ppm", name);
    FILE *f = fopen(fn, "wb");
    fprintf(f, "P6 %d %d 255\n", SCR_W, SCR_H);
    for (int y = 0; y < SCR_H; y++)
        for (int x = 0; x < SCR_W; x++) {
            unsigned v = fb[y][x];
            unsigned char rgb[3] = {(v >> 11) * 255 / 31, ((v >> 5) & 63) * 255 / 63, (v & 31) * 255 / 31};
            fwrite(rgb, 1, 3, f);
        }
    fclose(f);
}

static void expand(const char *s, char *o) {
    static const struct { const char *k, *v; } m[] = {
        {"{pi}", S_PI}, {"{sqrt}", S_SQRT "("}, {"{th}", S_TH}, {"{sq}", S_SQ}, {"{cu}", S_CU},
        {"{inf}", S_INF}, {"{x}", S_TIMES}, {"{sp}", " "}};
    while (*s) {
        int hit = 0;
        for (unsigned i = 0; i < sizeof m / sizeof m[0]; i++) {
            int L = strlen(m[i].k);
            if (strncmp(s, m[i].k, L) == 0) { strcpy(o, m[i].v); o += strlen(m[i].v); s += L; hit = 1; break; }
        }
        if (!hit) *o++ = *s++;
    }
    *o = 0;
}

int p_key(void) {
    p_digit = -1;
    for (;;) {
        if (ki >= nkeys) exit(0);
        const char *k = keys[ki++];
        if (strncmp(k, "SNAP:", 5) == 0) { snap(k + 5); continue; }
        if (strncmp(k, "DUMP", 4) == 0) {
            for (int i = 0; i < out_count(); i++) printf("%s\n", out_get(i));
            continue;
        }
        static const char *names[] = {"EXE", "EXIT", "DEL", "AC", "UP", "DOWN", "LEFT", "RIGHT",
                                      "F1", "F2", "F3", "F4", "F5", "F6", "MENU", "VAR"};
        for (int i = 0; i < 16; i++) if (strcmp(k, names[i]) == 0) return K_EXE + i;
        if (k[0] == ':') { expand(k + 1, insbuf); p_ins = insbuf; return K_INS; }
        expand(k, insbuf);
        p_ins = insbuf;
        if (insbuf[0] >= '0' && insbuf[0] <= '9' && !insbuf[1]) p_digit = insbuf[0] - '0';
        return K_INS;
    }
}
