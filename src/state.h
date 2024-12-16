#include<stdint.h>
#include<raylib.h>
#include"chips.h"
#include"assets.h"

#ifndef __STATE_H
#define __STATE_H

#define CANVAS_WIDTH 1600
#define CANVAS_HEIGHT 1200
#define HALF_WIDTH (int)(CANVAS_WIDTH / 2)
#define HALF_HEIGHT (int)(CANVAS_HEIGHT / 2)
#define GRID_SIZE 128
#define DOLLARS_PER_MONEY_BLOCK_SIZE 2250

typedef enum {
	SCREEN_MENU,
	SCREEN_PREAMBLE,
	SCREEN_GAME,
	SCREEN_SLOTS,
	SCREEN_ENDING,
} Screen;

typedef enum {
	TILE_FLIP_HORIZONAL_FLAG = 1 << 0,
	TILE_FLIP_VERTICAL_FLAG  = 1 << 1,
	TILE_FLIP_DIAGONAL_FLAG  = 1 << 2,
} Tile_Flags;

typedef struct {
	Texture_Id id;
	uint8_t iter;
} Tile_Texture;

typedef struct {
	float master_volume;
} Settings;

#define MAX_TILES 2056
#define MAX_BLOCKS 256

typedef enum {
	BLOCK_DISABLED_FLAG = 1 << 0,
	BLOCK_INTERACT_FLAG = 1 << 1,
} Block_Flags;

typedef enum {
	BLOCK_TYPE_DUMB,
	BLOCK_TYPE_MONEY,
	BLOCK_TYPE_SLOT_MACHINE,
	BLOCK_TYPE_TELLER,
} Block_Type;

typedef struct {
	uint8_t size;
} Money_Block;

typedef struct {
	Texture_Id tex_id;
	uint8_t tex_iter;
} Slot_Machine_Block;

typedef union {
	Money_Block money;
	Slot_Machine_Block slots;
} Block_Data;

typedef struct {
	bool paused;
	Camera2D cam;
	struct {
		bool has_control;
		Vector2 pos, vel;
		int facing_x, facing_y;
	} player;
	struct {
		int count;
		int16_t grid_cells[MAX_BLOCKS*2];
		Block_Flags flags[MAX_BLOCKS];
		Block_Type types[MAX_BLOCKS];
		Block_Data data[MAX_BLOCKS];
	} blocks;
	struct {
		int count;
		int16_t grid_cells[MAX_TILES*2];
		Tile_Texture textures[MAX_TILES];
		Tile_Flags flags[MAX_TILES];
	} tiles;
	int results[3];
	int random_results[6];
	uint16_t chips_count[CHIP_COUNT];
	int dollars_held;
} Game_State;

extern Screen on_screen;
extern Settings settings;
extern Game_State *state;

void init_state(void);
int insert_tile(int grid_cell_x, int grid_cell_y, Tile_Texture tile, Tile_Flags flags);
int insert_block(int grid_cell_x, int grid_cell_y, Block_Type type, Block_Data data, Block_Flags flags);
int insert_dumb_block(int grid_cell_x, int grid_cell_y);
int insert_money_block(int grid_cell_x, int grid_cell_y, Money_Block data);
int insert_slot_machine_block(int grid_cell_x, int grid_cell_y, Slot_Machine_Block data);

void block_pos(int block_id, int *x, int *y);
void tile_pos(int tile_id, int *x, int *y);

#endif
