#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"

int board_available_in_column(Board b, int column) {
	if (column < 0 || column >= BOARD_WIDTH)
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (b[column][y] != CELL_EMPTY) return y - 1;
	return BOARD_HEIGHT - 1;
}

int main(void) {
	Board game = { 0 };


	return 0;
}
