#ifndef CHECKERBOARD_H
#define CHECKERBOARD_H
#include "Checker.h"
#include <string>

using namespace std;

class CheckerBoard{

	private:
		bool game_over;
		int turn_player;
		int winner;						//1 is W; -1 is B; 0 is tie/undetermined
		Checker** board;
		string state_code;

		void populateBoard();
		void removeChecker(int);
		void whoWon(int);
		void genStateCode();

	public:

		CheckerBoard();
		CheckerBoard(CheckerBoard&);
		~CheckerBoard();
		
		Checker** getBoard();
		bool isGameOver();
		int getTurnPlayer();
		int getWinner();
		void switchTurns();
		string getStateCode();
		
		int getNumCheckers(int);
		
		Checker* getChecker(int, int);

		void moveSingle(int, int, int, int);
		void moveHopCapture(int, int, int, int);
		
		void newGame();
		void setGame(CheckerBoard&);

		string toString();
		
		bool operator==(const CheckerBoard&);
};
#endif
