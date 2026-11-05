#ifndef PLAT_H
#define PLAT_H
/* platform layer: implemented by plat_gint.c (calculator) and desk/plat_desk.c (PC previews) */
#define SCR_W 396
#define SCR_H 224

#define COL_WHITE 0xFFFF
#define COL_BLACK 0x0000
#define COL_TITLE 0x1A3B   /* dark blue */
#define COL_SEL   0xC6FF   /* light blue */
#define COL_HEAD  0x0019   /* blue */
#define COL_ANS   0xB000   /* red */
#define COL_GREY  0x8410
#define COL_LGREY 0xDEFB
#define COL_FKEY  0x3A9F

enum {
    K_NONE = 0, K_INS = 1, K_EXE = 300, K_EXIT, K_DEL, K_AC, K_UP, K_DOWN, K_LEFT, K_RIGHT,
    K_F1, K_F2, K_F3, K_F4, K_F5, K_F6, K_MENU, K_VAR, K_DIGIT0 /* +n for digit menus */
};

extern const char *p_ins;   /* string to insert when key == K_INS */
extern int p_digit;         /* digit value if a plain digit was pressed, else -1 */
extern char p_varname[8];   /* what the X,θ,T key inserts */

void p_init(void);
void p_clear(void);
void p_rect(int x1, int y1, int x2, int y2, int col);
void p_text(int x, int y, const char *s, int col);
int p_textw(const char *s);
int p_fonth(void);
void p_update(void);
int p_key(void);
#endif
