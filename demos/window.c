// Example of window creation

#include "../include/tgui.h"

#include <unistd.h>

int main() {
	tguiInit(TGUI_FLAG_NONE);

	TGUI_CONFIG_WINDOW* w1_cfg = tguiLoadDefaultConfig(TGUI_LOAD_WINDOW_ATTR);
	TGUI_WIN* w1 = tguiCreateWindow(w1_cfg, TGUI_WIN_POS_CENTERED);

	tguiClear();
	// tguiRender(w1);

	// tguiWinDestroy(w1);
	tguiQuit();
	return 1;
}
