#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/ansui.h"

/*
 * AS A MONO-HEADER LIBRARY,
 == ALL INTERNAL FUNCTIONS STARTS WITH __
 == ALL FOR PUBLIC USE IS NORMAL, SELF-INDUCING
 == all functions start with ansui#();
*/

static ANSUI_CONFIG_GLOBAL glob_cfg;
// TODO: Get this shitass pussy to work
// static ANSUI_CONFIG cfg;

#include <fcntl.h>
#include <unistd.h>
#include <err.h>

struct winsize ws;

static int fd;
static struct winsize __ansuiGetTermSize() {
	struct winsize ws;
	fd = open("/dev/tty", O_RDWR);

	if (fd < 0 || ioctl(fd, TIOCGWINSZ, &ws)) err(8, "/dev/tty");
	return ws;
}

// NOTE: We will have struct winsize at a global scope, outside this local function
//	 Therefore, we'd be returning a pointer to this struct
void* ansuiInit(ANSUI_FLAG flag) {
	// NOTE: Initialization consists on retrieving terminal info, like TIOCGWINSZ
	//	 Getting to know $HOME, current $PATH, probably some user info and other variables
	switch (flag) {
		default:
		case ANSUI_FLAG_NONE: // Normal default initialization
			ws = __ansuiGetTermSize(); // This way we get info about ROWS, COLS
			return &ws;
			break;
		// TODO: Switch in case passing, for example, any ANSUI_FLAG that can change our terminal info
		// TODO: setTermSize() with TIOCSWIN
	}
}

// === ATTRIBUTES

static ANSUI_WIN_CONFIG* __loadWinDefaultConfig() {
	ANSUI_WIN_CONFIG* cfg = malloc(sizeof(ANSUI_WIN_CONFIG));
	
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
	ANSUI_WIN_CONFIG* win_cfg;
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

// ===

void __ansuiResizeWinOutOfBorder(ANSUI_WIN* win) {
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
void __ansuiFillPixelArray(int size, ANSUI_PIXEL* pxa, ANSUI_WIN_CONFIG* cfg) {
	for (int i = 0; i < size; i++) {
		pxa[i].c = cfg->c;
		pxa[i].char_color = cfg->char_color;
		pxa[i].bg_color = cfg->bg_color;
	}
}

// TODO: Might look better to load into a cfg and just pass it as a whole on ansuiCreateWindow, but conceptually it's the same
// NOTE: Will be letting this one until some more advanced/complex stuff (for me) comes upon my mind and I have to remake it by a whole

// TODO: Understand what type of shit I've messed changing these config stuff
ANSUI_WIN* ansuiCreateWindow(ANSUI_WIN_CONFIG* cfg, ANSUI_WIN_FLAG flag) {
	// NOTE: When applying win = cfg, segfault occurs, even tho ANSUI_WIN is ANSUI_CONFIG_WINDOW; no idea
	ANSUI_WIN* win = malloc(sizeof(ANSUI_WIN));
	win->cfg = cfg;

	win->pxa = malloc(sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h);
	__ansuiResizeWinOutOfBorder(win);
	__ansuiFillPixelArray(win->cfg->w * win->cfg->h, win->pxa, win->cfg);

	switch (flag) {
		/*
		default:
		case ANSUI_WIN_OPAQUE: // "blankness" set from here
			__ansuiFillPixelArray(win);
			break;
		case ANSUI_WIN_TRANSPARENT:
			break;
		*/
		case ANSUI_WIN_FLAG_POS_CENTERED:
			win->cfg->x = (double)ws.ws_col / 2 - (double)win->cfg->w / 2;
			win->cfg->y = ws.ws_row / 2 - win->cfg->y / 2;
			break;
		default:
		case ANSUI_WIN_FLAG_NONE:
			break;
	}
	
	// free(cfg);
	return win;
}


int ansuiWinDestroy(ANSUI_WIN *win) {
	// Every malloc on *win is:
	free(win->pxa);
	free(win->cfg);
	free(win);
	return ANSUI_SUCCESS;
}

int ansuiQuit() {
	printf("\033[0m");

	// Free memory got from ansuiInit();
	close(fd); // file descriptor
	
	return ANSUI_SUCCESS;
}

// === RENDER

int __equalPixels(ANSUI_PIXEL px1, ANSUI_PIXEL px2) {
	if (px1.c != px2.c) return 0;
	if (px1.bg_color != px2.bg_color) return 0;
	if (px1.char_color != px2.char_color) return 0;
	return 1;
}

// NOTE: Probably need of a custom made cursor movement
int ansuiRender(ANSUI_WIN* win) {
	win->prev_pxa = malloc(sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h); // Set them as the same size, so no error

	// NOTE: Basically all we need is a formated buffer and therefore a pointer to it
	int char_bufsz = win->cfg->w * win->cfg->h; // NOTE: Must be +1 when sent to snprintf();
	int ansi_bufsz = char_bufsz * 18; // 9 => NOTE: Max ANSII String Size => \e[0;107m for example, has 9 chars
	int bufsz = char_bufsz + ansi_bufsz + 1; // NOTE: At the end, just append '\0', since buffer has to be detected as a string
						 // NOTE :For later on fputs(buffer, stdout);

	// Buffer size must be sizeof(char) * bufsz + size of
	char* buffer = malloc(sizeof(char) * bufsz);
	int fb = 0;

	int i = 0;

	// DONE: Improve performance by printing only changed pixels: cursor might move along i
	// PERF: Probably faster, in need to make any fps counter haha
	for (int h = 0; h < win->cfg->h; h++) {
		int px_amnt = 0;
		// fb += snprintf(buffer + fb, bufsz - fb, "", win->cfg->y + h, win->cfg->x); // change cursor position | same as setting x and y
		for (int w = 0; w < win->cfg->w; w++) {
			// HACK: After this appending, we check:
			// 	 In case it's different, we just remove from it (?)
			// 	 Do some sort of i--
			if (__equalPixels(win->prev_pxa[i], win->pxa[i]) == 1) {
				fb += snprintf(buffer + fb, bufsz - fb, "\033[1C");
			} else {
				fb += snprintf(buffer + fb, bufsz - fb, "\033[%dm\033[%dm%c", win->pxa[i].bg_color, win->pxa[i].char_color, win->pxa[i].c);
				px_amnt++;
			}
			i++;
		}
		fb += snprintf(buffer + fb, bufsz - fb, "\033[%dHPrinted %d pixels\e[%d;%dH", ws.ws_col, px_amnt, win->cfg->y + h, win->cfg->x);
	}

	fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	fputs(buffer, stdout);

	free(buffer);
	
	// QT: I hope it's not somehow poiting to it;
	//     Should not give error when running ./demos/window.c for example
	win->prev_pxa = win->pxa;
	return ANSUI_SUCCESS;
}

void ansuiClear(ANSUI_PIXEL_BG_COLOR c) {
	printf("\033[%dm\033[2J\033[1H", c);
}
