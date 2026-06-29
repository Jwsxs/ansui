#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef TGUI_H
#define TGUI_H

#define TGUI_TRUE 1
#define TGUI_FALSE 0

#define TGUI_SUCCESS 0
#define TGUI_ERROR -1

// === FLAG

typedef enum TGUI_FLAG {
	TGUI_FLAG_NONE,
} TGUI_FLAG;

int tguiInit(TGUI_FLAG flag);

// === PIXEL

// TGUI_PIXEL_COLOR_X_YC
// X => BACKGROUND OR TEXT
// Y => REGULAR OR BOLD ( no high intensity )
// C => COLOR

#define TGUI_PIXEL_COLOR_CH_RBLACK "\033[0;90m"
#define TGUI_PIXEL_COLOR_CH_RRED "\033[0;91m"

#define TGUI_PIXEL_COLOR_CH_BBLACK "\033[1;90m"
#define TGUI_PIXEL_COLOR_CH_BRED "\033[1;91m"

#define TGUI_PIXEL_COLOR_BG_RBLACK "\033[0;40m"
#define TGUI_PIXEL_COLOR_BG_RRED "\033[0;41m"
#define TGUI_PIXEL_COLOR_BG_RGREEN "\033[0;42m"
#define TGUI_PIXEL_COLOR_BG_RBLUE "\033[0;44m"
#define TGUI_PIXEL_COLOR_BG_RWHITE "\033[0;47m"

#define TGUI_PIXEL_COLOR_BG_BBLACK "\033[0;100m"
#define TGUI_PIXEL_COLOR_BG_BRED "\033[0;101m"
#define TGUI_PIXEL_COLOR_BG_BGREEN "\033[0;102m"
#define TGUI_PIXEL_COLOR_BG_BBLUE "\033[0;104m"
#define TGUI_PIXEL_COLOR_BG_BWHITE "\033[0;107m"

#define TGUI_PIXEL_RESET_COLOR "\033[0m"

typedef const char* TGUI_PIXEL_COLOR;

typedef struct TGUI_PIXEL {
	uint8_t x;
	uint8_t y;
	char c;
	TGUI_PIXEL_COLOR color;
} TGUI_PIXEL;

typedef struct TGUI_PIXEL_ARRAY {
	int size;
	TGUI_PIXEL* px;
} TGUI_PIXEL_ARRAY;

// === CONFIG

typedef struct TGUI_CONFIG {
	TGUI_PIXEL_COLOR clear_color;

	TGUI_PIXEL_COLOR color;

	char fill_char;

	int has_border;
} TGUI_CONFIG;

// === ATTRIBUTES

typedef enum TGUI_GLOB_ATTR {
	TGUI_ATTR_CLEAR_COLOR,
} TGUI_GLOB_ATTR;

typedef enum TGUI_WIN_ATTR {
	TGUI_ATTR_PXA_COLOR,
	TGUI_ATTR_PXA_FILL_CHAR,

	TGUI_ATTR_WIN_HAS_BORDER,
} TGUI_WIN_ATTR;

void tguiSetGlobAttr(TGUI_GLOB_ATTR attr, ...);
void tguiSetWinAttr(TGUI_WIN_ATTR attr, ...);

// === WINDOW

typedef struct TGUI_WIN {
	int x, y;
	int width;
	int height;
	TGUI_PIXEL_ARRAY* pxa;
	TGUI_CONFIG config;
} TGUI_WIN;

typedef enum TGUI_WIN_FLAG {
	TGUI_WIN_BLANK,
	TGUI_WIN_FILLED,
} TGUI_WIN_FLAG;

TGUI_WIN* tguiCreateWindow(int x, int y, int width, int height, TGUI_WIN_FLAG flag);

static void tguiFillPixelArray(TGUI_WIN* win);
static void tguiFillPixelCharArray(TGUI_WIN* win, char c);
static void tguiFillPixelColorArray(TGUI_WIN* win, TGUI_PIXEL_COLOR color);

int tguiUpdate(TGUI_WIN* win, TGUI_WIN_ATTR attr, ...);

// === RENDER

int tguiRender(TGUI_WIN* win);

void tguiClear(); // clear whole window, terminal itself

#endif
