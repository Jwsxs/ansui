#!/bin/bash

SRC=ansui.c

if [ "$1" -eq ]; then
	echo -e "Provide one demo: \x[1m./demos/\[0m"
fi

OBJ=${1//.c/}
if [ -f "$1" ]; then
	gcc "$1" "../src/$SRC" -lm -o "$OBJ"
else
	echo -e "$1 not found"
fi

if [ -f "$OBJ" ]; then
	echo -e "Running \033[1m$1\033[0m"
	./"$OBJ"
fi
