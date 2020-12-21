#include <iostream>
#include "../ChessDll/ChessDll.h"
//int __declspec(dllimport) executeMove(int);



int main()
{
	initGlobals();
	restart();
	setLevel(3);

	printBoard(0, 0);
	{
		int lastPiece;
		bool isComputersTurn = false;
		int lastFrom = 0;
		int lastTo = 0;
		int lastMove = 0;
		int moveResult = 0;
		do {
			const char* action;

			if (lastMove >= 0) {
				fputs((turnColor == WHITE ? "White's turn.\n" : "Black's turn.\n"), stdout);
			}

			action = "Last move:";
			if (!isComputersTurn) {
				lastMove = getUserMove();
				switch (lastMove) {
				case REDISPLAY_BOARD: {
					printBoard(lastFrom, lastTo);
					break;
				}

				case LET_COMPUTER_PLAY: {
					isComputersTurn = true;
					break;
				}

				case TAKE_BACK: {
					lastMove = takeBackMove();
					if (lastMove > 0) {
						lastFrom = moveFrom(lastMove);
						lastTo = moveTo(lastMove);
						lastMove = 0;
						action = "Took back move:";
					}
					else {
						fputs("No move to take back.\n", stdout);
						lastMove = -1;
					}
					break;
				}

				case NEW_GAME: {
					restart();
					lastMove = 0;
					lastFrom = 0;
					lastTo = 0;
					break;
				}

				default: {
					if (lastMove >= SET_MINIMUM_LEVEL && lastMove <= SET_MAXIMUM_LEVEL) {
						int level = lastMove - SET_MINIMUM_LEVEL;
						setLevel(level);
						fprintf(stdout, "Set computer level to %d.\n", level + 1);
						lastMove = -1;
					}
					break;
				}
				}
			}
			if (isComputersTurn) {
				lastMove = getComputerMove();
			}
			moveResult = 0;
			if (lastMove == CHECKMATE || lastMove == STALEMATE) {
				moveResult = lastMove;
				isComputersTurn = false;
				lastMove = -1;
			}

			if (lastMove > 0) {
				int lastCaptured, lastXCapture;
				lastFrom = moveFrom(lastMove);
				lastTo = moveTo(lastMove);
				lastPiece = board[lastFrom];
				lastCaptured = board[lastTo];
				lastXCapture = xCapture;

				moveResult = executeMove(lastMove);
				switch (moveResult) {
				case VALID_MOVE:
				case CHECKMATE:
				case STALEMATE: {
					registerMove(lastMove, lastPiece, lastCaptured, lastXCapture);
					if (moveResult == VALID_MOVE) {
						isComputersTurn = !isComputersTurn;
					}
					else {
						isComputersTurn = false;
					}
					break;
				}

				case IN_CHECK: {
					fputs("King in check.\n", stdout);
					lastMove = -1;
					break;
				}

				case INVALID_MOVE: {
					fputs("Invalid move.\n", stdout);
					lastMove = -1;
					break;
				}
				}
			}

			if (lastMove >= 0) {
				fputc('\n', stdout);
				printBoard(lastFrom, lastTo);
				if (lastFrom != 0 && lastTo != 0) {
					fprintf(stdout, "%s %s %c%c to %c%c.\n\n", action, PIECE_NAMES[lastPiece & PIECE_MASK]
						, ('A' + (lastFrom % STRIDE - 1)), ('1' + (lastFrom / STRIDE - 2))
						, ('A' + (lastTo % STRIDE - 1)), ('1' + (lastTo / STRIDE - 2)));
				}
			}

			if (moveResult == CHECKMATE) {
				fputs((turnColor == WHITE ? "Checkmate! Black wins.\n" : "Checkmate! White wins.\n"), stdout);
				lastMove = MATE;
			}
			else if (moveResult == STALEMATE) {
				fputs("Stalemate!\n", stdout);
				lastMove = MATE;
			}

		} while (lastMove != QUIT);
	}

	return 0;
}
