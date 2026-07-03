// Example of window creation

#include "../include/ansui.h"

#include <unistd.h>

int main() {
	ansuiInit(ANSUI_FLAG_NONE);

	ANSUI_CONFIG_WINDOW* w1_cfg = ansuiLoadDefaultConfig(ANSUI_LOAD_WINDOW_ATTR);
	ANSUI_WIN* w1 = ansuiCreateWindow(w1_cfg, ANSUI_WIN_POS_CENTERED);

	ansuiClear();
	// ansuiRender(w1);

	// ansuiWinDestroy(w1);
	ansuiQuit();
	return 1;
}
