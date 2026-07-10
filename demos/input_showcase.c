#include "../include/ansui.h"

#include <stdio.h>

int main() {
	// Initialize flagging that we want to receive input
	ansuiInit(ANSUI_FLAG_INPUT);
	
	printf("AAA\n");

	ansuiQuit();
}
