#!/bin/sh

DEB=" "

if [[ $1 -eq "D" ]]; then
	DEB="-D DEBUG"
fi

gcc -Wall -g -c $DEB snake.c
gcc -Wall -g -c $DEB mmenu.c
gcc -Wall -g -c $DEB main.c
gcc -lncurses snake.o main.o mmenu.o -o snake
