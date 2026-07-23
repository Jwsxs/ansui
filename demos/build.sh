#!/bin/bash

SRC=ansui.c
ASM_SRC=ansuix86.s

if [ "$#" -eq 0 ]; then
	echo -e "Provide one demo from:"
	ls .
	exit 1
fi

if [ ! -f "$1" ]; then
	echo -e "$1 not found."
	exit 1
fi

OBJ=${1//.c/}

if [ -f "$OBJ" ]; then
	echo -e "Cleaning $OBJ"
	rm "$OBJ"
fi

gcc "$1" "../src/$SRC" "../src/$ASM_SRC" -lm -o "$OBJ" || exit 1

if [ -f "$OBJ" ]; then
	echo -e "Running \033[1m$1\033[0m"
	./"$OBJ"
fi

if [ "$#" -ne 2 ]; then
	rm ./"$OBJ"
else
	if [ "$2" = "-s" ] || [ "$2" = "--save" ]; then
		echo -e "Saving \e[1m$OBJ\e[0m"
	fi
fi
