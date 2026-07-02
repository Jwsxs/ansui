#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "../include/tgui.h"

/*
 * AS A MONO-HEADER LIBRARY,
 == ALL INTERNAL FUNCTIONS STARTS WITH __
 == ALL FOR PUBLIC USE IS NORMAL, SELF-INDUCING
 == all functions start with tgui#();
*/

static TGUI_CONFIG glob_cfg;
static TGUI_CONFIG cfg;

#include <fcntl.h>
#include <unistd.h>
#include <err.h>

struct winsize ws;

static int fd;
static struct winsize __tguiGetTermSize() {
	struct winsize ws;
	fd = open("/dev/tty", O_RDWR);

	if (fd < 0 || ioctl(fd, TIOCGWINSZ, &ws)) err(8, "/dev/tty");
	return ws;
}

// We will have struct winsize at a global scope, outside this local function
// Therefore, we'd be returning a pointer to this struct
void* tguiInit(TGUI_FLAG flag) {
	// Initialization consists on retrieving terminal info, like TIOCGWINSZ
	// Getting to know $HOME, current $PATH, probably some user info and other variables
	switch (flag) {
		default:
		case TGUI_FLAG_NONE: // Normal default initialization
			ws = __tguiGetTermSize(); // This way we get info about ROWS, COLS
			return &ws;
			break;
		// In case passing, for example, any TGUI_FLAG that can change our terminal info
		// TODO: setTermSize() with TIOCSWIN
	}
}

// ===

void tguiSetGlobAttr(TGUI_GLOB_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_CLEAR_COLOR:
			glob_cfg.clear_color = va_arg(args, TGUI_PIXEL_COLOR);
			break;
	}

	va_end(args);
}

void tguiSetWinAttr(TGUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_PXA_COLOR:
			cfg.color = va_arg(args, TGUI_PIXEL_COLOR);
			break;
		case TGUI_ATTR_PXA_FILL_CHAR:
			cfg.fill_char = va_arg(args, int);
			break;

		case TGUI_ATTR_WIN_IS_OPAQUE:
			cfg.is_opaque = va_arg(args, int);
			break;
		case TGUI_ATTR_WIN_HAS_BORDER:
			cfg.has_border = va_arg(args, int);
			break;

		case TGUI_ATTR_WIN_POSITION_CENTERED:
			cfg.is_centered = va_arg(args, int);
			break;
	}

	va_end(args);
}

// TODO: Set entity (object) attributes for creation and appliance on the window
void tguiSetEntAttr(TGUI_ENT_ATTR attr, ...) {

}

// ===

static TGUI_CONFIG __tguiLoadDefaultConfig() {
	return (TGUI_CONFIG){
		.clear_color = TGUI_PIXEL_RESET_COLOR,
		.color = TGUI_PIXEL_COLOR_BG_BWHITE,
		.fill_char = ' ',
		.is_opaque = TGUI_TRUE,
		.has_border = TGUI_FALSE,
		.is_centered = TGUI_FALSE
	};
}

void __tguiResizeWinOutOfBorder(TGUI_WIN* win) {
	if (win->width <= ws.ws_col && win->x + win->width > ws.ws_col) {
		win->x -= (win->x + win->width - ws.ws_col);
	}
	if (win->height <= ws.ws_row && win->y + win->height > ws.ws_row) {
		win->y -= (win->y + win->height - ws.ws_row);
	}
	if (win->width > ws.ws_col) {
		win->width = ws.ws_col;
		win->x = 0;
	}
	if (win->height > ws.ws_row) {
		win->height = ws.ws_row;
		win->y = 0;
	}
}

void __tguiFillPixelCharArray(TGUI_WIN* win, char c) {
	for (int i = 0; i < win->pxa->size; i++) {
		win->pxa->px[i].c = c;
	}
}

void __tguiFillPixelColorArray(TGUI_WIN* win, TGUI_PIXEL_COLOR color) {
	for (int i = 0; i < win->pxa->size; i++) {
		win->pxa->px[i].color = color;
	}
}

int __tguiFillPixelArray(TGUI_WIN *win) {
	if (win->cfg.is_opaque == 0) { // then it's transparent
		return 1; // return true since pixel array could be filled succesfully
	}
	__tguiFillPixelCharArray(win, win->cfg.fill_char);
	__tguiFillPixelColorArray(win, win->cfg.color);

	return 1; // pixel array filled succesfully
}

