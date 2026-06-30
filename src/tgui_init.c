#include "../include/tgui.h"

#include <fcntl.h>
#include <unistd.h>
#include <err.h>

struct winsize ws;

static struct winsize getTermSize() {
	struct winsize ws;
	int fd = open("/dev/tty", O_RDWR);

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
			ws = getTermSize(); // This way we get info about ROWS, COLS
			return &ws;
			break;
		// In case passing, for example, any TGUI_FLAG that can change our terminal info
		// TODO: setTermSize() with TIOCSWINSZ, notice the S there
	}
}
