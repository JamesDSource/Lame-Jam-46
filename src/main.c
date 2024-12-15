#include "state.h"
#include <emscripten/emscripten.h>
#include <raylib.h>
#include <raymath.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"
#include "chips.h"

const int CANVAS_WIDTH = 1920;
const int CANVAS_HEIGHT = 1080;
const int GRID_SIZE = 128;
const float PLAYER_SPEED = 3.0f;
const float PLAYER_ACCELERATION = 0.4;

#define HALF_WIDTH (int)(CANVAS_WIDTH / 2)
#define HALF_HEIGHT (int)(CANVAS_HEIGHT / 2)

typedef enum {
	MENU_TYPE_MAIN,
	MENU_TYPE_PAUSE,
} Menu_Type;

Screen on_screen = SCREEN_MENU;
Settings settings = {
	.master_volume = 1.0f
};
Game_State state = {
	.paused = false,
	.cam = {
		.offset.x = HALF_WIDTH,
		.offset.y = HALF_HEIGHT,
		.zoom = 1.0f,
	},
	.player.pos.x = 32.0f,
	.player.pos.y = 32.0f,
	.money_stacks = {
		.count = 1,
		.grid_cells[0] = 1, // x
		.grid_cells[1] = 1, // y
		.sizes[0] = 3,
	},
};

void menu(Menu_Type type) {
	const char *title;
	switch (type) {
	case MENU_TYPE_MAIN:
		title = "Untitled Lame Game";
		break;
	case MENU_TYPE_PAUSE:
		title = "Paused";
		break;
	}
	Vector2 center = {HALF_WIDTH, HALF_HEIGHT};
	int panel_w = 264;
	int button_w = 164;
	GuiPanel(
		(Rectangle){
			center.x - (int)(panel_w / 2),
			center.y - 152,
			panel_w, 
			336
		},
		title
	);

	// Main button
	Rectangle button_rect = {center.x - (int)(button_w / 2), center.y - 24, button_w, 48};
	switch (type) {
	case MENU_TYPE_MAIN:
		if (GuiButton(button_rect, "Start Game")) {
			on_screen = SCREEN_PREAMBLE;
		}
		break;
	case MENU_TYPE_PAUSE:
		if (GuiButton(button_rect, "Resume")) {
			state.paused = false;
		}
		break;
	}

	// Volume
	char vol_amount_buf[16];
	snprintf(vol_amount_buf, 16, "[%.2f]", settings.master_volume);
	if (GuiSliderBar(
		(Rectangle){center.x - (int)(button_w / 2), center.y + 40, button_w, 48},
		"Volume",
		vol_amount_buf,
		&settings.master_volume, 0.0f, 1.0f)
	) {
		SetMasterVolume(settings.master_volume);
	}
}

void tick(void) {
	// Player movement
	switch (on_screen) {
	case SCREEN_MENU:
	case SCREEN_PREAMBLE:
		break;
	case SCREEN_GAME:
		if (IsKeyPressed(KEY_ESCAPE)) {
			state.paused = !state.paused;
		}
		if (state.paused) {
			break;
		}

		Vector2 dir = Vector2Normalize((Vector2){
			IsKeyDown(KEY_D) - IsKeyDown(KEY_A),
			IsKeyDown(KEY_S) - IsKeyDown(KEY_W),
		});
		Vector2 vel = Vector2Multiply(dir, (Vector2){PLAYER_SPEED, PLAYER_SPEED});
		state.player.pos = Vector2Add(state.player.pos, vel);
		state.cam.target = Vector2Add(
			state.cam.target,
			Vector2Divide(
				Vector2Subtract(state.player.pos, state.cam.target),
				(Vector2){10, 10})
			);
		break;
	}

	// Draw
	BeginDrawing();
	ClearBackground(RAYWHITE);
	switch(on_screen) {
	case SCREEN_MENU:
		menu(MENU_TYPE_MAIN);
		break;
	case SCREEN_PREAMBLE:
		; static const char *preamble =
			"John Doe, is a family everyman who just won big at 7/11.\n"
			"His family still doesn't know about the big piles of cash he\n"
			"earned from a Golden Paw Jackpot scratch card.";
		Font font = GuiGetFont();
		Vector2 text_size = MeasureTextEx(font, preamble, 24, 18);
		Vector2 center = {HALF_WIDTH, HALF_HEIGHT};
		Vector2 text_pos = Vector2Subtract(center, Vector2Multiply(text_size, (Vector2){0.5, 0.5}));
		DrawTextEx(font, preamble, text_pos, 24, 18, BLACK);
		if(GuiButton((Rectangle){center.x - 64, text_pos.y + text_size.y + 32, 128, 32}, "Continue")) {
			on_screen = SCREEN_GAME;
		}
		break;
	case SCREEN_GAME:
		BeginMode2D(state.cam);

		// Grid
		int grid_origin = -(int)(128*GRID_SIZE/2);
		for(int i = 0; i < 128 * 128; ++i) {
			int x, y;
			y = grid_origin + (int)(i / 128) * GRID_SIZE;
			x = grid_origin + (i % 128) * GRID_SIZE;
			DrawLine(x, y, x + GRID_SIZE, y, BLACK);
			DrawLine(x, y, x, y + GRID_SIZE, BLACK);
		}

		// Money
		for(int i = 0; i < state.money_stacks.count; ++i) {
			int x = state.money_stacks.grid_cells[i*2 + 0]*GRID_SIZE;
			int y = state.money_stacks.grid_cells[i*2 + 1]*GRID_SIZE;
			DrawRectangle(x, y, GRID_SIZE - 1, GRID_SIZE - 1, GREEN);
		}

		// Player
		DrawCircle(state.player.pos.x, state.player.pos.y, 50, RED);

		EndMode2D();
		
		// GUI
		for(int i = 0; i < CHIP_COUNT; ++i) {
			int y = (CHIP_COUNT - 1 - i)*48 + 32;
			for(int j = 0; j < state.chips_count[i]; j++) {
				int x = 32 + j*30;
				Color col = CHIP_COLORS[i];
				DrawCircle(x, y, 32, col);
				Color inv = col;
				inv.r = 255 - inv.r;
				inv.g = 255 - inv.g;
				inv.b = 255 - inv.b;
				DrawRing(
					(Vector2){x, y},
					30, 32,
					0, 360,
					28, inv
				);
			}
		}

		if(state.paused) {
			Color col = GRAY;
			col.a /= 4;
			DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, col);
			menu(MENU_TYPE_PAUSE);
		}
		break;
	}
	EndDrawing();
}

int main(void) {
	for(int i = 0; i < CHIP_COUNT; i++) {
		state.chips_count[i] = i + 1;
	}
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Lame Jam");
	emscripten_set_main_loop(tick, 60, 1);
	CloseWindow();
	return 0;
}
