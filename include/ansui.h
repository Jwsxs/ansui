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

// QT: Not being used, what to do?
/*
extern struct {
	struct winsize* ws; // Stuff retrieved from getTermSize() => ./src/ansui_init.c
} ANSUI_UINFO;
*/

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

// HACK: MAYBE THIS WOULD SET DIFFERENCE BETWEEN cfg->char_color AND cfg->bg_color;
// typedef const char* ANSUI_PIXEL_BG_COLOR;
// typedef const char* ANSUI_PIXEL_CH_COLOR;

typedef enum ANSUI_PIXEL_CH_COLOR {
	CH_RRED = 91,// = "\e[0;91m",
	CH_RGREEN = 92,// = "\e[0;92m",
	CH_RBLUE = 94,// = "\e[0;94m",
	CH_RBLACK = 90,// = "\e[0;90m",
	CH_RWHITE = 97,// = "\e[0;97m",

	CH_BRED = 191,// = "\e[1;91m",
	CH_BGREEN = 192,// = "\e[1;92m",
	CH_BBLUE = 194,// = "\e[1;94m",
	CH_BBLACK = 190,// = "\e[1;90m",
	CH_BWHITE = 197,// = "\e[1;97m",
} ANSUI_PIXEL_CH_COLOR;

typedef enum ANSUI_PIXEL_BG_COLOR {
	BG_RBLACK = 40,// = "\e[0;40m",
	BG_RRED = 41,// = "\e[0;41m",
	BG_RGREEN = 42,// = "\e[0;42m",
	BG_RBLUE = 44,// = "\e[0;44m",
	BG_RWHITE = 47,// = "\e[0;47m",

	BG_BBLACK = 100,// = "\e[0;100m",
	BG_BRED = 101,// = "\e[0;101m",
	BG_BGREEN = 102,// = "\e[0;102m",
	BG_BBLUE = 104,// = "\e[0;104m",
	BG_BWHITE = 107,// = "\e[0;107m",

	BG_RESET = 0,
} ANSUI_PIXEL_BG_COLOR;

// typedef const char* ANSUI_PIXEL_COLOR;

typedef struct {
	char c;
	ANSUI_PIXEL_CH_COLOR char_color;
	ANSUI_PIXEL_BG_COLOR bg_color;
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
} ANSUI_LOAD_ATTR;

void* ansuiLoadDefaultConfig(ANSUI_LOAD_ATTR attr);

// === CONFIG

// HACK: FOR USE GENERICALLY
typedef void* ANSUI_CONFIG;

typedef struct ANSUI_CONFIG_GLOBAL {
	ANSUI_PIXEL_BG_COLOR clear_color;// = ANSUI_PIXEL_RESET_COLOR; => HACK: CLEAR IT ON BACKGROUND
} ANSUI_CONFIG_GLOBAL;

// === ENTITIES / WIDGET / OBJECT
/* NOTE:
 * KEEPING ALL OBJECTS / WIDGETS / ENTITIES AS A WINDOW FOR MY OWN SAKE
 * YOU CAN MOVE / INTERACT WITH THEM BY JUST MESSING WITH THEM CFGs
*/

// === WINDOW

// NOTE: First create a window config, later on apply it on a window
typedef struct ANSUI_WIN_CONFIG {
	int x;// = 0;
	int y;// = 0;
	int w;// = 25;
	int h;// = 10;
	char c;// = ' ';
	ANSUI_PIXEL_CH_COLOR char_color;// = ANSUI_PIXEL_RESET_COLOR;
	ANSUI_PIXEL_BG_COLOR bg_color;// = ANSUI_PIXEL_COLOR_BG_BRED;
} ANSUI_WIN_CONFIG;

typedef struct ANSUI_WIN {
	ANSUI_WIN_CONFIG* cfg;
	ANSUI_PIXEL* pxa;
	ANSUI_PIXEL* prev_pxa;
} ANSUI_WIN;

// TODO: SET THESE FLAGS AS BITS
//	 JUST LIKE: 0x00000000 first one
//	 	    0x00000001 second one
//	 	    0x00000002 third
//	 	    0x00000004
//	 	    0x00000008
//	 Or something like that
typedef enum {
	ANSUI_WIN_FLAG_NONE,
	// ANSUI_WIN_TRANSPARENT,
	// ANSUI_WIN_OPAQUE, // will be pulling ANSUI_WIN_BLANK from here
			 // probably setting it's "blankness" from their config
	
	ANSUI_WIN_FLAG_POS_CENTERED
} ANSUI_WIN_FLAG;

void ansuiSetFlags(void* cfg, ...);

ANSUI_WIN* ansuiCreateWindow(ANSUI_WIN_CONFIG* cfg, ANSUI_WIN_FLAG flag);

int ansuiWinDestroy(ANSUI_WIN* win);

int ansuiQuit();

// === RENDER

int ansuiRender(ANSUI_WIN* win);

// HACK: CLEAR COLOR MUST BE BACKGROUND
void ansuiClear(ANSUI_PIXEL_BG_COLOR c); // clear whole terminal

#endif // ANSUI_H
