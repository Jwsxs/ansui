#include "../include/tgui.h"
#include <limits.h>

TGUI_PIXEL_COLOR TGUI_WIN_INITIAL_COLOR = TGUI_PIXEL_COLOR_BG_BWHITE;

int main() {
	tguiInit(TGUI_FLAG_NONE);

	TGUI_WIN* win;
	win = tguiCreateWindow(20, 2, TGUI_WIN_BLANK);
	
	tguiClear();
	
	int i = 0;
	while (i < 100000) {
		i++;
		tguiRender(win);
	}

	tguiClear();
	return 1;
}
