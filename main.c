#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "snake.h"
#include "sconst.h"
#include "mmenu.h"

const char snake_title[] 	= "CSNAKE";
const char author[] 		= "Created by Heylc";

void write_game_title(int sm_h, int sm_w)
{
	int i;
	for(i = 0; i < sizeof(snake_title) - 1; i++) {
		init_pair(i+10, i+1, COLOR_BLACK);
		attrset(COLOR_PAIR(i+10) | A_BOLD | A_UNDERLINE);
		mvaddch((sm_h - gwin_h - 2)/2,
				(sm_w - sizeof(snake_title) + 1)/2 + i,
				snake_title[i]);
		attroff(COLOR_PAIR(i+10) | A_BOLD | A_UNDERLINE);
	}

	attrset(A_DIM);
	mvaddstr(sm_h-1, sm_w - sizeof(author), author);
	attroff(A_DIM);
}

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
	void (*mm_func[])(int) = { &snake_game, NULL, &quit };
	int sm_h, sm_w, key;

	scr_prep();

	start_color();
	getmaxyx(stdscr, sm_h, sm_w);

	/* check scr size */
	if((sm_h < smin_h) ||
			(sm_w < smin_w)) {
		scr_rest();
		fprintf(stderr, "ERR: Too small window size\n");
		return 1;
	}

#ifdef DEBUG
	mvwprintw(stdscr, 0, 0, "H:%03d W:%03d", sm_h, sm_w);
#endif
	write_game_title(sm_h, sm_w);
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
