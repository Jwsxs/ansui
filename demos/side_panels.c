#include "../include/tgui.h"
#include <unistd.h>
#include <stdio.h>

int main(void) {
	tguiInit(TGUI_FLAG_NONE);
	tguiClear();

	tguiSetGlobAttr(TGUI_ATTR_CLEAR_COLOR, TGUI_PIXEL_RESET_COLOR);

	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_RRED);
	tguiSetWinAttr(TGUI_ATTR_PXA_FILL_CHAR, '#');
	TGUI_WIN* panel1 = tguiCreateWindow(20, 5, TGUI_WIN_BLANK);

	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_RBLUE);
	tguiSetWinAttr(TGUI_ATTR_PXA_FILL_CHAR, '.');
	TGUI_WIN* panel2 = tguiCreateWindow(10, 5, TGUI_WIN_FILLED);

	tguiSetWinAttr(TGUI_ATTR_PXA_COLOR, TGUI_PIXEL_COLOR_BG_BGREEN);
	TGUI_WIN* panel3 = tguiCreateWindow(4, 2, TGUI_WIN_BLANK);
	
	for (int frame = 0; frame < 10; frame++) {
		tguiClear();

		tguiRender(panel1);
		tguiRender(panel2);
		tguiRender(panel3);
		usleep(200000);
	}

	printf(TGUI_PIXEL_RESET_COLOR "\n");
	return 0;
}
