#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>

#ifndef TGUI_H
#define TGUI_H

#define TGUI_TRUE 1
#define TGUI_FALSE 0

#define TGUI_SUCCESS 0
#define TGUI_ERROR -1

#define TGUI_INIT_H
#ifdef TGUI_INIT_H

extern struct {
	struct winsize* ws; // Stuff retrieved from getTermSize() => ./src/tgui_init.c
} TGUI_UINFO;

typedef enum {
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

typedef struct {
	char c;
	TGUI_PIXEL_COLOR char_color;
	TGUI_PIXEL_COLOR bg_color;
} TGUI_PIXEL;

// PERF: Don't think this is needed, just use any array of px and pull stuff from them
/*
typedef struct {
	int size;
	TGUI_PIXEL* px;
} TGUI_PIXEL_ARRAY;
*/

// === ATTRIBUTES
// NOTE: These attributes are like flags, not variables to be handled but something
//	 that's used as a way of giving it the life it deserves.

// NOTE: Which config attribute to load to
typedef enum {
	TGUI_LOAD_GLOBAL_ATTR,
	TGUI_LOAD_WINDOW_ATTR,
	TGUI_LOAD_ENTITY_ATTR,
	TGUI_LOAD_WIDGET_ATTR,
} TGUI_LOAD_ATTR;

void* tguiLoadDefaultConfig(TGUI_LOAD_ATTR attr);

// === CONFIG

// HACK: FOR USE GENERICALLY
typedef void* TGUI_CONFIG;

typedef struct TGUI_CONFIG_GLOBAL {
	TGUI_PIXEL_COLOR clear_color;// = TGUI_PIXEL_RESET_COLOR;
} TGUI_CONFIG_GLOBAL;

typedef struct TGUI_CONFIG_WINDOW {
	int x;// = 0;
	int y;// = 0;
	int w;// = 25;
	int h;// = 10;
	char c;// = ' ';
	TGUI_PIXEL_COLOR char_color;// = TGUI_PIXEL_RESET_COLOR;
	TGUI_PIXEL_COLOR bg_color;// = TGUI_PIXEL_COLOR_BG_BRED;

	TGUI_PIXEL* px;
} TGUI_CONFIG_WINDOW;

/*
typedef struct TGUI_CONFIG_ENTITY {
	int x = 0;
	int y = 0;
	int w = 25;
	int h = 10;
	TGUI_PIXEL_COLOR char_color = TGUI_PIXEL_RESET_COLOR;
	TGUI_PIXEL_COLOR bg_color = TGUI_PIXEL_COLOR_BG_BRED;
} TGUI_CONFIG_ENTITY;

typedef struct TGUI_CONFIG_WIDGET {
	int x = 0;
	int y = 0;
	int w = 25;
	int h = 10;
	TGUI_PIXEL_COLOR char_color = TGUI_PIXEL_RESET_COLOR;
	TGUI_PIXEL_COLOR bg_color = TGUI_PIXEL_COLOR_BG_BRED;
} TGUI_CONFIG_WIDGET;
*/

// === WINDOW

// NOTE: First have to change config itself, then copy it towards window;
typedef TGUI_CONFIG_WINDOW TGUI_WIN;

// TODO: SET THESE FLAGS AS BITS
//	 JUST LIKE: 0x00000000 first one
//	 	    0x00000001 second one
//	 	    0x00000002 third
//	 	    0x00000004
//	 	    0x00000008
//	 Or something like that
typedef enum {
	// TGUI_WIN_TRANSPARENT,
	// TGUI_WIN_OPAQUE, // will be pulling TGUI_WIN_BLANK from here
			 // probably setting it's "blankness" from their config
	
	TGUI_WIN_POS_CENTERED
} TGUI_WIN_FLAG;

void tguiSetFlags(void* cfg, ...);

TGUI_WIN* tguiCreateWindow(TGUI_CONFIG_WINDOW* cfg, TGUI_WIN_FLAG flag);

// int tguiUpdate(TGUI_WIN* win, TGUI_WIN_ATTR attr, ...);

int tguiWinDestroy(TGUI_WIN* win);

int tguiQuit();

// === RENDER

int tguiRender(TGUI_WIN* win);

void tguiClear(); // clear whole terminal

#endif // TGUI_H
