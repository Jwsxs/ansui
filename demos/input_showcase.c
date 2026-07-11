// Example of receiving inputs from terminal
#include "../include/ansui.h"

#include <stdio.h>
#include <unistd.h>

int main() {
	// Initialize with input receival flag
	ansuiInit(ANSUI_FLAG_INPUT);

	int last_ch;
	// Quick hack for infinite loop
	int running = 1;
	while (running) {
		// We retrieve the int for the key pressed (according to ASCII table)
		int ch = ansuiGetKey();

		// The keys are based on input signals, not physical keys pressed ( as modern media sample libraries works with )
		// That is because I have no idea on how to implement physical keys

		if (ch == ANSUI_KEY_Q) running = 0;

		// Print out ch for your understanding
		if (last_ch != ch)
			printf("%d\t%c\n", ch, ch);
		last_ch = ch;
	}

	ansuiQuit();
}
