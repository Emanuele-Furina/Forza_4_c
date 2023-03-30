#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

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

int s_board_width = DEFAULT_BOARD_WIDTH;
int s_board_height = DEFAULT_BOARD_HEIGHT;
int s_count_target = DEFAULT_COUNT_TARGET;
unsigned short s_sp_coeff = DEFAULT_SP_COEFF;

bool normal_exit = false;

// indices for 256 colour palette
static const unsigned char player_colours[] = {
	21, 196, 201, 22, 202, 46, 51, 226, 225, 224
};
#define MAX_PLAYERS (sizeof(player_colours) / sizeof(player_colours[0]))

static void die(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);

	exit(1);
}

static void usage(const char *name) {
	die("usage: %s [-p players] "
	              "[-w width] [-h height] [-t target] [-s spacing]", name);
}

static void plr_fg(CellState player, const int *plrs, const int *none) {
	if (player == 0) {
		uiprintf(SGR_RESET "%ls", none);
		return;
	}
	uiprintf(FG_256 "%dm", player_colours[player - 1]);
	if (plrs != NULL) uiprintf("%ls" SGR_RESET, plrs);
}

static unsigned short board_vis_width(int n) {
	return BOARD_WIDTH * (n + 1) + 1;
}

static int board_available_in_column(BOARD(b), int column) {
	if (column < 0 || column >= BOARD_WIDTH)
		return -1;

	for (int y = 0; y < BOARD_HEIGHT; y++)
		if (b[column][y] != STATE_EMPTY) return y - 1;
	return BOARD_HEIGHT - 1;
}

static int board_real_drop_column(BOARD(b), int column, int direction) {
	int rcol = column;
	if (direction == 0) return 0;

	do {
		rcol = rcol + direction;
		if (rcol < 0)
			rcol = BOARD_WIDTH - 1;
		if (rcol >= BOARD_WIDTH)
			rcol = 0;
	} while (b[rcol][0] != STATE_EMPTY);

	return rcol - column;
}

static int board_count_in_dir(
	BOARD(b), CellState player, int ix, int iy, int dirx, int diry,
	int positions[][2], int offset
) {
	int count = 0;
	int x = ix + dirx, y = iy + diry;
	while (x >= 0 && x < BOARD_WIDTH && y >= 0 && y < BOARD_HEIGHT) {
		if (b[x][y] == player) {
			positions[count + offset][0] = x;
			positions[count + offset][1] = y;
			count++;
		} else break;
		x += dirx; y += diry;
	}
	return count;
}

static int board_player_has_won(
	BOARD(b), CellState player, int ix, int iy,
	int positions[][2]
) {
	for (int ic = 0; ic < 4; ic++) {
		int dirx = ic < 2 ? -1 : (ic - 2);
		int diry = ic == 0 ? 0 : -1;
		int sum = 1;
		positions[0][0] = ix;
		positions[0][1] = iy;
		sum += board_count_in_dir(b, player,
		    ix, iy, +dirx, +diry, positions, sum);
		sum += board_count_in_dir(b, player,
		    ix, iy, -dirx, -diry, positions, sum);
		if (sum >= COUNT_TARGET) return sum;
	}

	return 0;
}

// should not move the cursor!
unsigned short board_display(BOARD(b), CellState player) {
	unsigned short real_sp = SP_COEFF;
	unsigned short dims[2];
	uidimensions(dims);
	while (board_vis_width(real_sp) > dims[0] && real_sp > 0) real_sp--;

	uiprintf("%ls", USV_DR);
	for (int x = 0; x < BOARD_WIDTH; x++) {
		uiprintf("%ls", b[x][0] != STATE_EMPTY ? USV_BK : L" ");
		if (x < BOARD_WIDTH - 1) for (int i = 0; i < real_sp; i++)
			uiprintf("%ls", (b[x + 0][0] != STATE_EMPTY &&
			                 b[x + 1][0] != STATE_EMPTY) ? USV_BK : L" ");
	}
	uiprintf("%ls", USV_DL "\n\r");

	for (int y = 0; y < BOARD_HEIGHT; y++) {
		uiprintf("%ls", USV_LV);
		for (int x = 0; x < BOARD_WIDTH; x++) {
			plr_fg(b[x][y], USV_PC, L" ");
			if (x < BOARD_WIDTH - 1)
				for (int i = 0; i < real_sp; i++) uiprintf("%ls", L" ");
		}
		uiprintf("%ls", USV_LV);
		uiprintf("\n\r");
	}

	uiprintf("%ls", USV_UR);
	for (int x = 0; x < BOARD_WIDTH; x++) {
		uiprintf("%ls", USV_LH);
		if (x < BOARD_WIDTH - 1)
			for (int i = 0; i < real_sp; i++) uiprintf("%ls", USV_LH);
	}
	uiprintf("%ls", USV_UL "\n\r");

	plr_fg(player, NULL, NULL);
	uiprintf("Giocatore %d" SGR_RESET SGR_BLINK
	         " sta pensando..." SGR_RESET ANSI_CLR "\r", player);
	uiup(BOARD_HEIGHT + 2);

	return real_sp;
}

