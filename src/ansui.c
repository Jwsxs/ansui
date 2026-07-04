#include <cstdarg>
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

// We will have struct winsize at a global scope, outside this local function
// Therefore, we'd be returning a pointer to this struct
void* ansuiInit(ANSUI_FLAG flag) {
	// Initialization consists on retrieving terminal info, like TIOCGWINSZ
	// Getting to know $HOME, current $PATH, probably some user info and other variables
	switch (flag) {
		default:
		case ANSUI_FLAG_NONE: // Normal default initialization
			ws = __ansuiGetTermSize(); // This way we get info about ROWS, COLS
			return &ws;
			break;
		// In case passing, for example, any ANSUI_FLAG that can change our terminal info
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

		case ANSUI_LOAD_ENTITY_ATTR:
			//return malloc(sizeof(ANSUI_CONFIG_ENTITY));
		case ANSUI_LOAD_WIDGET_ATTR:
			//return malloc(sizeof(ANSUI_CONFIG_WIDGET));
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


/*
int ansuiUpdate(ANSUI_WIN *win, ANSUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case ANSUI_ATTR_PXA_COLOR:
			win->cfg.color = va_arg(args, ANSUI_PIXEL_COLOR);
			__ansuiFillPixelColorArray(win, win->cfg.color);
			break;
		case ANSUI_ATTR_PXA_FILL_CHAR:
			win->cfg.fill_char = va_arg(args, int);
			__ansuiFillPixelCharArray(win, win->cfg.fill_char);
			break;

		case ANSUI_ATTR_WIN_IS_OPAQUE:
			//win->config.fill_char = ' ';
			win->cfg.is_opaque = va_arg(args, int);
			if (win->cfg.is_opaque == 1) {
				__ansuiFillPixelColorArray(win, win->cfg.color);
				__ansuiFillPixelCharArray(win, win->cfg.fill_char);
			}
		case ANSUI_ATTR_WIN_HAS_BORDER:
			// not yet implemented
			break;

		case ANSUI_ATTR_WIN_POSITION_CENTERED:
			if (va_arg(args, int) == ANSUI_TRUE) {
				win->x = ws.ws_col - (win->width / 2);
				win->y = ws.ws_row - (win->height / 2);
			}
			break;
		default:
			break;
	}

	va_end(args);
	return ANSUI_SUCCESS; // was able to succesfully update, but no error handling yet
}
*/

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

// NOTE: Probably need of a custom made cursor movement
int ansuiRender(ANSUI_WIN* win) {
	// Basically all we need is a formated buffer and therefore a pointer to it
	int char_bufsz = win->cfg->w * win->cfg->h; // Must be +1 when sent to snprintf();
	int ansi_bufsz = char_bufsz * 18; // 9 => Max ANSII String Size => \e[0;107m for example, has 9 chars
	int bufsz = char_bufsz + ansi_bufsz + 1; // At the end, just append '\0', since buffer has to be detected as a string
						 // For later on fputs(buffer, stdout);

	// Buffer size must be sizeof(char) * bufsz + size of
	char* buffer = malloc(sizeof(char) * bufsz); // bo tá sendo aqui no tamanho

	int fb = 0;

	int i = 0;

	// DONE: Improve performance by writing to a single buffer and print it out later on
	// PERF: Probably faster, in need to make any fps counter haha
	for (int h = 0; h < win->cfg->h; h++) {
		fb += snprintf(buffer + fb, bufsz - fb, "\e[%d;%dH", win->cfg->y + h, win->cfg->x); // change cursor position | same as setting x and y
		for (int w = 0; w < win->cfg->w; w++) {
			// check if win->config.is_opaque == 0 and if win->pxa->px[i] isn't inside any entity / widget
			// if not, just move cursor pos to +1
			// HACK: DIRECTLY PRINTING IT JUST TO SEE IF IT WORKS
			/*
			if (win->cfg.is_opaque != 0) {
				//printf("\033[%d;%dH", c_pos[0] + 1, c_pos[1]);
				fb += snprintf(buffer + fb, bufsz - fb, "%s%c", win->pxa->px[i].color, win->pxa->px[i].c);
			} else {
				fb += snprintf(buffer + fb, bufsz - fb, "%s", "\e[1C");
			}
			*/
			fb += snprintf(buffer + fb, bufsz - fb, "\033[%dm\033[%dm%c", win->pxa[i].bg_color, win->pxa[i].char_color, win->pxa[i].c);
			i++;
		}
		fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	}

	fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	fputs(buffer, stdout);
	//write(STDOUT_FILENO, buffer, bufsz);
	//printf("%s\033[1H", ANSUI_PIXEL_RESET_COLOR);

	free(buffer);
	return ANSUI_SUCCESS;
}

void __ansuiTermClear(ANSUI_PIXEL_BG_COLOR _color) {
	printf("\033[%dm\033[2J\033[1H", _color);
}

static inline void __ansuiTermClearDef(void) {
	__ansuiTermClear(BG_RESET);
}
