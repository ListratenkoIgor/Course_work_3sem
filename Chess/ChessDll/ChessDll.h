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
extern enum {
	REDISPLAY_BOARD = -1
	, LET_COMPUTER_PLAY = -2
	, TAKE_BACK = -3
	, NEW_GAME = -4
	, QUIT = -5
	, SYNTAX_ERROR = -6
	, MATE = -7
	, SET_MAXIMUM_LEVEL = -93
	, SET_MINIMUM_LEVEL = -101
};


extern int board[BOARD_SIZE];
extern int nextPiece[BOARD_SIZE];
extern int prevPiece[BOARD_SIZE];
extern int xCapture;
extern int turnColor;
extern int boardHash;
extern const int DIRS[8] = { STRIDE, STRIDE + 1, 1, -STRIDE + 1, -STRIDE, -STRIDE - 1, -1, STRIDE - 1 };
extern const int KNIGHT_MOVES[8] = { STRIDE * 2 + 1, 2 + STRIDE, 2 - STRIDE, -STRIDE * 2 + 1, -STRIDE * 2 - 1, -STRIDE - 2, STRIDE - 2, STRIDE * 2 - 1 };
extern const int PIECE_VALUES[7] = { 0, PAWN_VALUE, 5 * PAWN_VALUE, 3 * PAWN_VALUE, 3 * PAWN_VALUE, QUEEN_VALUE, MAX_SCORE };
extern const int RANK_VALUES[8 + 4] = { 0, 0, 0, 30, 50, 60, 60, 50, 30, 0, 0, 0 };
extern enum {
	MAX_TARGET_SQUARES = 32, 	/* Max target squares for a single piece. */
	MAX_MOVES = 256,			/* Max psuedo-legal moves from any given position. 256 should be enough for any normal game. Prove me wrong. */
	MAX_CAPTURES = 32 			/* Max pseudo-legal captures from any given position. 32 should be enough for any normal game. Prove me wrong. */
};
extern int INIT_BOARD[BOARD_SIZE] = {
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	ROOK | WHITE,	KNIGHT | WHITE,	BISHOP | WHITE,	QUEEN | WHITE,	KING | WHITE,	BISHOP | WHITE,	KNIGHT | WHITE,	ROOK | WHITE,	OFFBOARD,
	OFFBOARD,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	PAWN | WHITE,	OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			EMPTY,			OFFBOARD,
	OFFBOARD,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	PAWN | BLACK,	OFFBOARD,
	OFFBOARD,	ROOK | BLACK,	KNIGHT | BLACK,	BISHOP | BLACK,	QUEEN | BLACK,	KING | BLACK,	BISHOP | BLACK,	KNIGHT | BLACK,	ROOK | BLACK,	OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,
	OFFBOARD,	OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD,		OFFBOARD
};

extern const char* PIECE_CHARS;
extern const char* PIECE_NAMES[];

extern enum {
	HASH_TABLE_SIZE = 64 * 1024,
	HASH_MASK = ~0
};
extern int pieceKeys[BOARD_SIZE * 64];
extern int xCaptureKeys[256];
extern int colorKeys[WHITE + 1];
extern int hashTable[HASH_TABLE_SIZE];
extern int evalCounter;
extern int history[HISTORY_SIZE * 4];		/* Four elements per half-move: 16-bit move, piece, captured and xCapture (see doMove() for more info). */
extern int historyCount;
extern int minEvals;


extern bool canCaptureKing(int);					 
extern bool checkInvariant();
extern int  doMove(int, int, int, int, int);
extern "C" int  executeMove(int);						
extern int  findBestMove(int, int, int, int, int, int, int[]);	 
extern int  getComputerMove();
extern int  getUserMove(); 

extern int getUserMove2(int, int, int, int);
extern bool isOnBoard(int);
extern bool isValidColor(int);
extern bool isValidMove(int);
extern bool isValidPiece(int);
extern void initGlobals();
extern void liftPiece(int);
extern int  listMoves(int, int, int[]);				 
extern int  moveFrom(int move);
extern int  moveTo(int move);
extern int  makeMove(int, int, int, int);
extern void movePiece(int, int, int);
extern unsigned int nextRandom();
extern void printBoard(int, int);
extern void putPiece(int, int);
extern void registerMove(int, int, int, int);
extern void restart();
extern int  search(int, int, int, int, bool);		
extern int  searchAll(int, int, int, int, int);		
extern int  searchCaptures(int, int, int, int, int);
extern void setLevel(int);
extern bool squareCapturesCastlingKing(int);
extern bool squareCapturesKing(int);
extern int  switchColor(int);
extern int  takeBackMove();
extern int  traceRBQ(int, int, int, int[], int, int); 
extern void undoMove(int, int, int, int, int);		 
//____________________________________________________________________________________________________________________________________________________________
extern int BORDER_SIZE = 8;