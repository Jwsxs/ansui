#include "../include/tgui.h"
#include <unistd.h>

int main() {
	// Initializing it by calling with TGUI_FLAG => #_NONE for none
	tguiInit(TGUI_FLAG_NONE);

	// TGUI_ATTR_CLEAR_COLOR set as TGUI_PIXEL_RESET_COLOR by default
	// Can be changed via global attributes - non-applicable in any windows, but on global terminal calls
	tguiSetGlobAttr(TGUI_ATTR_CLEAR_COLOR, TGUI_PIXEL_RESET_COLOR);

	// Each "window" has to have all their attributes set before window creation call
	// If #_FILL_CHAR is set, it's recommended to call window with TGUI_WIN_FILLED
	tguiSetWinAttr(TGUI_ATTR_PXA_FILL_CHAR, 'o');
	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BRED);
	TGUI_WIN* win1 = tguiCreateWindow(50, 20, TGUI_WIN_FILLED);

	// Example of a non-char bold-blue window
	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BBLUE);
	TGUI_WIN* win2 = tguiCreateWindow(25, 10, TGUI_WIN_BLANK);

	for (int frame = 0; frame < 10; frame++) {
		// Terminal render pipeline:
		// 1. Clear previous screen
		tguiClear();

		// 2. Render windows
		tguiRender(win1);
		tguiRender(win2);

		// 3. FPS set | usleep() for better visualization
		usleep(200000);
	}

	// 4. At end, clear screen and destroy (free()) everything
	tguiClear();
	return 1;
}