unsigned long decode_opts(int argc, char *argv[]) {
	unsigned long nplayers = DEFAULT_PLAYERS;
	for (int i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-p")) {
			if (++i >= argc) die("Expected argument after -p!");
			nplayers = atoi(argv[i]);
			if (nplayers < 2 || nplayers > MAX_PLAYERS)
				die("Invalid number of players!");
		} else if (!strcmp(argv[i], "-w")) {
			if (++i >= argc) die("Expected argument after -w!");
			s_board_width = atoi(argv[i]);
			if (s_board_width < 1)
				die("Invalid board width!");
		} else if (!strcmp(argv[i], "-h")) {
			if (++i >= argc) die("Expected argument after -h!");
			s_board_height = atoi(argv[i]);
			if (s_board_height < 1)
				die("Invalid board height!");
		} else if (!strcmp(argv[i], "-t")) {
			if (++i >= argc) die("Expected argument after -t!");
			s_count_target = atoi(argv[i]);
			if (s_count_target <= 1)
				die("Invalid counter target!");
		} else if (!strcmp(argv[i], "-s")) {
			if (++i >= argc) die("Expected argument after -s!");
			int t = atoi(argv[i]);
			if (t < 0 || t > USHRT_MAX)
				die("Invalid spacing coefficient!");
			else s_sp_coeff = t;
		} else {
			usage(argv[0]);
		}
	}
	return nplayers;
}

void atexit_cursor_cleanup(void) {
	if (normal_exit) return;
	uidown(BOARD_HEIGHT + 2);
	uiprintf("\r\033[K");
}

CellState do_round(BOARD(b), int nplayers) {
	CellState player = 1;
	int column = 0, watchdog = 0;
	while (watchdog < (BOARD_WIDTH * BOARD_HEIGHT)) {
		unsigned short real_sp = board_display(b, player);
		uiright(column * (real_sp + 1) + 1);

		char c;
		int row = 0;
		bool redraw = false;
		if (b[column][0] == STATE_EMPTY)
			plr_fg(player, USV_CU "\033[D", NULL);
		while ((c = uigetchar())) {
			if (c == 'q') exit(0);

			if (c == '\r' &&
			   (row = board_available_in_column(b, column)) >= 0 &&
			    row < BOARD_HEIGHT
			) {
				b[column][row] = player;
				watchdog++;
				break;
			}

			if (c == 0x0c) { // ctrl-l
				redraw = true;
				break;
			}

			if (c != '\033') continue;
			if (uigetchar() != '[') continue;

			int direction = 0;
			switch (uigetchar()) {
			case 'C': direction = +1; break;
			case 'D': direction = -1; break;
			default: continue;
			}
			int delta = board_real_drop_column(b, column, direction);
			uiprintf("%ls", b[column][0] == STATE_EMPTY ? L" " : USV_BK);
			uileft(1);

			uimovh(delta * (real_sp + 1));
			column += delta;
			if (b[column][0] == STATE_EMPTY)
				plr_fg(player, USV_CU "\033[D", NULL);
		}

		uileft(column * (real_sp + 1) + 1);
		if (redraw) continue;

		// check if the current player has won
		int positions[MAX_TARGET][2];
		int npositions =
			board_player_has_won(b, player, column, row, positions);
		if (npositions > 0) {
			board_display(b, player);
			for (int i = 0; i < npositions; i++) {
				int dx = positions[i][0] * (real_sp + 1) + 1;
				int dy = positions[i][1] + 1;
				uimovrel(dx, dy);

				plr_fg(player, USV_WC, NULL);

				uimovrel(-dx - 1, -dy);
			}
			uidown(BOARD_HEIGHT + 2); plr_fg(player, NULL, NULL);
			uiprintf("Giocatore %d" SGR_RESET
			         " ha vinto!" ANSI_CLR "\r\n", player);
			return player;
		} else player = (player % nplayers) + 1;
	}

	board_display(b, player);
	uidown(BOARD_HEIGHT + 2);
	uiprintf("%s\r\n", SGR_RESET "Pareggio!" ANSI_CLR);
	return STATE_EMPTY;
}

int main(int argc, char *argv[]) {
#ifdef F_NO_OPTS
	unsigned long nplayers = DEFAULT_PLAYERS;
#else
	unsigned long nplayers = decode_opts(argc, argv);
#endif
	if (nplayers > MAX_PLAYERS)
		die("Invalid number of players!");
	if (COUNT_TARGET > MAX_TARGET)
		die("Counter target is unobtainable with this board size!");

	CellState game[BOARD_WIDTH][BOARD_HEIGHT];
	memset(game, 0, sizeof(CellState) * BOARD_WIDTH * BOARD_HEIGHT);

	if (!uiinit())
		die("stderr must be a tty!");
	uihidecur();
	atexit(atexit_cursor_cleanup);

	do_round(game, nplayers);
	normal_exit = true;
}
