#include"chips.h"

const int CHIP_VALUES[CHIP_COUNT] = {
	1,
	5,
	10,
	25,
	100,
};

const Color CHIP_COLORS[CHIP_COUNT] = {
	WHITE,
	RED,
	BLUE,
	GREEN,
	GRAY,
};

void chips_from_dollars(int dollars, int (*chips)[CHIP_COUNT]) {
	int dollars_left = dollars;
	for(int i = CHIP_COUNT - 1; i >= 0; --i) {
		(*chips)[i] += (int)(dollars_left/CHIP_VALUES[i]);
		dollars_left %= CHIP_VALUES[i];
	}
}

int dollars_from_chips(int chips[CHIP_COUNT]) {
	int dollars = 0;
	for(int i = 0; i < CHIP_COUNT; ++i) {
		dollars += CHIP_VALUES[i];
	}
	return dollars;
}
