#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// It's better to handle the initialization stuff outside this file
// Organization and can be better written sometimes
#include "../include/tgui.h"

static TGUI_CONFIG glob_conf;
static TGUI_CONFIG config;

// === TGUI_INIT_H


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

		case TGUI_ATTR_WIN_IS_OPAQUE:
			config.is_opaque = va_arg(args, int);
			break;
		case TGUI_ATTR_WIN_HAS_BORDER:
			config.has_border = va_arg(args, int);
			break;

	}

	va_end(args);
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
		case TGUI_WIN_OPAQUE: // "blankness" set from here
			win->config.is_opaque = 1;
			win->config.fill_char = ' ';
			tguiFillPixelArray(win);
		break;
		case TGUI_WIN_TRANSPARENT:
			win->config.is_opaque = 0;
			tguiFillPixelArray(win);
		break;
	}
	
	return win;
}

static int tguiFillPixelArray(TGUI_WIN *win) {
	if (win->config.is_opaque == 0) { // then it's transparent
		return 1; // return true since pixel array could be filled succesfully
	}
	tguiFillPixelCharArray(win, win->config.fill_char);
	tguiFillPixelColorArray(win, win->config.color);

	return 1; // pixel array filled succesfully
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

		case TGUI_ATTR_WIN_IS_OPAQUE:
			//win->config.fill_char = ' ';
			win->config.is_opaque = va_arg(args, int);
			if (win->config.is_opaque == 1) {
				tguiFillPixelColorArray(win, win->config.color);
				tguiFillPixelCharArray(win, win->config.fill_char);
			}
		case TGUI_ATTR_WIN_HAS_BORDER:
			// not yet implemented
			break;
		default:
			break;
	}

	va_end(args);
	return TGUI_SUCCESS; // was able to succesfully update, but no error handling yet
}

int tguiWinDestroy(TGUI_WIN *win) {
	// Every malloc on *win is:
	free(win->pxa->px);
	free(win->pxa);
	free(win);
	return TGUI_SUCCESS;
}

int tguiQuit() {
	// Free memory got from tguiInit();
	return TGUI_SUCCESS;
}

// === RENDER

int tguiRender(TGUI_WIN* win) {
	int i = 0;

	for (int h = 0; h < win->height; h++) {
		int c_pos[2] = {
			win->y + h,
			win->x
		};

		printf("\033[%d;%dH", c_pos[0], c_pos[1]); // change cursor position | same as setting x and y
		for (int w = 0; w < win->width; w++) {
			// check if win->config.is_opaque == 0 and if win->pxa->px[i] isn't inside any entity / widget
			// if not, just move cursor pos to +1
			if (win->config.is_opaque == 0) {
				printf("\033[%d;%dH", c_pos[0] + 1, c_pos[1]);
			} else {
				printf("%s%c", win->pxa->px[i].color, win->pxa->px[i].c);
			}
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
