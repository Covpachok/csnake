#include <stdlib.h>
#include <ncurses.h>

#include "sconst.h"
#include "mmenu.h"

enum {
	titles_clrpid 	= 1,
	titles_count 	= 3,
	title_len 		= 8
};

const char *menu_titles[titles_count] = {
	"< PLAY >",
	"< LVLS >",
	"< EXIT >"
};

static void select_title(WINDOW *win, int num, attr_t attr)
{
	mvwchgat(win, (num*3)+2,
			(mwin_w - title_len)/2,
			title_len, A_BOLD | attr,
			titles_clrpid, NULL);
}

static void write_menu_titles(WINDOW *win)
{
	int i;
	for(i = 0; i < titles_count; i++) {
		wattrset(win, COLOR_PAIR(titles_clrpid) | A_BOLD);
		mvwaddstr(win, (i*3)+2,
				(mwin_w - title_len)/2,
				menu_titles[i]);
		wattroff(win, COLOR_PAIR(titles_clrpid) | A_BOLD);
	}
	wrefresh(win);
}

static void erase_menu_titles(WINDOW *win)
{
	int i;
	for(i = 0; i < titles_count; i++) {
		mvwaddstr(win, (i*3)+2,
				(mwin_w - title_len)/2,
				"        ");
	}
	wrefresh(win);
}

static void write_lvl_id(WINDOW *win, int lvl_id)
{
	char idch = '0' + lvl_id;
	wattrset(win, COLOR_PAIR(titles_clrpid) | A_BOLD);
	mvwaddch(win, (1*3)+2,
			(mwin_w + title_len)/2 + 1,
			idch);
	wattroff(win, COLOR_PAIR(titles_clrpid) | A_BOLD);
}

static void mm_win_init(WINDOW **win, int sm_y, int sm_x)
{
	*win = newwin(mwin_h, mwin_w,
			(sm_y - mwin_h)/2,
			(sm_x - mwin_w)/2);
	box(*win, 0, 0);
}

static void mm_win_delete(WINDOW **win)
{
	wborder(*win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	wrefresh(*win);
	delwin(*win);
}

void main_menu(void (*mm_func[])(int))
{
	WINDOW *mmw;
	int sm_y, sm_x, key, cur_pos = 0, lvl_id = 0;
	int func_params[] = { lvl_id, 0, 0 };

	timeout(100);
	getmaxyx(stdscr, sm_y, sm_x);

	init_pair(titles_clrpid, COLOR_YELLOW, COLOR_BLACK);

	while(true) {
		mm_win_init(&mmw, sm_y, sm_x);
		write_menu_titles(mmw);

		while((key = getch())) {
			switch(key) {
				case KEY_UP: 
					select_title(mmw, cur_pos, A_NORMAL);
					cur_pos--;
					if(cur_pos < 0)
						cur_pos = 2;
					break;
				case KEY_DOWN:
					select_title(mmw, cur_pos, A_NORMAL);
					cur_pos++;
					if(cur_pos > 2)
						cur_pos = 0;
					break;
				case KEY_RIGHT:
					if(cur_pos != 1)
						break;
					(func_params[0]) += 1;
					if(func_params[0] > 9)
						func_params[0] = 0;
					break;
				case KEY_LEFT:
					if(cur_pos != 1)
						break;
					(func_params[0]) -= 1;
					if(func_params[0] < 0)
						func_params[0] = 9;
					break;
				case key_enter:
					if(cur_pos != 1)
						goto run_func;
					break;
			}
			write_lvl_id(mmw, func_params[0]);
			select_title(mmw, cur_pos, A_REVERSE);
			wrefresh(mmw);
		}
run_func:
		erase_menu_titles(mmw);
		mm_win_delete(&mmw);

		(mm_func[cur_pos])(func_params[cur_pos]);
	}
}


