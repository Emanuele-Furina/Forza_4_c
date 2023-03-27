#include <stdio.h>
#include <stdlib.h>

#include "definitions.h"
#include "ui.h"

#define USV_LH L"\u2500"
#define USV_LV L"\u2502"

#define USV_DR L"\u256D" // L"\u250C"
#define USV_DL L"\u256E" // L"\u2510"
#define USV_UR L"\u2570" // L"\u2514"
#define USV_UL L"\u256F" // L"\u2518"

#define USV_BK L"\u2585"

int board_available_in_column(Board b, int column) {
	if (column < 0 || column >= BOARD_WIDTH)
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (b[column][y] != CELL_EMPTY) return y - 1;
	return BOARD_HEIGHT - 1;
}

int board_real_drop_column(Board b, int column, int direction) {
	int rcol = column;

	do {
		rcol = rcol + direction;
		if (rcol < 0) return 0;
		if (rcol >= BOARD_WIDTH) return 0;
	} while (b[rcol][0] != CELL_EMPTY);

	return rcol - column;
}

// should not move the cursor!
void board_display(Board b, CellState player) {
	uiprintf("%ls", USV_DR);
	for (int x = 0; x < BOARD_WIDTH; x++)
		uiprintf("%ls", b[x][0] != CELL_EMPTY ? USV_BK : L" ");
	uiprintf("%ls", USV_DL "\n\r");

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		uiprintf("%ls", USV_LV);
		for (int x = 0; x < BOARD_WIDTH; x++)
			uiprintf("%s", state_visual[b[x][y]]);
		uiprintf("%ls", USV_LV);
		uiprintf("\n\r");
	}

	uiprintf("%ls", USV_UR);
	for (int x = 0; x < BOARD_WIDTH; x++)
		uiprintf("%ls", USV_LH);
	uiprintf("%ls", USV_UL "\n\r");
	uiprintf("%s\r", turn_visual[player]);
	uiup(BOARD_HEIGHT + 2);
}

int main(void) {
	Board game = { 0 };

	uiinit();

	CellState player = CELL_PLAYER1;
	int column = 0, watchdog = 0;
	while (watchdog < (BOARD_WIDTH * BOARD_HEIGHT)) {
		board_display(game, player);
		uiright(column + 1);

		char c;
		int row;
		while ((c = uigetchar())) {
			if (c == 'q') exit(0);

			if (c == '\r' &&
			   (row = board_available_in_column(game, column)) >= 0 &&
			    row < BOARD_HEIGHT
			) {
				game[column][row] = player;
				watchdog++;
				break;
			}
			if (c != '\033') continue;

			if (uigetchar() != '[') continue;

			int direction = 0;
			switch (uigetchar()) {
			case 'C': direction = +1; break;
			case 'D': direction = -1; break;
			default: break;
			} if (direction == 0) continue;
			int delta = board_real_drop_column(game, column, direction);
			if (delta > 0)
				uiright(delta);
			else if (delta < 0)
				uileft(-delta);
			column += delta;
		}

		uileft(column + 1);
		player = player == CELL_PLAYER1 ? CELL_PLAYER2 : CELL_PLAYER1;
	}

	exit(0);
}
