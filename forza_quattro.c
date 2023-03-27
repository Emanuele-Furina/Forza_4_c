#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "ui.h"

int board_available_in_column(Board b, int column) {
	if (column < 0 || column >= BOARD_WIDTH)
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (b[column][y] != CELL_EMPTY) return y - 1;
	return BOARD_HEIGHT - 1;
}

// does not move the cursor!
void board_display(Board b) {
	uiprintf("%ls", L"\u250C");
	uiprintf("%*s", BOARD_WIDTH, "");
	uiprintf("%ls", L"\u2510\n\r");

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		uiprintf("%ls", L"\u2502");
		for (int x = 0; x < BOARD_WIDTH; x++)
			uiprintf("%s", state_visual[b[x][y]]);
		uiprintf("%ls", L"\u2502");
		uiprintf("\n\r");
	}

	uiprintf("%ls", L"\u2514");
	for (int x = 0; x < BOARD_WIDTH; x++)
		uiprintf("%ls", L"\u2500");
	uiprintf("%ls", L"\u2518\r");
	uiup(BOARD_HEIGHT + 1);
}

int main(void) {
	Board game = { 0 };

	uiinit();

	while (1) {
		board_display(game);
		uiright(1);

		int column = 0;
		char c;
		while ((c = uigetchar())) {
			if (c == 'q') exit(0);

			if (c == '\r') {
				game[column][board_available_in_column(game, column)]
					= CELL_PLAYER1;
				break;
			}

			if (c == '\e') {
				if (uigetchar() != '[') continue;
				c = uigetchar();
				if (c == 'D' && column > 0) {
					column--;
					uileft(1);
				} else if (c == 'C' && column < BOARD_WIDTH - 1) {
					column++;
					uiright(1);
				}
			}
		}

		uileft(column + 1);
	}

	exit(0);
}