// NOTE: Might look better to load into a cfg and just pass it as a whole on tguiCreateWindow, but conceptually it's the same
// NOTE: Will be letting this one until some more advanced/complex stuff (for me) comes upon my mind and I have to remake it by a whole
TGUI_WIN* tguiCreateWindow(int x, int y, int width, int height, TGUI_WIN_FLAG flag) {
	TGUI_WIN* win = malloc(sizeof(TGUI_WIN));
	win->width = width;
	win->height = height;

	if (cfg.is_centered == TGUI_TRUE) {
		win->x = ws.ws_col / 2 - win->width / 2;
		win->y = ws.ws_row / 2 - win->height / 2;
	} else {
		win->x = x;
		win->y = y;
	}
	__tguiResizeWinOutOfBorder(win);

	TGUI_PIXEL_ARRAY* pxa = malloc(sizeof(TGUI_PIXEL_ARRAY));
	win->pxa = pxa;

	TGUI_PIXEL* px = malloc(sizeof(TGUI_PIXEL) * width * height);
	pxa->px = px;
	pxa->size = width * height;

	win->cfg = cfg;

	switch (flag) {
		default:
		case TGUI_WIN_OPAQUE: // "blankness" set from here
			win->cfg.is_opaque = 1;
			__tguiFillPixelArray(win);
		break;
		case TGUI_WIN_TRANSPARENT:
			win->cfg.is_opaque = 0;
			__tguiFillPixelArray(win);
		break;
	}
	
	cfg = __tguiLoadDefaultConfig();
	return win;
}

int tguiUpdate(TGUI_WIN *win, TGUI_WIN_ATTR attr, ...) {
	va_list args;
	va_start(args, attr);

	switch (attr) {
		case TGUI_ATTR_PXA_COLOR:
			win->cfg.color = va_arg(args, TGUI_PIXEL_COLOR);
			__tguiFillPixelColorArray(win, win->cfg.color);
			break;
		case TGUI_ATTR_PXA_FILL_CHAR:
			win->cfg.fill_char = va_arg(args, int);
			__tguiFillPixelCharArray(win, win->cfg.fill_char);
			break;

		case TGUI_ATTR_WIN_IS_OPAQUE:
			//win->config.fill_char = ' ';
			win->cfg.is_opaque = va_arg(args, int);
			if (win->cfg.is_opaque == 1) {
				__tguiFillPixelColorArray(win, win->cfg.color);
				__tguiFillPixelCharArray(win, win->cfg.fill_char);
			}
		case TGUI_ATTR_WIN_HAS_BORDER:
			// not yet implemented
			break;

		case TGUI_ATTR_WIN_POSITION_CENTERED:
			if (va_arg(args, int) == TGUI_TRUE) {
				win->x = ws.ws_col - (win->width / 2);
				win->y = ws.ws_row - (win->height / 2);
			}
			break;
		default:
			break;
	}

	va_end(args);
	return TGUI_SUCCESS; // was able to succesfully update, but no error handling yet
}

int tguiWinDestroy(TGUI_WIN *win) {
	// Every malloc on *win is:
	free(win->pxa->px);
	free(win->pxa);
	free(win);
	return TGUI_SUCCESS;
}

int tguiQuit() {
	// Free memory got from tguiInit();
	close(fd); // file descriptor
	
	return TGUI_SUCCESS;
}

// === RENDER

// NOTE: Probably need of a custom made cursor movement
int tguiRender(TGUI_WIN* win) {
	// Basically all we need is a formated buffer and therefore a pointer to it
	int char_bufsz = win->width * win->height; // Must be +1 when sent to snprintf();
	int ansi_bufsz = char_bufsz * 9; // 9 => Max ANSII String Size => \e[0;107m for example, has 9 chars
	int bufsz = char_bufsz + ansi_bufsz + 1; // At the end, just append '\0', since buffer has to be detected as a string
						 // For later on fputs(buffer, stdout);

	// Buffer size must be sizeof(char) * bufsz + size of
	char* buffer = malloc(sizeof(char) * bufsz); // bo tá sendo aqui no tamanho

	int fb = 0;

	int i = 0;

	// DONE: Improve performance by writing to a single buffer and print it out later on
	// PERF: Probably faster, in need to make any fps counter haha
	for (int h = 0; h < win->height; h++) {
		fb += snprintf(buffer + fb, bufsz - fb, "\e[%d;%dH", win->y + h, win->x); // change cursor position | same as setting x and y
		for (int w = 0; w < win->width; w++) {
			// check if win->config.is_opaque == 0 and if win->pxa->px[i] isn't inside any entity / widget
			// if not, just move cursor pos to +1
			if (win->cfg.is_opaque != 0) {
				//printf("\033[%d;%dH", c_pos[0] + 1, c_pos[1]);
				fb += snprintf(buffer + fb, bufsz - fb, "%s%c", win->pxa->px[i].color, win->pxa->px[i].c);
			} else {
				fb += snprintf(buffer + fb, bufsz - fb, "%s", "\e[1C");
			}

			i++;
		}
		fb += snprintf(buffer + fb, bufsz - fb, "%s", "\n");
	}

	fputs(buffer, stdout);
	//write(STDOUT_FILENO, buffer, bufsz);
	//printf("%s\033[1H", TGUI_PIXEL_RESET_COLOR);

	free(buffer);
	return TGUI_SUCCESS;
}

void tguiClear() {
	printf("%s\033[2J\033[1H", glob_cfg.clear_color); // framebuffer size supports current a fixed size

