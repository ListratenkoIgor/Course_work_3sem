#pragma once
#include "framework.h"

extern enum {
	STRIDE = (8 + 2),
	BOARD_SIZE = (8 + 4) * STRIDE,
	RANK_1 = STRIDE * 2,
	RANK_2 = STRIDE * 3,
	RANK_3 = STRIDE * 4,
	RANK_4 = STRIDE * 5,
	RANK_5 = STRIDE * 6,
	RANK_6 = STRIDE * 7,
	RANK_7 = STRIDE * 8,
	RANK_8 = STRIDE * 9,

	OFFBOARD = -1,
	EMPTY = 0,
	PAWN = 1,
	ROOK = 2,
	KNIGHT = 3,
	BISHOP = 4,
	QUEEN = 5,
	KING = 6,
	BLACK = 8,
	WHITE = 16,										/* We use two bits for color. 00 = empty, 01 = black, 10 = white, 11 = off-board. This allows fast bit-tricks for tracing possible moves. E.g. (... & color) == 0 to check if a square is empty or contains an opponent piece. */
	PIECE_MASK = 7,
	COLOR_MASK = BLACK | WHITE,
	COLOR_SWITCH_XOR = BLACK ^ WHITE,
	MOVED_MASK = 32,								/* The moved bit helps us determine if a king can castle or not and also if a pawn is allowed to move two squares. */
	PIECE_AND_MOVED_MASK = PIECE_MASK | MOVED_MASK,

	MIN_DEPTH = 4,
	MAX_DEPTH = 16,
	QUIESCENCE_DEPTH = 8,
	TO_SHIFT = 8,
	SCORE_SHIFT = 16,
	MOVE_MASK = 0x0000FFFF,
	FROM_MASK = 0x000000FF,
	TO_MASK = 0x0000FF00,
	SCORE_MASK = ~MOVE_MASK,
	CHECKMATE = 0xFFFF,
	STALEMATE = 0xFFFE,
	IN_CHECK = 0xFFFD,
	VALID_MOVE = 0xFFFC,
	INVALID_MOVE = -1,
	MAX_SCORE = 10000,
	MIN_SCORE = -MAX_SCORE,
	PAWN_VALUE = 100,
	QUEEN_VALUE = 9 * PAWN_VALUE,
	PROMOTION_SCORE = QUEEN_VALUE - PAWN_VALUE,
	CAPTURE_KING_MIN_SCORE = MAX_SCORE - MAX_DEPTH,
	RANDOM_AMOUNT = PAWN_VALUE / 10,
	PAWN_ADVANCE_SCORE = PAWN_VALUE / 10,
	CASTLING_SCORE = PAWN_VALUE / 2,
	CASTLING_MASK = 0x80, 								
	HISTORY_SIZE = 256
};

extern int board[BOARD_SIZE];


//____________________________________________________________________________________________________________________________________________________________
extern int BORDER_SIZE = 8;