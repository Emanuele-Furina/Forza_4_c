#ifndef F4_DEFINITIONS_H
#define F4_DEFINITIONS_H

// defaults: 7, 6, 4
#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define COUNT_TARGET 4

typedef enum {
	CELL_EMPTY,
	CELL_PLAYER1,
	CELL_PLAYER2,

	CELL_STATE_LENGTH
} CellState;

typedef CellState Board[BOARD_WIDTH][BOARD_HEIGHT];

#endif /* F4_DEFINITIONS_H */
