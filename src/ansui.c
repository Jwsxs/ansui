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

void* ansuiLoadDefaultConfig(ANSUI_LOAD_ATTR attr) {
	ANSUI_CONFIG_WINDOW* cfg = malloc(sizeof(ANSUI_CONFIG_WINDOW));

	switch (attr) {
		case ANSUI_LOAD_GLOBAL_ATTR:
			// return malloc(sizeof(ANSUI_CONFIG_GLOBAL));
		case ANSUI_LOAD_WINDOW_ATTR:
			cfg->x=0;
			cfg->y=0;
			cfg->w=25;
			cfg->h=10;
			cfg->c=' ';
			cfg->char_color=ANSUI_PIXEL_RESET_COLOR;
			cfg->bg_color=ANSUI_PIXEL_COLOR_BG_BRED;
			return cfg;
			break; // QT: Is this needed?

		case ANSUI_LOAD_ENTITY_ATTR:
			//return malloc(sizeof(ANSUI_CONFIG_ENTITY));
		case ANSUI_LOAD_WIDGET_ATTR:
			//return malloc(sizeof(ANSUI_CONFIG_WIDGET));
	}
}

// ===

void __ansuiResizeWinOutOfBorder(ANSUI_WIN* win) {
	if (win->w <= ws.ws_col && win->x + win->w > ws.ws_col) {
		win->x -= (win->x + win->w - ws.ws_col);
	}
	if (win->h <= ws.ws_row && win->y + win->h > ws.ws_row) {
		win->y -= (win->y + win->h - ws.ws_row);
	}
	if (win->w > ws.ws_col) {
		win->w = ws.ws_col;
		win->x = 0;
	}
	if (win->h > ws.ws_row) {
		win->h = ws.ws_row;
		win->y = 0;
	}
}

// HACK: BOTH OF THESE
void __ansuiFillPixelArray(ANSUI_WIN* win) {
	for (int i = 0; i < sizeof(ANSUI_PIXEL) * win->w * win->h; i+=sizeof(ANSUI_PIXEL)) {
		win->px[i].c = win->c;
		win->px[i].char_color = win->char_color;
		win->px[i].bg_color = win->bg_color;
	}
}

// TODO: Might look better to load into a cfg and just pass it as a whole on ansuiCreateWindow, but conceptually it's the same
// NOTE: Will be letting this one until some more advanced/complex stuff (for me) comes upon my mind and I have to remake it by a whole

// TODO: Understand what type of shit I've messed changing these config stuff
ANSUI_WIN* ansuiCreateWindow(ANSUI_CONFIG_WINDOW* cfg, ANSUI_WIN_FLAG flag) {
	ANSUI_WIN* win = cfg;

	win->px = malloc(sizeof(ANSUI_PIXEL) * win->w * win->h);
	__ansuiResizeWinOutOfBorder(win);
	__ansuiFillPixelArray(win);

	switch (flag) {
		/*
		default:
		case ANSUI_WIN_OPAQUE: // "blankness" set from here
			__ansuiFillPixelArray(win);
			break;
		case ANSUI_WIN_TRANSPARENT:
			break;
		*/
		case ANSUI_WIN_POS_CENTERED:
			win->x = (double)ws.ws_col / 2 - (double)win->w / 2;
			win->y = ws.ws_row / 2 - win->y / 2;
			break;
	}
	
	//free(cfg);
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
	free(win->px);
	free(win);
	return ANSUI_SUCCESS;
}

int ansuiQuit() {
	// Free memory got from ansuiInit();
	close(fd); // file descriptor
	
	return ANSUI_SUCCESS;
}

// === RENDER

// NOTE: Probably need of a custom made cursor movement
int ansuiRender(ANSUI_WIN* win) {
	// Basically all we need is a formated buffer and therefore a pointer to it
	int char_bufsz = win->w * win->h; // Must be +1 when sent to snprintf();
	int ansi_bufsz = char_bufsz * 18; // 9 => Max ANSII String Size => \e[0;107m for example, has 9 chars
	int bufsz = char_bufsz + ansi_bufsz + 1; // At the end, just append '\0', since buffer has to be detected as a string
						 // For later on fputs(buffer, stdout);

	// Buffer size must be sizeof(char) * bufsz + size of
	char* buffer = malloc(sizeof(char) * bufsz); // bo tá sendo aqui no tamanho

	int fb = 0;

	int i = 0;

	// DONE: Improve performance by writing to a single buffer and print it out later on
	// PERF: Probably faster, in need to make any fps counter haha
	for (int h = 0; h < win->h; h++) {
		fb += snprintf(buffer + fb, bufsz - fb, "\e[%d;%dH", win->y + h, win->x); // change cursor position | same as setting x and y
		for (int w = 0; w < win->w; w++) {
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
			fb += snprintf(buffer + fb, bufsz - fb, "%s%s%c", win->px[i].bg_color, win->px[i].char_color, win->px[i].c);
			i++;
		}
		fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	}

	fputs(buffer, stdout);
	//write(STDOUT_FILENO, buffer, bufsz);
	//printf("%s\033[1H", ANSUI_PIXEL_RESET_COLOR);

	free(buffer);
	return ANSUI_SUCCESS;
}

void ansuiClear() {
	printf("%s\033[2J\033[1H", glob_cfg.clear_color); // framebuffer size supports current a fixed size
}
