#include <stdio.h>

#include "tgui.h"

uint8_t tguiInit(TGUI_FLAG flag) {
	switch (flag) {
		case TGUI_FLAG_NONE:
			break;
	}
	return TGUI_SUCCESS;
}

// ===

TGUI_WIN* tguiCreateWindow(uint8_t width, uint8_t height, TGUI_WIN_FLAG flag) {
	TGUI_WIN* win = malloc(sizeof(TGUI_WIN));
	win->width = width;
	win->height = height;

	TGUI_PIXEL_ARRAY* pxa = malloc(sizeof(TGUI_PIXEL) * width * height);

	switch (flag) {
		default:
		case TGUI_WIN_BLANK:
			tguiFillPixelArray(&pxa, ' ');
		break;
	}
	
	win->pxa = pxa;
	return win;
}

static void tguiFillPixelArray(TGUI_PIXEL_ARRAY** pxa, char c) {
}

// ===

int tguiRender(TGUI_WIN* win) {
	int i = 0;
	for (int h = 0; h < win->height; h++) {
		for (int w = 0; w < win->width; w++) {
			printf("%c", 'a');
			//printf("%s%c", px->color, 'b');
			i++;
		}
		//printf("b");
		printf("\n");
	}
	printf("%s\033[1H", TGUI_WIN_COLOR_CLEAR);
	return TGUI_SUCCESS;
}

void tguiClear() {
	printf("\033[2J"); // framebuffer size supports current a fixed size
}
