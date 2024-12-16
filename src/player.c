#include <stdlib.h>
#include <raylib.h>
#include <raymath.h>
#include <stdio.h>
#include "player.h"
#include "state.h"
#include "collision.h"

#define BROAD_DETECT_SIZE 256
#define MAX_BROAD_BLOCKS 64

const float PLAYER_SPEED = 3.0f;
const float PLAYER_ACCELERATION = 0.4f;
const float PLAYER_DECELERATION = 1.0f;

static void cell_player_facing(int *cell_x, int *cell_y) {
	int at_x = (int)(floor(state->player.pos.x/GRID_SIZE));
	int at_y = (int)(floor(state->player.pos.y/GRID_SIZE));
	*cell_x = at_x + state->player.facing_x;
	*cell_y = at_y + state->player.facing_y;
}

void update_player(void) {
	Rectangle player_rect = {
		state->player.pos.x - 30.0f,
		state->player.pos.y - 50.0f,
		60.0f,
		60.0f
	};
	if(!state->paused && state->player.has_control) {
		int d_down = IsKeyDown(KEY_D);
		int a_down = IsKeyDown(KEY_A);
		int s_down = IsKeyDown(KEY_S);
		int w_down = IsKeyDown(KEY_W);
		Vector2 dir = {0, 0};
		if(d_down || a_down || s_down || w_down) {
			state->player.facing_x = (d_down > 0) - (a_down > 0);
			state->player.facing_y = (s_down > 0) - (w_down > 0);
			dir = Vector2Normalize((Vector2){
				state->player.facing_x,
				state->player.facing_y
			});
		}
		Vector2 target_vel = Vector2Multiply(dir, (Vector2){PLAYER_SPEED, PLAYER_SPEED});
		float acc = (target_vel.x == 0.0f && target_vel.y == 0.0f) ? PLAYER_DECELERATION : PLAYER_ACCELERATION;
		state->player.vel = Vector2MoveTowards(state->player.vel, target_vel, acc);
		state->player.pos = Vector2Add(state->player.pos, state->player.vel);
		state->cam.target = Vector2Add(
			state->cam.target,
			Vector2Divide(
				Vector2Subtract(state->player.pos, state->cam.target),
				(Vector2){20, 20}
			)
		);
		state->cam.target.x = floorf(state->cam.target.x);
		state->cam.target.y = floorf(state->cam.target.y);

		Rectangle broad_detection_rect = {
			state->player.pos.x - (float)(BROAD_DETECT_SIZE)/2,
			state->player.pos.y - (float)(BROAD_DETECT_SIZE)/2,
			BROAD_DETECT_SIZE,
			BROAD_DETECT_SIZE
		};
		int broad_blocks_count = 0;
		int broad_blocks[MAX_BROAD_BLOCKS];
		for(int i = 0; i < state->blocks.count; ++i) {
			if((state->blocks.flags[i] & BLOCK_DISABLED_FLAG) > 0) {
				continue;
			}
			int x, y;
			block_pos(i, &x, &y);
			Rectangle block_rect = { x, y, GRID_SIZE - 1, GRID_SIZE - 1 };
			if(box_box_collision(block_rect, broad_detection_rect, NULL)) {
				if(broad_blocks_count >= MAX_BROAD_BLOCKS) {
					printf("Ran out of space for broad phase!\n");
					break;
				}
				broad_blocks[broad_blocks_count] = i;
				broad_blocks_count++;
			}
		}
		int facing_x, facing_y;
		cell_player_facing(&facing_x, &facing_y);
		int hovering_block_id = -1;
		for(int i = 0; i < broad_blocks_count; ++i) {
			int index = broad_blocks[i];
			int cell_x = state->blocks.grid_cells[index*2 + 0];
			int cell_y = state->blocks.grid_cells[index*2 + 1];
			int x = cell_x*GRID_SIZE;
			int y = cell_y*GRID_SIZE;
			block_pos(index, &x, &y);
			Rectangle block_rect = { x, y, GRID_SIZE - 1, GRID_SIZE - 1 };
			Vector2 push_out;
			if(box_box_collision(block_rect, player_rect, &push_out)) {
				state->player.pos = Vector2Add(state->player.pos, push_out);
				if(push_out.x != 0.0f) {
					state->player.vel.x = 0.0f;
				}
				if(push_out.y != 0.0f) {
					state->player.vel.y = 0.0f;
				}
			}
			if(((state->blocks.flags[index] & BLOCK_INTERACT_FLAG) > 0) && cell_x == facing_x && cell_y == facing_y) {
				hovering_block_id = index;
			}
		}

		if(hovering_block_id > 0)  {
			if(IsKeyPressed(KEY_E)) {
				switch(state->blocks.types[hovering_block_id]) {
				case BLOCK_TYPE_DUMB: break;
				case BLOCK_TYPE_MONEY:
					; Money_Block *data = &state->blocks.data[hovering_block_id].money;
					data->size -= 1;
					if(data->size == 0) {
						state->blocks.flags[hovering_block_id] = BLOCK_DISABLED_FLAG;
					}
					state->dollars_held += DOLLARS_PER_MONEY_BLOCK_SIZE;
					break;
				case BLOCK_TYPE_SLOT_MACHINE:
					on_screen = SCREEN_SLOTS;
					break;
				case BLOCK_TYPE_TELLER:
					break;
				}
			}
			int prompt_x = facing_x*GRID_SIZE + (int)(GRID_SIZE/2);
			int prompt_y = facing_y*GRID_SIZE;
			Texture prompt_texture = lookup_texture(TEXTURE_PROMPT, 1);
			DrawTexture(
				prompt_texture,
				prompt_x - (int)(prompt_texture.width/2),
				prompt_y - (int)(prompt_texture.height/2),
				WHITE
			);
		}
	}

	Texture tex = lookup_texture(TEXTURE_PLAYER_FRONT, 1);
	DrawTexture(tex, state->player.pos.x - (int)(tex.width/2), state->player.pos.y - tex.width + 10.0f, WHITE);
}
