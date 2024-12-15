#include<raylib.h>

#ifndef __CHIPS_H
#define __CHIPS_H

typedef enum {
	CHIP_WHITE,
	CHIP_RED,
	CHIP_BLUE,
	CHIP_GREEN,
	CHIP_BLACK,
	CHIP_COUNT,
} Chip_Type;

extern const int CHIP_VALUES[CHIP_COUNT];
extern const Color CHIP_COLORS[CHIP_COUNT];

void chips_from_dollars(int dollars, int (*chips)[CHIP_COUNT]);
int dollars_from_chips(int chips[CHIP_COUNT]);

#endif
