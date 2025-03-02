#include "platform.h"
#include <emscripten/emscripten.h>

void platform_loop() {
	emscripten_set_main_loop(tick, 60, 1);
}
