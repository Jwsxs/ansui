#include <stdarg.h>
#include <stdio.h>

#include "../include/tgui.h"

static TGUI_CONFIG glob_conf;
static TGUI_CONFIG config;

int tguiInit(TGUI_FLAG flag) {
	switch (flag) {
		case TGUI_FLAG_NONE:
			break;
	}
	return TGUI_SUCCESS;
}

// ===

TGUI_WIN* tguiCreateWindow(int x, int y, int width, int height, TGUI_WIN_FLAG flag) {
	TGUI_WIN* win = malloc(sizeof(TGUI_WIN));
	win->width = width;
	win->height = height;

	win->x = x;
	win->y = y;

	TGUI_PIXEL_ARRAY* pxa = malloc(sizeof(TGUI_PIXEL_ARRAY));
	win->pxa = pxa;

	TGUI_PIXEL* px = malloc(sizeof(TGUI_PIXEL) * width * height);
	pxa->px = px;
	pxa->size = width * height;

	win->config = config;

	switch (flag) {
		default:
		case TGUI_WIN_BLANK:
			win->config.fill_char = ' ';
			tguiFillPixelArray(win);
		break;
		case TGUI_WIN_FILLED:
			tguiFillPixelArray(win);
		break;
	}
	
	return win;
}

static void tguiFillPixelArray(TGUI_WIN *win) {
	tguiFillPixelCharArray(win, win->config.fill_char);
	tguiFillPixelColorArray(win, win->config.color);
}

static void tguiFillPixelCharArray(TGUI_WIN* win, char c) {
	for (int i = 0; i < win->pxa->size; i++) {
		win->pxa->px[i].c = win->config.fill_char;
	}
}

static void tguiFillPixelColorArray(TGUI_WIN* win, TGUI_PIXEL_COLOR color) {
	for (int i = 0; i < win->pxa->size; i++) {
		win->pxa->px[i].color = win->config.color;
	}
}

int tguiUpdate(TGUI_WIN *win, TGUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_PXA_COLOR:
			win->config.color = va_arg(args, TGUI_PIXEL_COLOR);
			tguiFillPixelColorArray(win, win->config.color);
			break;
		case TGUI_ATTR_PXA_FILL_CHAR:
			win->config.fill_char = va_arg(args, int);
			tguiFillPixelCharArray(win, win->config.fill_char);
			break;
	}

	va_end(args);
	return TGUI_SUCCESS; // was able to succesfully update, but no error handling yet
}

// ===

void tguiSetGlobAttr(TGUI_GLOB_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_CLEAR_COLOR:
			glob_conf.clear_color = va_arg(args, TGUI_PIXEL_COLOR);
			break;
	}

	va_end(args);
}

void tguiSetWinAttr(TGUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_PXA_COLOR:
			config.color = va_arg(args, TGUI_PIXEL_COLOR);
			break;
		case TGUI_ATTR_PXA_FILL_CHAR:
			config.fill_char = va_arg(args, int);
			break;

		case TGUI_ATTR_WIN_HAS_BORDER:
			config.has_border = va_arg(args, int);
			break;

	}

	va_end(args);
}

/*
int tguiUpdate(TGUI_WIN *win, TGUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_PXA_COLOR:
			win->config.color = va_arg(args, TGUI_PIXEL_COLOR);
	}
	return TGUI_SUCCESS;
}
*/
// ===

int tguiRender(TGUI_WIN* win) {
	int i = 0;

	for (int h = 0; h < win->height; h++) {
		printf("\033[%d;%dH", win->y + h, win->x); // change cursor position | same as setting x and y
		for (int w = 0; w < win->width; w++) {
			printf("%s%c", win->pxa->px[i].color, win->pxa->px[i].c);
			i++;
		}
		//printf("b");
		printf("\n");
	}
	printf("%s\033[1H", TGUI_PIXEL_RESET_COLOR);
	return TGUI_SUCCESS;
}

void tguiClear() {
	printf("%s\033[2J\033[1H", glob_conf.clear_color); // framebuffer size supports current a fixed size
}
