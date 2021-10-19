#!/bin/sh

GUBED=" "

if [[ $1 == "DEBUG" ]]; then
	GUBED="-D DEBUG"
fi

gcc -Wall -g -c $GUBED snake.c
gcc -Wall -g -c $GUBED mmenu.c
gcc -Wall -g -c $GUBED main.c
gcc -lncurses snake.o main.o mmenu.o -o snake
