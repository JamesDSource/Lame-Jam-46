#include<emscripten/emscripten.h>
#include<raylib.h>
#include<raymath.h>
#include"state.h"
#define RAYGUI_IMPLEMENTATION
#include"raygui.h"

const int CANVAS_WIDTH = 1920;
const int CANVAS_HEIGHT = 1080;
const int GRID_SIZE = 64;
const float PLAYER_SPEED = 3.0f;
const float PLAYER_ACCELERATION = 0.4;

void tick(void) {
	// Player movement
	switch(on_screen) {
	case SCREEN_MENU:
		break;
	case SCREEN_GAME:
		; Vector2 dir = Vector2Normalize((Vector2){
			IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
			IsKeyDown(KEY_S) - IsKeyDown(KEY_W),
		});
		Vector2 vel = Vector2Multiply(dir, (Vector2){PLAYER_SPEED, PLAYER_SPEED});
		state.player.pos = Vector2Add(state.player.pos, vel);
		state.cam.target = Vector2Add(
			state.cam.target,
			Vector2Divide(
				Vector2Subtract(state.player.pos, state.cam.target),
				(Vector2){10, 10}
			)
		);
		break;
	}

	// Draw
	BeginDrawing();
	switch(on_screen) {
		case SCREEN_MENU:
			break;
		case SCREEN_GAME:
			BeginMode2D(state.cam);
			ClearBackground(RAYWHITE);

			// Grid
			for(int i = 0; i < 128*128; ++i) {
				int x, y;
				y = (i/128)*64;
				x = (i%128)*GRID_SIZE;
				DrawLine(x, y, x + GRID_SIZE, y, BLACK);
				DrawLine(x, y, x, y + GRID_SIZE, BLACK);
			}

			// Player
			DrawCircle(state.player.pos.x, state.player.pos.y, 32, RED);
			EndMode2D();
			break;
	}
	EndDrawing();
}

int main(void) {
	on_screen = SCREEN_MENU;
	settings = (Settings){
		.master_volume = 1.0f,
	};
	state = (Game_State){
		.cam = (Camera2D){
			.offset.x = CANVAS_WIDTH/2,
			.offset.y = CANVAS_HEIGHT/2,
			.zoom = 1.0f,
		},
		.player.pos.x = 32.0f,
		.player.pos.y = 32.0f,
	};
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Lame Jam");
	emscripten_set_main_loop(tick, 60, 1);
	CloseWindow();
	return 0;
}
