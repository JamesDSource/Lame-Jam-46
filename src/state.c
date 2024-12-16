#define CUTE_TILED_NO_EXTERNAL_TILESET_WARNING
#define CUTE_TILED_IMPLEMENTATION

#include <assert.h>
#include <string.h>
#include "state.h"
#include "cute_tiled.h"

Screen on_screen = SCREEN_MENU;
Settings settings = {
	.master_volume = 1.0f
};
Game_State *state = NULL;

const Tile_Texture TEX_FROM_TILE_ID[21] = {
	[0] = (Tile_Texture){TEXTURE_FLOOR_TILE, 1},
	[1] = (Tile_Texture){TEXTURE_FLOOR_TILE, 2},
	[2] = (Tile_Texture){TEXTURE_WALL_TILE, 1},
	[3] = (Tile_Texture){TEXTURE_WALL_TILE, 2},
	[4] = (Tile_Texture){TEXTURE_WALL_TILE, 3},
	[5] = (Tile_Texture){TEXTURE_WALL_TILE, 4},
	[6] = (Tile_Texture){TEXTURE_WALL_TILE, 5},
	[13] = (Tile_Texture){TEXTURE_GATE, 1},
	[14] = (Tile_Texture){TEXTURE_GATE, 2},
	[15] = (Tile_Texture){TEXTURE_GATE, 3},
	[16] = (Tile_Texture){TEXTURE_GATE, 4},
	[17] = (Tile_Texture){TEXTURE_GATE, 5},
	[18] = (Tile_Texture){TEXTURE_GATE, 6},
	[19] = (Tile_Texture){TEXTURE_STREET_TILE, 1},
};

void init_state(void) {
	state = malloc(sizeof(Game_State));
	assert(state != NULL);
	*state = (Game_State){
		.paused = false,
		.player.has_control = true,
		.cam = {
			.offset.x = HALF_WIDTH,
			.offset.y = HALF_HEIGHT,
			.zoom = 1.0f,
		},
		.blocks.count = 0,
		.tiles.count = 0,
	};
	cute_tiled_map_t *map = cute_tiled_load_map_from_file("res/casino.json", NULL);
	cute_tiled_layer_t *layer = map->layers;
	while(layer) {
		if(strncmp(layer->name.ptr, "Objects", 10) == 0) {
			cute_tiled_object_t *obj = layer->objects;
			while(obj) {
				if(strncmp(obj->name.ptr, "PlayerSpawn", 16) == 0) {
					state->player.pos.x = obj->x;
					state->player.pos.y = obj->y;
					state->cam.target.x = obj->x;
					state->cam.target.y = obj->y;
				}
				obj = obj->next;
			}
			layer = layer->next;
			continue;
		}

		bool is_blocks = strncmp(layer->name.ptr, "Blocks", 10) == 0;
		bool is_wall = strncmp(layer->name.ptr, "Walls", 10) == 0;
		for(int i = 0; i < layer->data_count; ++i) {
			// Get tile pos
			int tile_id = layer->data[i];
			if(tile_id == 0) {
				continue;
			}
			int x_pos, y_pos;
			y_pos = i/(map->width);
			x_pos = i%(map->width);

			if(is_blocks) {
				tile_id = cute_tiled_unset_flags(tile_id);
				if(tile_id == 8 || tile_id == 9 || tile_id == 10) {
					Slot_Machine_Block data = {};
					switch(tile_id) {
					case 8:
						data.tex_id = TEXTURE_SLOT_FRONT;
						data.tex_iter = 1;
						break;
					case 9:
						data.tex_id = TEXTURE_SLOT_SIDE;
						data.tex_iter = 1;
						break;
					case 10:
						data.tex_id = TEXTURE_SLOT_SIDE;
						data.tex_iter = 2;
						break;
					}
					insert_slot_machine_block(x_pos, y_pos, data);
				} else if(tile_id == 11) {
					insert_money_block(x_pos, y_pos, (Money_Block){2});
				}
			} else {
				// Get tile flags
				int h_flip, v_flip, d_flip;
				cute_tiled_get_flags(tile_id, &h_flip, &v_flip, &d_flip);
				Tile_Flags flags = 0;
				if(h_flip) {
					flags |= TILE_FLIP_HORIZONAL_FLAG;
				}
				if(v_flip) {
					flags |= TILE_FLIP_VERTICAL_FLAG;
				}
				if(d_flip) {
					flags |= TILE_FLIP_DIAGONAL_FLAG;
				}

				// Get tile texture
				tile_id = cute_tiled_unset_flags(tile_id);
				Tile_Texture tex = TEX_FROM_TILE_ID[tile_id - 1];

				insert_tile(x_pos, y_pos, tex, flags);
				if(is_wall) {
					insert_dumb_block(x_pos, y_pos);
				}
			}
		}
		layer = layer->next;
	}
	for(int i = 0; i < 6; ++i) {
		state->random_results[i] = GetRandomValue(1, 4);
	}
}

int insert_tile(int grid_cell_x, int grid_cell_y, Tile_Texture tex, Tile_Flags flags) {
	assert(state->tiles.count < MAX_TILES);
	int i = state->tiles.count;
	state->tiles.grid_cells[i*2 + 0] = grid_cell_x;
	state->tiles.grid_cells[i*2 + 1] = grid_cell_y;
	state->tiles.textures[i] = tex;
	state->tiles.flags[i] = flags;
	state->tiles.count++;
	return i;
}

int insert_block(int grid_cell_x, int grid_cell_y, Block_Type type, Block_Data data, Block_Flags flags) {
	assert(state->blocks.count < MAX_BLOCKS);
	int i = state->blocks.count;
	state->blocks.grid_cells[i*2 + 0] = grid_cell_x;
	state->blocks.grid_cells[i*2 + 1] = grid_cell_y;
	state->blocks.flags[i] = flags;
	state->blocks.types[i] = type;
	state->blocks.data[i] = data;
	state->blocks.count++;
	return i;
}

int insert_dumb_block(int grid_cell_x, int grid_cell_y) {
	return insert_block(grid_cell_x, grid_cell_y, BLOCK_TYPE_DUMB, (Block_Data){}, 0);
}

int insert_money_block(int grid_cell_x, int grid_cell_y, Money_Block data) {
	return insert_block(
		grid_cell_x,
		grid_cell_y,
		BLOCK_TYPE_MONEY,
		(Block_Data){.money = data},
		BLOCK_INTERACT_FLAG
	);
}

int insert_slot_machine_block(int grid_cell_x, int grid_cell_y, Slot_Machine_Block data) {
	return insert_block(
		grid_cell_x,
		grid_cell_y,
		BLOCK_TYPE_SLOT_MACHINE,
		(Block_Data){.slots = data},
		BLOCK_INTERACT_FLAG
	);
}

void block_pos(int block_id, int *x, int *y) {
	*x = state->blocks.grid_cells[block_id*2 + 0]*GRID_SIZE;
	*y = state->blocks.grid_cells[block_id*2 + 1]*GRID_SIZE;
};

void tile_pos(int tile_id, int *x, int *y) {
	*x = state->tiles.grid_cells[tile_id*2 + 0] * GRID_SIZE;
	*y = state->tiles.grid_cells[tile_id*2 + 1] * GRID_SIZE;
};
