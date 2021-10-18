#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

#include "levels.h"
#include "sconst.h"

#include "snake.h"

typedef struct {
	int x, y;
} crdt;

typedef struct {
	crdt *pos;
	int len, dx, dy;
} snkt;

enum color_pair_ids {
	sn_clrpid 	= 1,
	fd_clrpid	= 2,
	gf_clrpid  	= 3,
};

enum {
	min_len_to_pass 	= 75,
	default_snake_speed = 300,
};

const char snake_body_char 	= 'o';
const char snake_head_char 	= '@';
const char food_char 		= '&';
const char winning_msg[] 	= "YOU WIN";
const char losing_msg[]  	= "YOU LOSE";
const char *menu_titles[] 	= { "PLAY", "EXIT" };

static int is_snake_crashed(snkt snk, int lvl_id)
{
	int i, j, ret = 0;

	ret += (lvl_shapes[lvl_id][snk.pos[0].y-1][snk.pos[0].x-1] == '#');

	for(i = 1; i < snk.len; i++) {
		ret += 	(snk.pos[0].x == snk.pos[i].x) &&
				(snk.pos[0].y == snk.pos[i].y);
	}

#if 0
	ret += ((snk.pos[0].x >= gwin_w-1) ||
			(snk.pos[0].y >= gwin_h-1) || 
			(snk.pos[0].x < 1) || (snk.pos[0].y < 1));
#endif

	return ret;
}

static void add_snake_tail(snkt *snk)
{
	snk->len++;
	snk->pos = realloc(snk->pos, sizeof(crdt)*(snk->len));
	snk->pos[snk->len-1].x = snk->pos[snk->len-2].x;
	snk->pos[snk->len-1].y = snk->pos[snk->len-2].y;
}

static void free_snake(snkt *snk)
{
	free(snk->pos);
	snk->pos = NULL;
}

static void snake_movement(WINDOW *win, snkt *snk)
{
	int i;
	wattrset(win, COLOR_PAIR(sn_clrpid) | A_BOLD);

	mvwaddch(win, snk->pos[snk->len-1].y, snk->pos[snk->len-1].x, ' ');

	for(i = snk->len-1; i > 0; i--) {
		snk->pos[i].x = snk->pos[i-1].x;
		snk->pos[i].y = snk->pos[i-1].y;
	}
	mvwaddch(win, snk->pos[1].y, snk->pos[1].x, snake_body_char);

	snk->pos[0].x += snk->dx;
	snk->pos[0].y += snk->dy;

	/* snake teleportation on the other side of the field */
	if(snk->pos[0].x < 1)
		snk->pos[0].x = gwin_w-2;
	else
	if(snk->pos[0].x > gwin_w-2)
		snk->pos[0].x = 1;

	if(snk->pos[0].y < 1)
		snk->pos[0].y = gwin_h-2;
	else
	if(snk->pos[0].y > gwin_h-2)
		snk->pos[0].y = 1;

	mvwaddch(win, snk->pos[0].y, snk->pos[0].x, snake_head_char);

	wattroff(win, COLOR_PAIR(sn_clrpid) | A_BOLD);
}

static int is_snake_on_food(crdt food, int x, int y)
{
	return (food.x == x) && (food.y == y);
}

static void spawn_food(WINDOW *win, crdt *food, snkt snk, int lvl_id)
{
	int x, y, flag, i;
	wattrset(win, COLOR_PAIR(fd_clrpid) | A_BOLD);
	/* 0 and game_win_size coordinates is border */
	/* food can't spawn on border */
	flag = 1;
	while(flag) {
		flag = 0;
		y = 1+rand()%(gwin_h-2); 
		x = 1+rand()%(gwin_w-2);
		for(i = 0; i < snk.len; i++)
			flag += (snk.pos[i].y == y) && (snk.pos[i].x == x);
		flag += (lvl_shapes[lvl_id][y-1][x-1] == '#');
	}
	mvwaddch(win, y, x, food_char);
#ifdef DEBUG
	mvprintw(0, 12, "y=%03d x=%03d", y, x);
#endif

	food->x = x;
	food->y = y;
	wattroff(win, COLOR_PAIR(fd_clrpid) | A_BOLD);
}

