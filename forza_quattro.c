#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "definitions.h"
#include "ui.h"

#define USV_LH L"\u2500"
#define USV_LV L"\u2502"

#define USV_DR L"\u256D" // L"\u250C"
#define USV_DL L"\u256E" // L"\u2510"
#define USV_UR L"\u2570" // L"\u2514"
#define USV_UL L"\u256F" // L"\u2518"

#define USV_BK L"\u2585" /// blocked column
#define USV_PC L"\u25CB" /// normal cell
#define USV_WC L"\u25CF" /// winning cell
#define USV_CU L"\u21D3" /// cursor indicator

// indices for 256 colour palette
static const unsigned char player_colours[MAX_PLAYERS] = {
	21, 196, 201, 22, 202, 46, 51, 226, 225, 224
};

static void plr_fg(CellState player, const int *plrs, const int *none) {
	if (player == 0) {
		uiprintf(SGR_RESET "%ls", none);
		return;
	}
	uiprintf(FG_256 "%dm", player_colours[player - 1]);
	if (plrs != NULL) uiprintf("%ls" SGR_RESET, plrs);
}

static int board_available_in_column(Board b, int column) {
	if (column < 0 || column >= BOARD_WIDTH)
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (b[column][y] != STATE_EMPTY) return y - 1;
	return BOARD_HEIGHT - 1;
}

static int board_real_drop_column(Board b, int column, int direction) {
	int rcol = column;

	do {
		rcol = rcol + direction;
		if (rcol < 0) return 0;
		if (rcol >= BOARD_WIDTH) return 0;
	} while (b[rcol][0] != STATE_EMPTY);

	return rcol - column;
}

static bool board_player_has_won(
	Board b, CellState player,
	int positions[COUNT_TARGET * 2]
) {
	int count;

	// check horizontal
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH - (COUNT_TARGET - 1); x++) {
			count = 0;
			for (int i = 0; i < COUNT_TARGET; i++)
				if (b[x + i][y] == player) {
					count++;
					positions[(i * 2) + 0] = x + i;
					positions[(i * 2) + 1] = y;
				}
			if (count == COUNT_TARGET) return true;
		}
	}

	// check vertical
	for (int x = 0; x < BOARD_WIDTH; x++) {
		for (int y = 0; y < BOARD_HEIGHT - (COUNT_TARGET - 1); y++) {
			count = 0;
			for (int i = 0; i < COUNT_TARGET; i++)
				if (b[x][y + i] == player) {
					count++;
					positions[(i * 2) + 0] = x;
					positions[(i * 2) + 1] = y + i;
				}
			if (count == COUNT_TARGET) return true;
		}
	}

	// check diagonal (bottom-left to top-right)
	for (int x = 0; x < BOARD_WIDTH - (COUNT_TARGET - 1); x++) {
		for (int y = (COUNT_TARGET - 1); y < BOARD_HEIGHT; y++) {
			count = 0;
			for (int i = 0; i < COUNT_TARGET; i++)
				if (b[x + i][y - i] == player) {
					count++;
					positions[(i * 2) + 0] = x + i;
					positions[(i * 2) + 1] = y - i;
				}
			if (count == COUNT_TARGET) return true;
		}
	}

	// check diagonal (top-left to bottom-right)
	for (int x = 0; x < BOARD_WIDTH - (COUNT_TARGET - 1); x++) {
		for (int y = 0; y < BOARD_HEIGHT - (COUNT_TARGET - 1); y++) {
			count = 0;
			for (int i = 0; i < COUNT_TARGET; i++)
				if (b[x + i][y + i] == player) {
					count++;
					positions[(i * 2) + 0] = x + i;
					positions[(i * 2) + 1] = y + i;
				}
			if (count == COUNT_TARGET) return true;
		}
	}

	return false;
}

// should not move the cursor!
void board_display(Board b, CellState player) {
	uiprintf("%ls", USV_DR);
	for (int x = 0; x < BOARD_WIDTH; x++) {
		uiprintf("%ls", b[x][0] != STATE_EMPTY ? USV_BK : L" ");
		if (x < BOARD_WIDTH - 1) for (int i = 0; i < SP_COEFF; i++)
			uiprintf("%ls", (b[x + 0][0] != STATE_EMPTY &&
			                 b[x + 1][0] != STATE_EMPTY) ? USV_BK : L" ");
	}
	uiprintf("%ls", USV_DL "\n\r");

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		uiprintf("%ls", USV_LV);
		for (int x = 0; x < BOARD_WIDTH; x++) {
			plr_fg(b[x][y], USV_PC, L" ");
			if (x < BOARD_WIDTH - 1)
				for (int i = 0; i < SP_COEFF; i++) uiprintf("%ls", L" ");
		}
		uiprintf("%ls", USV_LV);
		uiprintf("\n\r");
	}

	uiprintf("%ls", USV_UR);
	for (int x = 0; x < BOARD_WIDTH; x++) {
		uiprintf("%ls", USV_LH);
		if (x < BOARD_WIDTH - 1)
			for (int i = 0; i < SP_COEFF; i++) uiprintf("%ls", USV_LH);
	}
	uiprintf("%ls", USV_UL "\n\r");

	plr_fg(player, NULL, NULL);
	uiprintf("Giocatore %d" SGR_RESET SGR_BLINK
	         " sta pensando..." SGR_RESET ANSI_CLR "\r", player);
	uiup(BOARD_HEIGHT + 2);
}

int main(void) {
	Board game = { 0 };
	int nplayers = DEFAULT_PLAYERS;

	uiinit();
	uihidecur();

	CellState player = 1;
	int column = 0, watchdog = 0;
	bool game_over = false;
	while (watchdog < (BOARD_WIDTH * BOARD_HEIGHT) && !game_over) {
		board_display(game, player);
		uiright(column * (SP_COEFF + 1) + 1);

		char c;
		int row;
		if (game[column][0] == STATE_EMPTY)
			{ uiprintf("%ls", USV_CU); uileft(1); }
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
			uiprintf("%ls", game[column][0] == STATE_EMPTY ? L" " : USV_BK);
			uileft(1);
			if (delta > 0)
				uiright(delta * (SP_COEFF + 1));
			else if (delta < 0)
				uileft(-delta * (SP_COEFF + 1));
			column += delta;
			if (game[column][0] == STATE_EMPTY)
				{ uiprintf("%ls", USV_CU); uileft(1); }
		}

		uileft(column * (SP_COEFF + 1) + 1);

		// check if the current player has won
		int positions[COUNT_TARGET * 2] = { 0 };
		if (board_player_has_won(game, player, positions)) {
			game_over = true;

			board_display(game, player);
			for (int i = 0; i < COUNT_TARGET; i++) {
				int dx = (positions[i * 2 + 0]) * (SP_COEFF + 1) + 1;
				int dy = positions[i * 2 + 1] + 1;
				uidown(dy); uiright(dx);

				plr_fg(player, USV_WC, NULL);

				uiup(dy); uileft(dx + 1);
			}
			uidown(BOARD_HEIGHT + 2);
			plr_fg(player, NULL, NULL);
			uiprintf("Giocatore %d" SGR_RESET " ha vinto!"
			         ANSI_CLR "\r\n", player);
			break;
		}

		player = (player % nplayers) + 1;
	}

	if (!game_over) {
		board_display(game, player);
		uidown(BOARD_HEIGHT + 2);
		uiprintf("%s\r\n", SGR_RESET "Pareggio!" ANSI_CLR);
	}

	exit(0);
}
