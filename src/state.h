#include<raylib.h>

typedef enum {
	SCREEN_MENU,
	SCREEN_GAME,
} Screen;

typedef struct {
	float master_volume;
} Settings;

typedef struct {
	Camera2D cam;
	struct {
		Vector2 pos;
		Vector2 vel;
	} player;
} Game_State;

Screen on_screen;
Settings settings;
Game_State state;
