#ifndef F4_DEFINITIONS_H
#define F4_DEFINITIONS_H

// defaults: 7, 6, 4, 2
#define BOARD_WIDTH 7
#define BOARD_HEIGHT 6
#define COUNT_TARGET 4
#define DEFAULT_PLAYERS 2

#define MAX_PLAYERS 10

#if DEFAULT_PLAYERS > MAX_PLAYERS
	#error DEFAULT_PLAYERS cannot be greater than MAX_PLAYERS
#endif

#if BOARD_WIDTH < 1 || BOARD_HEIGHT < 1
	#error Board dimensions must be sensical!
#endif

#if COUNT_TARGET < 2
	#error Count target must be greater than 1!
#endif

#if COUNT_TARGET > BOARD_WIDTH && COUNT_TARGET > BOARD_HEIGHT
	#error Count target is unreachable!
#endif

typedef char CellState;
#define STATE_EMPTY ((CellState) 0)

typedef CellState Board[BOARD_WIDTH][BOARD_HEIGHT];

#endif /* F4_DEFINITIONS_H */
