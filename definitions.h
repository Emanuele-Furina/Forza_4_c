#ifndef F4_DEFINITIONS_H
#define F4_DEFINITIONS_H

// defaults: 7, 6, 4, 2
#define DEFAULT_BOARD_WIDTH 7
#define DEFAULT_BOARD_HEIGHT 6
#define DEFAULT_COUNT_TARGET 4
#define DEFAULT_PLAYERS 2
#define DEFAULT_SP_COEFF 1
#define MAX_PLAYERS 10

#if DEFAULT_PLAYERS > MAX_PLAYERS
	#error DEFAULT_PLAYERS cannot be greater than MAX_PLAYERS
#endif

#if DEFAULT_BOARD_WIDTH < 1 || DEFAULT_BOARD_HEIGHT < 1
	#error Board dimensions must be sensical!
#endif

#if DEFAULT_COUNT_TARGET <= 1
	#error Count target must be greater than 1!
#endif

#if DEFAULT_SP_COEFF < 0
	#error Spacing coefficient cannot be less than 0!
#endif

#ifdef F_NO_OPTS
#define BOARD_WIDTH DEFAULT_BOARD_WIDTH
#define BOARD_HEIGHT DEFAULT_BOARD_HEIGHT
#define COUNT_TARGET DEFAULT_COUNT_TARGET
#define SP_COEFF DEFAULT_SP_COEFF
#else
extern int s_board_width;
extern int s_board_height;
extern int s_count_target;
extern int s_sp_coeff;
#define BOARD_WIDTH (s_board_width)
#define BOARD_HEIGHT (s_board_height)
#define COUNT_TARGET (s_count_target)
#define SP_COEFF (s_sp_coeff)
#endif

typedef char CellState;
#define STATE_EMPTY ((CellState) 0)

#define BOARD(x) CellState x[BOARD_WIDTH][BOARD_HEIGHT]

#endif /* F4_DEFINITIONS_H */
