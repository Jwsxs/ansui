#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef TGUI_H
#define TGUI_H

#define TGUI_TRUE 1
#define TGUI_FALSE 0

#define TGUI_SUCCESS 0
#define TGUI_ERROR -1

// ===

typedef enum TGUI_FLAG {
	TGUI_FLAG_NONE,
} TGUI_FLAG;

uint8_t tguiInit(TGUI_FLAG flag);

// ===

// TGUI_PIXEL_COLOR_X_YC
// X => BACKGROUND OR TEXT
// Y => REGULAR OR BOLD ( no high intensity )
// C => COLOR

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

#define TGUI_WIN_COLOR_CLEAR "\033[0m"

typedef const char* TGUI_PIXEL_COLOR;
extern TGUI_PIXEL_COLOR TGUI_WIN_INITIAL_COLOR;

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

typedef struct TGUI_WIN {
	uint8_t width;
	uint8_t height;
	TGUI_PIXEL_ARRAY* pxa;
} TGUI_WIN;

typedef enum TGUI_WIN_FLAG {
	TGUI_WIN_BLANK,
	//TGUI_WIN_COLORED,
} TGUI_WIN_FLAG;

TGUI_WIN* tguiCreateWindow(uint8_t width, uint8_t height, TGUI_WIN_FLAG flag);

static void tguiFillPixelArray(TGUI_PIXEL_ARRAY** pxa, char c);

// ===

int tguiRender(TGUI_WIN* win);

void tguiClear();

#endif
