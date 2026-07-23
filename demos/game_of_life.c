// Example of a Game of Life creation
#include "../include/ansui.h"

#include <stdint.h>

void set_term_grid(struct winsize *ws, double size) {
  int HEIGHT = (double)ws->ws_col / size;
  int WIDTH = (double)ws->ws_row / size;

  for (int8_t h = 0; h < HEIGHT; h++) {
    for (int8_t w = 0; w < WIDTH; w++) {
      ansuiDrawLine({w, h - size / 2}, {w, h + size / 2});
    }
  }
}

int main() {
  struct winsize *ws = ansuiInit(ANSUI_FLAG_NONE);
  set_term_grid(ws, 5);

  ansuiQuit();
}
