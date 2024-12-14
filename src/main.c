#include<emscripten/emscripten.h>
#include<raylib.h>

void tick(void) {
	BeginDrawing();
		ClearBackground(RAYWHITE);
		DrawText("Window!", 190, 200, 20, LIGHTGRAY);
	EndDrawing();
}

int main(void) {
	InitWindow(1920, 1080, "Lame Jam");
	emscripten_set_main_loop(tick, 0, 1);
	CloseWindow();
	return 0;
}