void snake_init(WINDOW *win, snkt *snk) 
{
	snk->dx  = -1;
	snk->dy  = 0;
	snk->len = 2;

	snk->pos = malloc(sizeof(crdt));

	wattrset(win, COLOR_PAIR(sn_clrpid) | A_BOLD);

	snk->pos[0].x = gwin_w/2;
	snk->pos[0].y = gwin_h/2;
	mvwaddch(win, snk->pos[0].y, snk->pos[0].x, snake_head_char);

	snk->pos[1].x = 1+gwin_w/2;
	snk->pos[1].y = gwin_h/2;
	mvwaddch(win, snk->pos[1].y, snk->pos[1].x, snake_body_char);
	wrefresh(win);

	wattroff(win, COLOR_PAIR(sn_clrpid) | A_BOLD);
}

void choose_direction(int *dy, int *dx, int key)
{
	switch(key) {
		case KEY_UP:
			/* removes the possibility of moving 
			 * in opposite of current direction */
			if(*dy == 0) {
				*dy = -1;
				*dx = 0;
			}
			break;
		case KEY_DOWN:
			if(*dy == 0) {
				*dy = 1;
				*dx = 0;
			}
			break;
		case KEY_LEFT:
			if(*dx == 0) {
				*dy = 0;
				*dx = -1;
			}
			break;
		case KEY_RIGHT:
			if(*dx == 0) {
				*dy = 0;
				*dx = 1;
			}
			break;
	}
}

static void init_gamefield(WINDOW *win, int lvl_id)
{
	int i, j;
	wattrset(win, COLOR_PAIR(gf_clrpid) | A_BOLD);
	for(i = 0; i < lvl_cols; i++) {
		for(j = 0; j < lvl_rows; j++) {
			if(lvl_shapes[lvl_id][i][j] == '#')
				mvwaddch(win, i+1, j+1, '#');
		}
	}
	wattroff(win, COLOR_PAIR(gf_clrpid) | A_BOLD);
}

static void init_colors()
{
	init_pair(sn_clrpid, COLOR_GREEN  	, COLOR_BLACK);
	init_pair(fd_clrpid, COLOR_YELLOW 	, COLOR_BLACK);
	init_pair(gf_clrpid, COLOR_BLUE 	, COLOR_BLACK);
}

void snake_game(int lvl_id)
{
	WINDOW *game_win;
	int key, scr_max_y, scr_max_x;
	crdt food;
	snkt snake;

	srand(time(NULL));
	timeout(default_snake_speed);

	/* game window initialization */
	getmaxyx(stdscr, scr_max_y, scr_max_x);
	game_win = newwin(gwin_h, gwin_w,
				(scr_max_y - gwin_h)/2,
				(scr_max_x - gwin_w)/2);
	box(game_win, 0, 0);

	init_colors();

	snake_init(game_win, &snake);
	spawn_food(game_win, &food, snake, lvl_id);
	init_gamefield(game_win, lvl_id);
	wrefresh(game_win);

	while((key = getch())) {
		if(key == key_escape)
			goto snake_exit;
#ifdef DEBUG
		else if(key == '+')
			add_snake_tail(&snake);
#endif
		if(is_snake_on_food(food, snake.pos[0].x, snake.pos[0].y)) {
			add_snake_tail(&snake);
			spawn_food(game_win, &food, snake, lvl_id);
			wrefresh(game_win);
#ifdef DEBUG
			mvprintw(1, 12, "len = %03d", snake.len);
#endif
		}

		choose_direction(&(snake.dy), &(snake.dx), key);
		snake_movement(game_win, &snake);

		if(is_snake_crashed(snake, lvl_id))
			break;

#ifdef DEBUG
		for(int i = 0; i < snake.len; i++) {
			mvprintw(i+1, 0, "[%d] %02d, %02d",
					i+1, snake.pos[i].x, snake.pos[i].y);
		}
#endif
		wrefresh(game_win);
	}
	
	if(snake.len >= min_len_to_pass) {
		mvprintw((scr_max_y - gwin_h)/2,
				(scr_max_x - sizeof(winning_msg))/2,
				winning_msg);
	} else {
		mvprintw((scr_max_y - gwin_h)/2,
				(scr_max_x - sizeof(losing_msg))/2,
				losing_msg);
	}

	refresh();
	napms(2000);

snake_exit:
	notimeout(game_win, 1);
	free_snake(&snake);
}
