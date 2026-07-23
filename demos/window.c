// Example of window creation
// Nothing more than a simple window configuration, creation and rendering

#include "../include/ansui.h"

#include <unistd.h>

int main() {
  // Initializing ANSUI with no flag being used
  // It is important to initialize since ansuiRender() needs info gathered on
  // it;
  ansuiInit(ANSUI_FLAG_DEBUG);

  // For each window, we need a config struct
  ANSUI_CONFIG *cfg = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
  cfg->w = 100;
  cfg->h = 25;
  cfg->c = ' ';
  cfg->bg_color = BG_BRED;

  // With config already created, we pass it out to ansuiCreateWindow();
  ANSUI_WIN *win = ansuiCreateWindow(
      cfg, (ANSUI_WIN_FLAG_AUTO_RESIZE | ANSUI_WIN_FLAG_POS_CENTERED));

  ansuiClear(BG_RESET);
  ansuiRender(win);

  // Quitting ANSUI reassures all memory allocated throughout are free'd
  ansuiQuit();
}
