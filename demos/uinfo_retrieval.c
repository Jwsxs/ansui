// Demo of retrieving info from terminal for use with our user
// For example, we can exchange memory for access of terminal width, terminal height
// Later on could be passed against ansuiCreateWindow() that's size / pos is bigger / outside of the user's view

#include "../include/ansui.h"

#include <stdio.h>

// We will need a winsize struct for our retrieval of terminal info
// It must be set globally
static struct winsize* ws;

int main() {
	// ansuiInit() returns the address of our info
	// Then we'd just retrieve this pointer
	void* uinfo = ansuiInit(ANSUI_FLAG_NONE);

	// Pass it onto our static ws set globally;
	ws = uinfo;

	// Later on, normal procedure: ws is a struct of:
	// - ws_col => total columns ( is updated accordingly on runtime )
	// - ws_rol => total rows ( is updated accordingly on runtime )
	// - x_pixel => i have no idea
	// - y_pixel => i have no idea
	printf("cols: %zu\nrows: %zu\n\nxpixel: %zu\nypixel: %zu\n", (size_t)ws->ws_col, (size_t)ws->ws_row, (size_t)ws->ws_xpixel, (size_t)ws->ws_ypixel);
}
