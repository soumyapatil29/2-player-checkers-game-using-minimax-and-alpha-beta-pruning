#ifndef USERPLAYER_H
#define USERPLAYER_H
#include "Player.h"
#include "Checker.h"
#include "CheckerBoard.h"
#include <string>

using namespace std;

class UserPlayer : public Player{
	private:
		int player_num;
		int moves_made;

	public:
		UserPlayer(int);
		UserPlayer(const UserPlayer&);

		int getPlayerNum();

		void makeMove(CheckerBoard&);

		void printStats();
};
#endif

