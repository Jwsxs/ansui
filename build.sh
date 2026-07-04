#!/bin/bash

SRC=ansui.c

OBJ=demo.o

if [ "$1" -eq ]; then
	echo -e "Provide one demo: \x[1m./demos/\[0m"
fi

if [ -f "$1" ]; then
	gcc "$1" "./src/$SRC" -o $OBJ
else
	echo -e "$1 not found"
fi

if [ -f "$OBJ" ]; then
	echo -e "Running \033[1m$1\033[0m"
	./"$OBJ"

	rm ./"$OBJ"
fi
