# Desktop (PC) builds used for testing and screenshots.
# Calculator builds: see cg/*/ (needs fxSDK + gint).
CC ?= gcc
CFLAGS ?= -O1 -Wall -Wno-unused-function
COMMON = common/num.c common/expr.c common/la.c common/ui.c common/util.c common/stats.c
FMSRC = fm/u1.c fm/u2.c fm/u3.c fm/u4.c fm/u6.c
DESK = desk/plat_desk.c desk/main_desk.c

all: desk/fm desk/maths desk/cs

desk/fm: $(COMMON) $(FMSRC) fm/app.c $(DESK)
	$(CC) $(CFLAGS) -o $@ $(COMMON) $(FMSRC) fm/app.c $(DESK) -lm

desk/maths: $(COMMON) $(FMSRC) maths/*.c $(DESK)
	$(CC) $(CFLAGS) -o $@ $(COMMON) $(FMSRC) maths/*.c $(DESK) -lm

cs/notes.c: cs/notes1.txt cs/notes2.txt cs/notes3.txt cs/notes4.txt cs/gen.py
	cd cs && python3 gen.py

desk/cs: $(COMMON) cs/notes.c cs/boolean.c $(DESK)
	$(CC) $(CFLAGS) -o $@ $(COMMON) cs/notes.c cs/boolean.c $(DESK) -lm

test: desk/cs
	cd cs/test && python3 run.py

clean:
	rm -f desk/fm desk/maths desk/cs

.PHONY: all test clean
