#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ./src/tgui_init.c
#include <sys/ioctl.h>

#ifndef TGUI_H
#define TGUI_H

#define TGUI_TRUE 1
#define TGUI_FALSE 0

#define TGUI_SUCCESS 0
#define TGUI_ERROR -1

#define TGUI_INIT_H
#ifdef TGUI_INIT_H

extern struct TGUI_UINFO {
	struct winsize* ws; // Stuff retrieved from getTermSize() => ./src/tgui_init.c
} TGUI_UINFO;

typedef enum TGUI_FLAG {
	TGUI_FLAG_NONE,
} TGUI_FLAG;

void* tguiInit(TGUI_FLAG flag);

#endif // TGUI_INIT_H

// === PIXEL

// TGUI_PIXEL_COLOR_X_YC
// X => BACKGROUND OR TEXT
// Y => REGULAR OR BOLD ( no high intensity )
// C => COLOR

// TODO: SET ALL COLORS AS ENUM. PULL FROM THEM AFTERWARDS
// NOTE: All ANSII code colors consists on: "ESCAPESEQUENCE[BOLD;COLORm"
// NOTE: For example: \e[0;97m; => Not bold high-intensity white

// TGUI_SET_BG/CH_COLOR, BOLD/REGULAR, # (COLOR_RED/BLUE/GREEN)

#define TGUI_PIXEL_COLOR_CH_RBLACK "\e[0;90m"
#define TGUI_PIXEL_COLOR_CH_RRED "\e[0;91m"
#define TGUI_PIXEL_COLOR_CH_RGREEN "\e[0;92m"
#define TGUI_PIXEL_COLOR_CH_RBLUE "\e[0;94m"
#define TGUI_PIXEL_COLOR_CH_RWHITE "\e[0;97m"

#define TGUI_PIXEL_COLOR_CH_BBLACK "\e[1;90m"
#define TGUI_PIXEL_COLOR_CH_BRED "\e[1;91m"
#define TGUI_PIXEL_COLOR_CH_BGREEN "\e[1;92m"
#define TGUI_PIXEL_COLOR_CH_BBLUE "\e[1;94m"
#define TGUI_PIXEL_COLOR_CH_BWHITE "\e[1;97m"

#define TGUI_PIXEL_COLOR_BG_RBLACK "\e[0;40m"
#define TGUI_PIXEL_COLOR_BG_RRED "\e[0;41m"
#define TGUI_PIXEL_COLOR_BG_RGREEN "\e[0;42m"
#define TGUI_PIXEL_COLOR_BG_RBLUE "\e[0;44m"
#define TGUI_PIXEL_COLOR_BG_RWHITE "\e[0;47m"

#define TGUI_PIXEL_COLOR_BG_BBLACK "\e[0;100m"
#define TGUI_PIXEL_COLOR_BG_BRED "\e[0;101m"
#define TGUI_PIXEL_COLOR_BG_BGREEN "\e[0;102m"
#define TGUI_PIXEL_COLOR_BG_BBLUE "\e[0;104m"
#define TGUI_PIXEL_COLOR_BG_BWHITE "\e[0;107m"

#define TGUI_PIXEL_RESET_COLOR "\e[0m"

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

	int is_opaque; // ignore totally .fill_char in case it's 0
		       // == 0 => TGUI_WIN_TRANSPARENT
		       // == 1 => TGUI_WIN_OPAQUE
	int has_border;
	int is_centered;
} TGUI_CONFIG;

// === ATTRIBUTES

typedef enum TGUI_GLOB_ATTR {
	TGUI_ATTR_CLEAR_COLOR,
} TGUI_GLOB_ATTR;

typedef enum TGUI_WIN_ATTR {
	TGUI_ATTR_PXA_COLOR,
	TGUI_ATTR_PXA_FILL_CHAR,

	TGUI_ATTR_WIN_IS_OPAQUE,
	TGUI_ATTR_WIN_HAS_BORDER,
	TGUI_ATTR_WIN_POSITION_CENTERED,
} TGUI_WIN_ATTR;

void tguiSetGlobAttr(TGUI_GLOB_ATTR attr, ...);
void tguiSetWinAttr(TGUI_WIN_ATTR attr, ...);

// === ENTITIES
// for entities it's different, since most of it is an object with different w and h
// also, different colors, for sprite, maybe image integration? ( BMP-PARSER or JPEG )

typedef struct TGUI_ENTITY {

} TGUI_ENTITY;

// each window will have it's own entities
typedef struct TGUI_ENTITIES {
	int size; // amount of entities to be there
	TGUI_ENTITY* ent;
} TGUI_ENTITIES;

// === WIDGET

typedef enum TGUI_WIDGET_TYPE {
	TGUI_WIDGET_TEXT,
	// TGUI_WIDGET_BUTTON, // don't think I'll be using pretty soon
} TGUI_WIDGET_TYPE;

typedef struct TGUI_WIDGET {
	int x, y;
	int width, height;
	TGUI_WIDGET_TYPE type;
} TGUI_WIDGET;

// === WINDOW

typedef struct TGUI_WIN {
	int x, y;
	int width;
	int height;
	int depth; // not used yet
	TGUI_PIXEL_ARRAY* pxa;
	TGUI_CONFIG cfg;

	TGUI_WIDGET* widget;
	TGUI_ENTITIES* entities;
} TGUI_WIN;

typedef enum TGUI_WIN_FLAG {
	TGUI_WIN_TRANSPARENT,
	TGUI_WIN_OPAQUE, // will be pulling TGUI_WIN_BLANK from here
			 // probably setting it's "blankness" from their config
} TGUI_WIN_FLAG;

TGUI_WIN* tguiCreateWindow(int x, int y, int width, int height, TGUI_WIN_FLAG flag);

int tguiUpdate(TGUI_WIN* win, TGUI_WIN_ATTR attr, ...);

int tguiWinDestroy(TGUI_WIN* win);

int tguiQuit();

// === RENDER

int tguiRender(TGUI_WIN* win);

void tguiClear(); // clear whole terminal

#endif // TGUI_H
