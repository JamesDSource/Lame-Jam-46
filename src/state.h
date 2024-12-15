#include<raylib.h>
#include"chips.h"

#ifndef __STATE_H
#define __STATE_H

typedef enum {
	SCREEN_MENU,
	SCREEN_PREAMBLE,
	SCREEN_GAME,
} Screen;

typedef struct {
	float master_volume;
} Settings;

#define MAX_MONEY_STACKS 64

typedef struct {
	bool paused;
	Camera2D cam;
	struct {
		Vector2 pos;
		Vector2 vel;
	} player;
	struct {
		int count;
		int grid_cells[MAX_MONEY_STACKS*2]; // { x, y, x, y, x, y, ... }
		int sizes[MAX_MONEY_STACKS];
	} money_stacks;
	int chips_count[CHIP_COUNT];
} Game_State;

extern Screen on_screen;
extern Settings settings;
extern Game_State state;

#endif
