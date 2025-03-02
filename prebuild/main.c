#define CUTE_TILED_IMPLEMENTATION
#include "cute_tiled.h"

void process_tiled(const char *inpath, const char *outpath) {
	cute_tiled_map_t *map = cute_tiled_load_map_from_file(inpath, NULL);
}

int main() {
	process_tiled("prebuild/casino.json", "src/casino");
	return 0;	
}
