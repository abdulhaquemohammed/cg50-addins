#!/bin/sh
# Builds the browser versions with Emscripten (emcc) into web/*.js
set -e
cd "$(dirname "$0")/.."
COMMON="common/num.c common/expr.c common/la.c common/ui.c common/util.c common/stats.c"
FLAGS="-O2 -sASYNCIFY -sASYNCIFY_STACK_SIZE=65536 -sSTACK_SIZE=1048576 -sMODULARIZE -sSINGLE_FILE -sSINGLE_FILE_BINARY_ENCODE=0 -sENVIRONMENT=web -sALLOW_MEMORY_GROWTH -sEXPORTED_FUNCTIONS=_main,_web_push,_web_var -sEXPORTED_RUNTIME_METHODS=ccall,HEAPU16"
emcc $FLAGS -sEXPORT_NAME=FMApp -o web/fm.js $COMMON fm/*.c web/plat_web.c
emcc $FLAGS -sEXPORT_NAME=MathsApp -o web/maths.js $COMMON fm/u1.c fm/u2.c fm/u3.c fm/u4.c fm/u6.c maths/*.c web/plat_web.c
emcc $FLAGS -sEXPORT_NAME=CSApp -o web/cs.js $COMMON cs/notes.c cs/boolean.c web/plat_web.c
