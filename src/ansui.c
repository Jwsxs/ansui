#include <bits/time.h>
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

static struct ANSUI_CONFIG_GLOBAL glob_cfg;
int __debugMode = 1; // Could be changed
// TODO: Get this shitass pussy to work
// static ANSUI_CONFIG cfg;

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

static struct termios __newt;

static int __termiAttrWasChanged = 0;

static void __configKeyb() {
	tcgetattr(STDIN_FILENO, &__oldt);
	// Save default terminal configs
	__newt = __oldt;
	
	// Sets off ICANON and ECHO
	__newt.c_lflag &= ~(ICANON | ECHO);

	// Retrieve default flags
	__oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	
	// Sets them attributes as the same as __newt
	tcsetattr(STDIN_FILENO, 0, &__newt);
	// And flags with no block, change it at current time
	fcntl(STDIN_FILENO, F_SETFL, __oldf | O_NONBLOCK);
}

#include <time.h>

static struct timespec __lastTick, __curntTick;

// NOTE: We will have struct winsize at a global scope, outside this local function
//	 Therefore, we'd be returning a pointer to this struct
void* ansuiInit(ANSUI_FLAG flag) {
	// NOTE: Initialization consists on retrieving terminal info, like TIOCGWINSZ
	//	 Getting to know $HOME, current $PATH, probably some user info and other variables
	switch (flag) {
		// TODO: Switch in case passing, for example, any ANSUI_FLAG that can change our terminal info
		// TODO: setTermSize() with TIOCSWIN
		case ANSUI_FLAG_INPUT:
			__termiAttrWasChanged = 1;
			__configKeyb();

			clock_gettime(CLOCK_MONOTONIC, &__lastTick);
			break;

		default:
		case ANSUI_FLAG_NONE: // Normal default initialization
			break;
	}

	// HACK: Enforcing ansuiInit() is called => later use on ansuiGetInput() and ansuiQuit()
	//	Save old settings -> we'll be changing terminal's modes
	ws = __ansuiGetTermSize(); // This way we get info about ROWS, COLS

	// Returning only &ws since that's all it's needed to return => input keyboard should be fine everywhere
	return &ws;
}

// === KEYBOARD

// HACK: Global __curntKey for check on release key;
ANSUI_KEYP ansuiGetKey() {
	int ch = getc(stdin);

	if (ch == EOF) {
		return ANSUI_KEY_NONE;
	}
	// HACK: Probably will need to iterate through every possible
	//	I'll mess only with ASCII Alphabetical keys
	return (ch >= 97) ? (ANSUI_KEYP)ch - 32 : (ANSUI_KEYP)ch;
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

// HACK: Will be letting this one until some more advanced/complex stuff (for me) comes upon my mind and I have to remake it by a whole
ANSUI_WIN* ansuiCreateWindow(ANSUI_WIN_CONFIG* cfg, ANSUI_WIN_FLAG flag) {
	// NOTE: When applying win = cfg, segfault occurs, even tho ANSUI_WIN is ANSUI_CONFIG_WINDOW; no idea
	ANSUI_WIN* win = malloc(sizeof(ANSUI_WIN));
	win->cfg = cfg;

	win->pxa = malloc(sizeof(ANSUI_PIXEL) * win->cfg->w * win->cfg->h);

	switch (flag) {
		case ANSUI_WIN_FLAG_RESIZE: // HACK: Check commit 034b211's file ./demos/window => auto resizing is breaking the render
			__ansuiResizeWinOutOfBorder(win);
			break;
		case ANSUI_WIN_FLAG_POS_CENTERED:
			win->cfg->x = (double)ws.ws_col / 2 - (double)win->cfg->w / 2;
			win->cfg->y = ws.ws_row / 2 - win->cfg->y / 2;
			break;
		default:
		case ANSUI_WIN_FLAG_NONE:
			break;
	}

	__ansuiFillPixelArray(win->cfg->w * win->cfg->h, win->pxa, win->cfg);
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

int ansuiQuit() {
	printf("\033[0m");

	// Free memory got from ansuiInit();
	close(fd); // file descriptor

	// HACK: Turning off changed configs on ansuiInit();

	if (__termiAttrWasChanged) {
		tcsetattr(STDIN_FILENO, 0, &__oldt);
		fcntl(STDIN_FILENO, F_SETFL, __oldf);
	}

	return ANSUI_SUCCESS;
}

// === RENDER

int __equalPixels(ANSUI_PIXEL px1, ANSUI_PIXEL px2) {
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
		if (__debugMode)
			fb += snprintf(buffer + fb, bufsz - fb, "\e[0m\033[%dHPrinted %d pixels | FPS: %f\e[%d;%dH", ws.ws_col, px_amnt, __calcGetFPS(), win->cfg->y + h, win->cfg->x);
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
