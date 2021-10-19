#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "snake.h"
#include "sconst.h"
#include "mmenu.h"

void scr_prep()
{
	initscr();
	cbreak();
	keypad(stdscr, 1);
	noecho();
	curs_set(0);
}

void scr_rest()
{
	curs_set(1);
	echo();
	nocbreak();
	endwin();
}

void quit(int extc)
{
	scr_rest();
	exit(extc);
}

int main()
{
	void (*mm_func[])(int) = { &snake_game, &snake_game, &quit };
	int scr_max_x, scr_max_y, key;

	scr_prep();

	start_color();
	getmaxyx(stdscr, scr_max_y, scr_max_x);

	/* check scr size */
	if((scr_max_y < smin_h) ||
			(scr_max_x < smin_w)) {
		scr_rest();
		fprintf(stderr, "ERR: Too small window size\n");
		return 1;
	}

#ifdef DEBUG
	mvwprintw(stdscr, 0, 0, "Y:%03d X:%03d", scr_max_y, scr_max_x);
#endif
	refresh();

	main_menu(mm_func);
	refresh();

	while((key = getch()) != key_escape) {
		if((key >= '0') && (key <= '9')) {
			snake_game(key-'0');
			break;
		}
	}
	
	scr_rest();
	return 0;
}
