#include <bits/time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include "../include/ansui.h"

/*
	AS A MONO-HEADER LIBRARY,
	== ALL INTERNAL FUNCTIONS STARTS WITH __
	== ALL FOR PUBLIC USE IS NORMAL, SELF-INDUCING
	== all functions start with ansui#();
*/

static struct ANSUI_CONFIG_GLOBAL glob_cfg;
static int8_t __debugMode = 0; // Could be changed

#include <fcntl.h>
#include <unistd.h>
#include <err.h>

// === INITIALIZATION
struct winsize ws;

static int fd;
static struct winsize __ansuiGetTermSize() {
	struct winsize ws;
	fd = open("/dev/tty", O_RDWR);

	if (fd < 0 || ioctl(fd, TIOCGWINSZ, &ws)) err(8, "/dev/tty");
	return ws;
}

#include <termios.h>

static struct termios __oldt;
static int __oldf;

static void __getTerminalConfig() {
	// Retrieve default flags and terminal config
	tcgetattr(fd, &__oldt);
	__oldf = fcntl(fd, F_GETFL, 0);
}


// Only call this if ANSUI_FLAG_INPUT is set
static void __setTerminalConfigInputMode() {
	// Save default terminal configs
	struct termios __newt = __oldt;
	
	// Sets off ICANON and ECHO
	__newt.c_lflag &= ~(ICANON | ECHO);

	// Sets them attributes as the same as __newt
	tcsetattr(fd, 0, &__newt);
	// And flags with no block, change it at current time
	fcntl(fd, F_SETFL, __oldf | O_NONBLOCK);
}

// === KEYBOARD

ANSUI_KEY ansuiGetKey() {
	int ch = getc(stdin);
	if (ch == EOF) { return ANSUI_KEY_NONE; }

	return (ch >= 97) ? (ANSUI_KEY)ch - 32 : (ANSUI_KEY)ch;
}

// === ATTRIBUTES

static ANSUI_CONFIG* __loadWinDefaultConfig() {
	ANSUI_CONFIG* cfg = malloc(sizeof(ANSUI_CONFIG));
	cfg->x = 0;
	cfg->y = 0;
	cfg->w = 25;
	cfg->h = 10;
	cfg->c = ' ';
	cfg->char_color = CH_BBLACK;
	cfg->bg_color = BG_BRED;

	return cfg;
}

void* ansuiLoadDefaultConfig(ANSUI_LOAD_ATTR attr) {
	ANSUI_CONFIG* win_cfg;
	// HACK: MAYBE USE THOSE BITS CHECKER TO GET THEM VALUES
	switch (attr) {
		case ANSUI_LOAD_GLOBAL_ATTR:
			// return malloc(sizeof(ANSUI_CONFIG_GLOBAL));
		case ANSUI_LOAD_WINDOW_ATTR:
			win_cfg = __loadWinDefaultConfig();
			return win_cfg;
			break; // QT: Is this needed?
	}
}

// === UTIL

static void __ansuiResizeWinOutOfBorder(ANSUI_WIN* win) {
	if (win->cfg->w <= ws.ws_col && win->cfg->x + win->cfg->w > ws.ws_col) {
		win->cfg->x -= (win->cfg->x + win->cfg->w - ws.ws_col);
	}
	if (win->cfg->h <= ws.ws_row && win->cfg->y + win->cfg->h > ws.ws_row) {
		win->cfg->y -= (win->cfg->y + win->cfg->h - ws.ws_row);
	}
	if (win->cfg->w > ws.ws_col) {
		win->cfg->w = ws.ws_col;
		win->cfg->x = 0;
	}
	if (win->cfg->h > ws.ws_row) {
		win->cfg->h = ws.ws_row;
		win->cfg->y = 0;
	}
}

// HACK: BOTH OF THESE
static void __ansuiFillPixelArray(ANSUI_CONFIG* cfg, ANSUI_PIXEL* pxa) {
	int16_t size = cfg->w * cfg->h;

	for (int i = 0; i < size; i++) {
		pxa[i].c = cfg->c;
		pxa[i].char_color = cfg->char_color;
		pxa[i].bg_color = cfg->bg_color;
	}
}

// === OBJECT

ANSUI_OBJECT *ansuiCreateObject(ANSUI_CONFIG *cfg) {
	ANSUI_OBJECT *obj = malloc(sizeof(ANSUI_OBJECT));
	obj->cfg = cfg;

	obj->pxa = malloc(sizeof(ANSUI_PIXEL) * obj->cfg->w * obj->cfg->h);

	// QT: Should object have any flags?

	__ansuiFillPixelArray(obj->cfg, obj->pxa);
	return obj;
}

// === WINDOW

