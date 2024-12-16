#include <raylib.h>

#ifndef __ASSETS_H
#define __ASSETS_H

#define ASSET_MAX_ITERATIONS 16

typedef enum {
	TEXTURE_PLAYER_FRONT,
	TEXTURE_SLOT_FRONT,
	TEXTURE_SLOT_SIDE,
	TEXTURE_SLOT_GUI,
	TEXTURE_FLOOR_TILE,
	TEXTURE_WALL_TILE,
	TEXTURE_MONEY,
	TEXTURE_CHIP,
	TEXTURE_SPINNER_ICON,
	TEXTURE_PHONE,
	TEXTURE_SPIN_BUTTON,
	TEXTURE_GATE,
	TEXTURE_STREET_TILE,
	TEXTURE_PROMPT,
	TEXTURE_COUNT,
} Texture_Id;

typedef enum {
	SOUND_EXCITEMENT,
	SOUND_PHONE_RING,
	UI_CONFIRM,
	SOUND_COUNT,
} Sound_Id;

void load_assets(void);
Texture lookup_texture(Texture_Id id, unsigned int iter);
Wave lookup_sound(Sound_Id id);

#endif
