#define RAYGUI_IMPLEMENTATION

#include <assert.h>
#include <raylib.h>
#include <raymath.h>

#include "raygui.h"
#include "state.h"
#include "chips.h"
#include "player.h"
#include "assets.h"
#include "platform.h"

typedef enum {
	MENU_TYPE_MAIN,
	MENU_TYPE_PAUSE,
} Menu_Type;

const int ODDS[20] = {
	0, 0, 0, 0, 0,
	0, 0, 0, 0, 0,
	1, 1, 1, 2, 2,
	2, 3, 3, 3, 4,
};

bool gamble_sound_init = true;
Sound gamble_sound;

void display_chips(void) {
Texture tex = lookup_texture(TEXTURE_CHIP, 1);
	for(int i = 0; i < CHIP_COUNT; ++i) {
		int y = (CHIP_COUNT - 1 - i)*48 + 92;
		for(int j = 0; j < state->chips_count[i]; j++) {
			int x = 18 + j*30;
			Color col = CHIP_COLORS[i];
			DrawTexture(tex, x, y, col);
		}
	}
}

void display_dollars(void) {
	char dollar_buf[16];
	snprintf(dollar_buf, 16, "$%d.00", state->dollars_held);
	DrawTextEx(GuiGetFont(), dollar_buf, (Vector2){32, 32}, 38, 6, GREEN);
}

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
			state->paused = false;
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
	BeginDrawing();
	ClearBackground(GRAY);
	bool can_pause = false;
	switch(on_screen) {
	case SCREEN_MENU:
		menu(MENU_TYPE_MAIN);
		break;
	case SCREEN_PREAMBLE:
		; static const char *preamble =
			"Johnney Dough, is a family everyman who just won big at 7/11.\n"
			"His family still doesn't know about the massive piles of cash he\n"
			"earned from a Golden Paw Jackpot scratch card. Now he wants to follow\n"
			"his childhood dream of being a high roller.";
		Font font = GuiGetFont();
		Vector2 text_size = MeasureTextEx(font, preamble, 24, 0);
		Vector2 center = {HALF_WIDTH, HALF_HEIGHT};
		Vector2 text_pos = Vector2Subtract(center, Vector2Multiply(text_size, (Vector2){0.5, 0.5}));
		DrawTextEx(font, preamble, text_pos, 24, 2, BLACK);
		if(GuiButton((Rectangle){center.x - 64, text_pos.y + text_size.y + 32, 128, 32}, "Continue")) {
			on_screen = SCREEN_GAME;
		}
		break;
	case SCREEN_GAME:
		can_pause = true;
		BeginMode2D(state->cam);

		// Tiles
		for(int i = 0; i < state->tiles.count; ++i) {
			int x, y; tile_pos(i, &x, &y);
			Tile_Texture tex_id = state->tiles.textures[i];
			Texture tex = lookup_texture(tex_id.id, tex_id.iter);
			Tile_Flags flags = state->tiles.flags[i];
			Rectangle src = {0, 0, tex.width, tex.height};
			Rectangle dst = {x, y, tex.width, tex.height};
			dst.x += dst.width*0.5;
			dst.y += dst.height*0.5;
			float rot = 0.0f;
			bool flip_diag = (flags & TILE_FLIP_DIAGONAL_FLAG) > 0;
			if(flip_diag) {
				rot = 180;
			}
			if((flags & TILE_FLIP_HORIZONAL_FLAG) > 0) {
				if(flip_diag) {
					rot = 90;
				} else {
					src.width = -src.width;
				}
			}
			if((flags & TILE_FLIP_VERTICAL_FLAG) > 0) {
				if(flip_diag) {
					rot = 270;
				} else {
					src.height = -src.height;
				}
			}
			DrawTexturePro(
				tex,
				src,
				dst,
				(Vector2){dst.width*0.5, dst.height*0.5},
				rot,
				WHITE
			);
		}

		// Blocks
		int total_money = state->dollars_held;
		for(int i = 0; i < state->blocks.count; ++i) {
			int x, y;
			Texture tex;
			switch(state->blocks.types[i]) {
			case BLOCK_TYPE_DUMB:
			case BLOCK_TYPE_TELLER:
				break;
			case BLOCK_TYPE_MONEY:
				total_money += DOLLARS_PER_MONEY_BLOCK_SIZE
					*state->blocks.data[i].money.size;
				block_pos(i, &x, &y);
				Money_Block money_data = state->blocks.data[i].money;
				int tex_iter;
				switch(money_data.size){
				case 2:
					tex_iter = 1;
					break;
				case 1:
					tex_iter = 2;
					break;
				case 0:
					tex_iter = 3;
					break;
				default:
					printf("Invalid money size: %d\n", money_data.size);
					assert(false);
				}
				tex = lookup_texture(TEXTURE_MONEY, tex_iter);
				DrawTexture(tex, x, y, WHITE);
				break;
			case BLOCK_TYPE_SLOT_MACHINE:
				block_pos(i, &x, &y);
				Slot_Machine_Block slot_data = state->blocks.data[i].slots;
				tex = lookup_texture(slot_data.tex_id, slot_data.tex_iter);
				DrawTexture(tex, x, y, WHITE);
				break;
			}
		}
		if(total_money <= 0) {
			on_screen = SCREEN_ENDING;
		}

		update_player();

		EndMode2D();
		
		// GUI
		display_dollars();
		/*display_chips();*/
		break;
	case SCREEN_SLOTS:
		can_pause = true;
		display_dollars();
		if(gamble_sound_init) {
			gamble_sound = LoadSoundFromWave(lookup_sound(SOUND_EXCITEMENT));
			gamble_sound_init = false;
		}
		/*display_chips();*/
		Texture machine_tex = lookup_texture(TEXTURE_SLOT_GUI, 1);
		Texture button = lookup_texture(TEXTURE_SPIN_BUTTON, 1);
		Texture button_hover = lookup_texture(TEXTURE_SPIN_BUTTON, 2);
		int x_start = HALF_WIDTH - (int)(machine_tex.width/2);
		int y_start = HALF_HEIGHT - (int)(machine_tex.height/2);
		DrawTexture(machine_tex, x_start, y_start, WHITE);
		Rectangle spin_rect = {
			.x = HALF_WIDTH - (int)(button.width/2),
			.y = y_start + machine_tex.height + 32,
			.width = button.width,
			.height = button.height,
		};
		int cost = 300;
		bool can_afford = state->dollars_held >= cost;
		bool in_button = CheckCollisionPointRec(GetMousePosition(), spin_rect)
			&& can_afford;
		if(in_button && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			PlaySound(gamble_sound);
			state->dollars_held -= cost;
			state->results[0] = ODDS[GetRandomValue(0, 19)];
			state->results[1] = ODDS[GetRandomValue(0, 19)];
			state->results[2] = ODDS[GetRandomValue(0, 19)];
			for(int i = 0; i < 6; ++i) {
				state->random_results[i] = GetRandomValue(1, 4);
			}
			int reward = 0;
			if(state->results[0] != 0) {
				bool same1 = state->results[0] == state->results[1];
				bool same2 = state->results[0] == state->results[2];
				if(same1 && same2) {
					reward = cost*3;
				} else if(same1 || same2) {
					reward = cost + 100;
				}
			} else if(state->results[1] != 0 && state->results[1] == state->results[2]) {
				reward = cost + 100;
			}
			state->dollars_held += reward;
		}
		for(int i = 0; i < 3; ++i) {
			int iter = state->results[i];
			if(iter > 0) {
				Texture tex = lookup_texture(TEXTURE_SPINNER_ICON, iter);
				DrawTexture(tex, x_start + 230*i + 84, y_start + 300, WHITE);
			} else {
				int top = state->random_results[i];
				int bottom = state->random_results[i + 1];
				Texture tex_top = lookup_texture(TEXTURE_SPINNER_ICON, top);
				Texture tex_bottom = lookup_texture(TEXTURE_SPINNER_ICON, bottom);
				DrawTexture(tex_top, x_start + 230*i + 84, y_start + 200, WHITE);
				DrawTexture(tex_bottom, x_start + 230*i + 84, y_start + 400, WHITE);
			}
		}
		DrawTexture(
			in_button ? button_hover : button,
			spin_rect.x,
			spin_rect.y,
			can_afford ? WHITE : GRAY
		);
		if(GuiButton((Rectangle){CANVAS_WIDTH - 128 - 16, 16, 128, 64}, "Exit")) {
			on_screen = SCREEN_GAME;
			StopSound(gamble_sound);
		}
		break;
	case SCREEN_ENDING:
		; static const char *end_message = "You've lost all your money!";
		DrawTextEx(GuiGetFont(), end_message, (Vector2){HALF_WIDTH - 32, HALF_HEIGHT}, 38, 6, GREEN);
		break;
	}
	if(can_pause) {
		if(IsKeyPressed(KEY_ESCAPE)) {
			state->paused = !state->paused;
		}
		if(state->paused) {
			Color col = GRAY;
			col.a /= 4;
			DrawRectangle(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, col);
			menu(MENU_TYPE_PAUSE);
		}
	}
	EndDrawing();
}

int main(void) {
	InitWindow(CANVAS_WIDTH, CANVAS_HEIGHT, "Lame Jam");
	InitAudioDevice();
	load_assets();
	init_state();
	platform_loop();
	CloseWindow();
	return 0;
}
