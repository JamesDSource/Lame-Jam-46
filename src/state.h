#include<raylib.h>

typedef struct {
	Camera2D cam;
	struct {
		Vector2 pos;
		Vector2 vel;
	} player;
} Global_State;

Global_State state;
