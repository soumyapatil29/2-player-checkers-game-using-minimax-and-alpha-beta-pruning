#ifndef COMPPLAYER_H
#define COMPPLAYER_H
#include "Player.h"
#include "Checker.h"
#include "CheckerBoard.h"
#include <chrono>

using namespace std;

class CompPlayer : public Player{
	private:
		int player_num;
		int search_type;
		int eval_func;
		int moves_made;
		int nodes_gen;
		int nodes_exp;
		chrono::milliseconds func_dur;
		
		struct Move{
			int c_row;
			int c_col;
			int r_dir;
			int c_dir;
			int move_type;
		};
		
		void minimax_a_b(CheckerBoard&, Move*&, int&, int, int&, int);
		void alpha_beta(CheckerBoard&, Move*&, int&, int, int&, int, int player);
		int move_gen(CheckerBoard&, Move**&, int*&, int);
		void genHopCaptureMoveSet(CheckerBoard&, Move**&, int*&, Move*, int&, int, int, int, int);
		int evaluate(CheckerBoard&);
		
	public:
		CompPlayer(int);
		CompPlayer(int, int, int);
		CompPlayer(const CompPlayer&);
		
		int getPlayerNum();
		int getSearchType();
		int getEvalFunct();
		
		void makeMove(CheckerBoard& game);

		void printStats();
};
#endif
