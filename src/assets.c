#include <stdio.h>
#include "assets.h"

const char *SOUND_NAMES[SOUND_COUNT] = {
	"excitement",
	"phone_ring",
	"ui_confirm",
};

const char *TEXTURE_NAMES[TEXTURE_COUNT] = {
	"player_front",
	"slot_front",
	"slot_side",
	"slot_gui",
	"floor_tile",
	"wall_tile",
	"money",
	"chip",
	"spinner_icon",
	"phone",
	"spin_button",
	"gate",
	"street_tile",
	"prompt",
};

typedef struct {
	Texture2D textures[TEXTURE_COUNT*ASSET_MAX_ITERATIONS];
	Wave sounds[SOUND_COUNT];
} Assets;

Assets assets;

void load_assets(void) {
	assets = (Assets){
		.textures = {},
		.sounds = {},
	};
	const int buff_len = 256;
	char buff[buff_len];
	for(int i = 0; i < TEXTURE_COUNT; ++i) {
		const char *name = TEXTURE_NAMES[i];
		snprintf(buff, buff_len, "res/%s.png", name);
		if(FileExists(buff)) {
			assets.textures[i*ASSET_MAX_ITERATIONS] = LoadTexture(buff);
		} else {
			for(int j = 0; j < ASSET_MAX_ITERATIONS; ++j) {
				snprintf(buff, buff_len, "res/%s_%02d.png", name, j + 1);
				if(!FileExists(buff)) {
					break;
				}
				assets.textures[i*ASSET_MAX_ITERATIONS + j] = LoadTexture(buff);
			}
		}
	}
	for(int i = 0; i < SOUND_COUNT; ++i) {
		snprintf(buff, buff_len, "res/%s.wav", SOUND_NAMES[i]);
		assets.sounds[i] = LoadWave(buff);
	}
}

Texture2D lookup_texture(Texture_Id id, unsigned int iter) {
	return assets.textures[id*ASSET_MAX_ITERATIONS + (iter - 1)];
}

Wave lookup_sound(Sound_Id id) {
	return assets.sounds[id];
}