// HACK: Will be letting this one until some more advanced/complex stuff (for me) comes upon my mind and I have to remake it by a whole
ANSUI_WIN* ansuiCreateWindow(ANSUI_CONFIG* cfg, ANSUI_WIN_FLAG flag) {
	// NOTE: When applying win = cfg, segfault occurs, even tho ANSUI_WIN is ANSUI_CONFIG_WINDOW; no idea
	ANSUI_WIN* win = malloc(sizeof(ANSUI_WIN));
	win->cfg = cfg;

	win->pxa = malloc(sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h);
	win->prev_pxa = malloc(sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h);

	if (flag & ANSUI_WIN_FLAG_AUTO_RESIZE) {
		__ansuiResizeWinOutOfBorder(win);
	}

	if (flag & ANSUI_WIN_FLAG_POS_CENTERED) {
		win->cfg->x = (int16_t)((double)ws.ws_col / 2 - (double)win->cfg->w / 2);
		win->cfg->y = (int16_t)((double)ws.ws_row / 2 - (double)win->cfg->h / 2);
	}

	__ansuiFillPixelArray(win->cfg, win->pxa);
	// free(cfg);
	return win;
}

int ansuiWinDestroy(ANSUI_WIN *win) {
	// Every malloc on *win is:
	free(win->pxa);
	free(win->cfg);

	// And free the window itself;
	free(win);
	return ANSUI_SUCCESS;
}

#include <time.h>

static struct timespec __lastTick, __curntTick;

// NOTE: We will have struct winsize at a global scope, outside this local function
//	 Therefore, we'd be returning a pointer to this struct
struct winsize* ansuiInit(ANSUI_FLAG flag) {
	__getTerminalConfig();
	// NOTE: Initialization consists on retrieving terminal info, like TIOCGWINSZ
	//	 Getting to know $HOME, current $PATH, probably some user info and other variables
	
	if (flag & ANSUI_FLAG_DEBUG) {
		__debugMode = 1;
	}

	if (flag & ANSUI_FLAG_INPUT) {
		__setTerminalConfigInputMode();
	}

	clock_gettime(CLOCK_MONOTONIC, &__lastTick);

	// DONE: Enforcing ansuiInit() is called => later use on ansuiGetInput() and ansuiQuit()
	//	Save old settings -> we'll be changing terminal's modes
	ws = __ansuiGetTermSize(); // This way we get info about ROWS, COLS

	// Returning only &ws since that's all it's needed to return => input keyboard should be fine everywhere
	return &ws;
}

int ansuiQuit() {
	// QT: Should QUIT also clear the screen? I'm saying it should not
	// printf("\e[%dm\e[0;0H", BG_RESET);

	tcsetattr(fd, 0, &__oldt);
	fcntl(fd, F_SETFL, __oldf);

	// Free memory got from ansuiInit();
	close(fd); // file descriptor

	return ANSUI_SUCCESS;
}

// === RENDER

int8_t __cmpPixels(ANSUI_PIXEL px1, ANSUI_PIXEL px2) {
	if (px1.c != px2.c) return 0;
	if (px1.bg_color != px2.bg_color) return 0;
	if (px1.char_color != px2.char_color) return 0;
	return 1;
}

static float __calcGetFPS() {
	clock_gettime(1, &__curntTick);
	// HACK: Get each second and nanosecond. Divide by milisseconds
	double __d = (__curntTick.tv_sec - __lastTick.tv_sec) + (__curntTick.tv_nsec - __lastTick.tv_nsec) / 1e9;
	__lastTick = __curntTick;

	// NOTE: Should wait for finishing of while loop on dev's code
	// TODO: 1.0 can be set as FPS_MAX
	return (1.0 / __d);
}

int ansuiRender(ANSUI_WIN* win) {
	// NOTE: Basically all we need is a formated buffer and therefore a pointer to it
	int char_bufsz = win->cfg->w * win->cfg->h; // NOTE: Must be +1 when sent to snprintf();
	int ansi_bufsz = char_bufsz * 27; // 9 => NOTE: Max ANSII String Size => \e[0;107m for example, has 9 chars
	#define ANSUI_DEBUG_LINE_BUFSZ 128
	int bufsz = char_bufsz + ansi_bufsz + 1; // NOTE: At the end, just append '\0', since buffer has to be detected as a string
						 // NOTE :For later on fputs(buffer, stdout);

	// Buffer size must be sizeof(char) * bufsz + size of
	char* buffer = malloc(sizeof(char) * bufsz);
	int fb = 0;
	int i = 0;
	int px_amnt = 0;

	for (int h = 0; h < win->cfg->h; h++) {
		for (int w = 0; w < win->cfg->w; w++) {
			if (__cmpPixels(win->prev_pxa[i], win->pxa[i]) != 1) {
				fb += snprintf(buffer + fb, bufsz - fb, "\e[%d;%dH\033[%dm\033[%dm%c", win->cfg->y + h, win->cfg->x + w, win->pxa[i].bg_color, win->pxa[i].char_color, win->pxa[i].c);
				px_amnt++;
			}
			i++;
		}
	}
	if (__debugMode)
		fb += snprintf(buffer + fb, bufsz - fb, "\e[0m\e[%d;1HPrinted %d pixels | FPS: %f", ws.ws_row, px_amnt, __calcGetFPS());

	fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	fputs(buffer, stdout);
	
	//	Free buffer for use of it again on a next render calls.
	//	Although shadowing is quite reinforced there
	//	Save memory space
	free(buffer);

	memcpy(win->prev_pxa, win->pxa, sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h);
	return ANSUI_SUCCESS;
}

void ansuiClear(ANSUI_PIXEL_BG_COLOR c) {
	printf("\033[%dm\033[2J\033[1H", c);
}
