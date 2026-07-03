#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ioctl.h>

#ifndef ANSUI_H
#define ANSUI_H

#define ANSUI_TRUE 1
#define ANSUI_FALSE 0

#define ANSUI_SUCCESS 0
#define ANSUI_ERROR -1

#define ANSUI_INIT_H
#ifdef ANSUI_INIT_H

extern struct {
	struct winsize* ws; // Stuff retrieved from getTermSize() => ./src/ansui_init.c
} ANSUI_UINFO;

typedef enum {
	ANSUI_FLAG_NONE,
} ANSUI_FLAG;

void* ansuiInit(ANSUI_FLAG flag);

#endif // ANSUI_INIT_H

// === PIXEL

// ANSUI_PIXEL_COLOR_X_YC
// X => BACKGROUND OR TEXT
// Y => REGULAR OR BOLD ( no high intensity )
// C => COLOR

// TODO: SET ALL COLORS AS ENUM. PULL FROM THEM AFTERWARDS
// NOTE: All ANSII code colors consists on: "ESCAPESEQUENCE[BOLD;COLORm"
// NOTE: For example: \e[0;97m; => Not bold high-intensity white

// ANSUI_SET_BG/CH_COLOR, BOLD/REGULAR, # (COLOR_RED/BLUE/GREEN)

#define ANSUI_PIXEL_COLOR_CH_RBLACK "\e[0;90m"
#define ANSUI_PIXEL_COLOR_CH_RRED "\e[0;91m"
#define ANSUI_PIXEL_COLOR_CH_RGREEN "\e[0;92m"
#define ANSUI_PIXEL_COLOR_CH_RBLUE "\e[0;94m"
#define ANSUI_PIXEL_COLOR_CH_RWHITE "\e[0;97m"

#define ANSUI_PIXEL_COLOR_CH_BBLACK "\e[1;90m"
#define ANSUI_PIXEL_COLOR_CH_BRED "\e[1;91m"
#define ANSUI_PIXEL_COLOR_CH_BGREEN "\e[1;92m"
#define ANSUI_PIXEL_COLOR_CH_BBLUE "\e[1;94m"
#define ANSUI_PIXEL_COLOR_CH_BWHITE "\e[1;97m"

#define ANSUI_PIXEL_COLOR_BG_RBLACK "\e[0;40m"
#define ANSUI_PIXEL_COLOR_BG_RRED "\e[0;41m"
#define ANSUI_PIXEL_COLOR_BG_RGREEN "\e[0;42m"
#define ANSUI_PIXEL_COLOR_BG_RBLUE "\e[0;44m"
#define ANSUI_PIXEL_COLOR_BG_RWHITE "\e[0;47m"

#define ANSUI_PIXEL_COLOR_BG_BBLACK "\e[0;100m"
#define ANSUI_PIXEL_COLOR_BG_BRED "\e[0;101m"
#define ANSUI_PIXEL_COLOR_BG_BGREEN "\e[0;102m"
#define ANSUI_PIXEL_COLOR_BG_BBLUE "\e[0;104m"
#define ANSUI_PIXEL_COLOR_BG_BWHITE "\e[0;107m"

#define ANSUI_PIXEL_RESET_COLOR "\e[0m"

typedef const char* ANSUI_PIXEL_COLOR;

typedef struct {
	char c;
	ANSUI_PIXEL_COLOR char_color;
	ANSUI_PIXEL_COLOR bg_color;
} ANSUI_PIXEL;

// PERF: Don't think this is needed, just use any array of px and pull stuff from them
/*
typedef struct {
	int size;
	ANSUI_PIXEL* px;
} ANSUI_PIXEL_ARRAY;
*/

// === ATTRIBUTES
// NOTE: These attributes are like flags, not variables to be handled but something
//	 that's used as a way of giving it the life it deserves.

// NOTE: Which config attribute to load to
typedef enum {
	ANSUI_LOAD_GLOBAL_ATTR,
	ANSUI_LOAD_WINDOW_ATTR,
	ANSUI_LOAD_ENTITY_ATTR,
	ANSUI_LOAD_WIDGET_ATTR,
} ANSUI_LOAD_ATTR;

void* ansuiLoadDefaultConfig(ANSUI_LOAD_ATTR attr);

// === CONFIG

// HACK: FOR USE GENERICALLY
typedef void* ANSUI_CONFIG;

typedef struct ANSUI_CONFIG_GLOBAL {
	ANSUI_PIXEL_COLOR clear_color;// = ANSUI_PIXEL_RESET_COLOR;
} ANSUI_CONFIG_GLOBAL;

typedef struct ANSUI_CONFIG_WINDOW {
	int x;// = 0;
	int y;// = 0;
	int w;// = 25;
	int h;// = 10;
	char c;// = ' ';
	ANSUI_PIXEL_COLOR char_color;// = ANSUI_PIXEL_RESET_COLOR;
	ANSUI_PIXEL_COLOR bg_color;// = ANSUI_PIXEL_COLOR_BG_BRED;

	ANSUI_PIXEL* px;
} ANSUI_CONFIG_WINDOW;

/*
typedef struct ANSUI_CONFIG_ENTITY {
	int x = 0;
	int y = 0;
	int w = 25;
	int h = 10;
	ANSUI_PIXEL_COLOR char_color = ANSUI_PIXEL_RESET_COLOR;
	ANSUI_PIXEL_COLOR bg_color = ANSUI_PIXEL_COLOR_BG_BRED;
} ANSUI_CONFIG_ENTITY;

typedef struct ANSUI_CONFIG_WIDGET {
	int x = 0;
	int y = 0;
	int w = 25;
	int h = 10;
	ANSUI_PIXEL_COLOR char_color = ANSUI_PIXEL_RESET_COLOR;
	ANSUI_PIXEL_COLOR bg_color = ANSUI_PIXEL_COLOR_BG_BRED;
} ANSUI_CONFIG_WIDGET;
*/

// === WINDOW

// NOTE: First have to change config itself, then copy it towards window;
typedef ANSUI_CONFIG_WINDOW ANSUI_WIN;

// TODO: SET THESE FLAGS AS BITS
//	 JUST LIKE: 0x00000000 first one
//	 	    0x00000001 second one
//	 	    0x00000002 third
//	 	    0x00000004
//	 	    0x00000008
//	 Or something like that
typedef enum {
	// ANSUI_WIN_TRANSPARENT,
	// ANSUI_WIN_OPAQUE, // will be pulling ANSUI_WIN_BLANK from here
			 // probably setting it's "blankness" from their config
	
	ANSUI_WIN_POS_CENTERED
} ANSUI_WIN_FLAG;

void ansuiSetFlags(void* cfg, ...);

ANSUI_WIN* ansuiCreateWindow(ANSUI_CONFIG_WINDOW* cfg, ANSUI_WIN_FLAG flag);

// int ansuiUpdate(ANSUI_WIN* win, ANSUI_WIN_ATTR attr, ...);

int ansuiWinDestroy(ANSUI_WIN* win);

int ansuiQuit();

// === RENDER

int ansuiRender(ANSUI_WIN* win);

void ansuiClear(); // clear whole terminal

#endif // ANSUI_H
