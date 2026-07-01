#include "../include/tgui.h"
#include <unistd.h>

int main() {
	// Initializing it by calling with TGUI_FLAG => #_NONE for none
	// Will not be checking terminal info => check ./demos/uinfo_retrieval.c for demo of getting user's terminal width / height
	tguiInit(TGUI_FLAG_NONE);

	// TGUI_ATTR_CLEAR_COLOR set as TGUI_PIXEL_RESET_COLOR by default
	// Can be changed via global attributes - non-applicable in any windows, but on global terminal calls
	tguiSetGlobAttr(TGUI_ATTR_CLEAR_COLOR, TGUI_PIXEL_RESET_COLOR);

	// Each "window" has to have all their attributes set before window creation call
	// If #_FILL_CHAR is set, it's recommended to call window with TGUI_WIN_OPAQUE
	tguiSetWinAttr(TGUI_ATTR_PXA_FILL_CHAR, 'o');
	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BRED);
	TGUI_WIN* win1 = tguiCreateWindow(10, 10, 40, 20, TGUI_WIN_OPAQUE);

	// When tguiCreateWindow() is called, the attributes are all set to default. Check ./include/tgui.h
	// You need to set your Windows attributes again.
	// Example of a non-char bold-blue window
	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BBLUE);
	TGUI_WIN* win2 = tguiCreateWindow(0, 0, 10, 10, TGUI_WIN_OPAQUE);

	for (int frame = 0; frame < 20; frame++) {
		// Terminal render pipeline:
		// 1. Clear previous screen
		tguiClear();

		// 2. Render windows
		tguiRender(win1);
		tguiRender(win2);

		if (frame > 5) {
			tguiUpdate(win1, TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BWHITE);
		}

		if (frame == 10) {
			tguiUpdate(win1, TGUI_ATTR_WIN_IS_OPAQUE, TGUI_TRUE);
		}
		// 3. FPS set | usleep() for better visualization
		usleep(200000);
	}
	
	// You have to destroy the window before quitting TGUI
	// In this case, destroying only win1 since we're still coding
	tguiWinDestroy(win1);

	tguiSetWinAttr(TGUI_ATTR_PXA_FILL_CHAR, 'O');
	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_CH_BBLUE);
	TGUI_WIN* win3 = tguiCreateWindow(25, 25, 10, 10, TGUI_WIN_TRANSPARENT);

	for (int frame = 0; frame < 20; frame++) {
		tguiClear();
		tguiRender(win2);
		tguiRender(win3);
		
		if (frame > 5) {
			tguiUpdate(win2, TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_CH_BGREEN);
			tguiUpdate(win2, TGUI_ATTR_PXA_FILL_CHAR, 'C');
		}

		if (frame == 10) {
			tguiUpdate(win3, TGUI_ATTR_WIN_IS_OPAQUE, TGUI_TRUE);
		}

		usleep(200000);
	}

	// 4. At end, clear screen and destroy (free()) everything
	tguiClear();
	tguiWinDestroy(win2);
	tguiWinDestroy(win3);
	tguiQuit();
	return 1;
}
