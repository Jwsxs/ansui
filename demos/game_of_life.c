#include "../include/ansui.h"

void set_term_grid(struct winsize* ws, int8_t size) {
    int h = ws->ws_col;
}

int main() {
    struct winsize* ws = ansuiInit(ANSUI_FLAG_NONE);
    set_term_grid(ws, 5);

    ansuiQuit();
}