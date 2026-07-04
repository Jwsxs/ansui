// Example of window creation

#include "../include/ansui.h"

#include <stdio.h>
#include <unistd.h>

int main() {
	ansuiInit(ANSUI_FLAG_NONE);

	ANSUI_WIN_CONFIG* cfg1 = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	cfg1->w = 500;
	cfg1->h = 25;
	cfg1->c = ' ';
	cfg1->char_color = CH_BBLUE;
	cfg1->bg_color = BG_BGREEN;

	ANSUI_WIN* win1 = ansuiCreateWindow(cfg1, ANSUI_WIN_FLAG_NONE);

	ANSUI_WIN_CONFIG* cfg2 = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	cfg2->x = 50;
	cfg2->y = 10;
	cfg2->w = 20;
	cfg2->h = 10;

	ANSUI_WIN* win2 = ansuiCreateWindow(cfg2, ANSUI_WIN_FLAG_NONE);	

	ansuiClear(BG_RESET);

	for (int frame = 0; frame < 20; frame++) {
		ansuiClear(BG_RESET);
		cfg2->x--;
		ansuiRender(win1);
		ansuiRender(win2);

		usleep(200000);
	}
	ansuiQuit();
}
