#ifndef CONST_SIZES_H_SENTRY
#define CONST_SIZES_H_SENTRY

enum sizes {
	// main game win
	gwin_h = 15,
	gwin_w = 40,
	// game field
	gfld_h = gwin_h - 2,
	gfld_w = gwin_w - 2,
	// main menu
	mwin_h = gwin_h - 4,
	mwin_w = gwin_w - 20,
	// terminal screen min 
	smin_h = gwin_h + 4,
	smin_w = gwin_w + 4,
};

enum {
	key_escape 	= 27,
	key_enter 	= 10
};

#endif
