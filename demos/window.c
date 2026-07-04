// Example of window creation

#include "../include/ansui.h"

#include <stdio.h>
#include <unistd.h>

int main() {
	ansuiInit(ANSUI_FLAG_NONE);

	ANSUI_WIN_CONFIG* cfg = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	cfg->w = 500;
	cfg->h = 25;
	cfg->c = ' ';
	cfg->char_color = CH_BBLUE;
	cfg->bg_color = BG_BRED;

	ANSUI_WIN* win = ansuiCreateWindow(cfg, ANSUI_WIN_FLAG_NONE);
	printf("aa");
	

	ansuiClear(BG_RESET);

	ansuiRender(win);

	ansuiQuit();
}
