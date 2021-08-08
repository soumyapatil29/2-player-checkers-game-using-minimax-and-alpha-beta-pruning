#ifndef PLAYER_H
#define PLAYER_H
#include "Checker.h"
#include "CheckerBoard.h"

class Player{
	public:
		virtual void makeMove(CheckerBoard&) = 0;
		virtual void printStats() = 0;
};
#endif
