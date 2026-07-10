#!/bin/bash

SRC=ansui.c

if [ "$#" -eq 0 ]; then
	echo -e "Provide one demo from:"
	ls .
	exit 1
fi

OBJ=${1//.c/}

if [ -f "$OBJ" ]; then
	echo -e "Cleaning $OBJ"
	rm "$OBJ"
fi

if [ -f "$1" ]; then
	gcc "$1" "../src/$SRC" -lm -o "$OBJ"
else
	echo -e "$1 not found"
fi

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
