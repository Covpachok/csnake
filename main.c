#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "snake.h"
#include "sconst.h"

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

void quit()
{
	scr_rest();
	exit(0);
}

int main()
{
	WINDOW *mm_win;
#if 0
	MENU *main_menu;
	ITEM **mm_items;
	void (*mm_func[])(void) = { &snake_game, &quit };
#endif
	int scr_max_x, scr_max_y, key, n_choices, i;

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

	mm_win = newwin(mwin_h, mwin_w,
				(scr_max_y - mwin_h)/2,
				(scr_max_x - mwin_w)/2);
	box(mm_win, 0, 0);
	wrefresh(mm_win);

#if 0
	n_choices = sizeof(menu_titles)/sizeof(menu_titles[0]);
	mm_items = calloc(n_choices, sizeof(ITEM *));
	for(i = 0; i < n_choices; i++)
		mm_items[i] = new_item(menu_titles[i], "");

	main_menu = new_menu(mm_items);
	set_menu_win(main_menu, mm_win);

	set_menu_mark(main_menu, " > ");

	box(mm_win, 0, 0);

	post_menu(main_menu);
	wrefresh(mm_win);

	napms(2000);
#endif

	while((key = getch()) != key_escape) {
		if((key >= '0') && (key <= '9')) {
			snake_game(key-'0');
			break;
		}
	}
	
	scr_rest();
	return 0;
}
