// Example of window creation
#include "../include/ansui.h"

#include <unistd.h>

int main() {
	// Initializing ANSUI with no flag being used
	// It is important to initialize since ansuiRender() needs info gathered on it;
	ansuiInit(ANSUI_FLAG_NONE);

	// For each window, we need a config struct
	ANSUI_WIN_CONFIG* cfg1 = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	cfg1->w = 100;
	cfg1->h = 25;
	cfg1->c = ' ';
	cfg1->bg_color = BG_BRED;

	// With config already created, we pass it out to ansuiCreateWindow();
	ANSUI_WIN* win1 = ansuiCreateWindow(cfg1, (ANSUI_WIN_FLAG_AUTO_RESIZE | ANSUI_WIN_FLAG_POS_CENTERED));

	for (int f = 0; f < 20; f++) {
		// ansuiClear() clears the whole screen (terminal)
		ansuiClear(BG_RESET);
		ansuiRender(win1);
	}

	// Quitting ANSUI reassures all memory allocated throughout are free'd
	ansuiQuit();
	return 0;
}
