#ifndef CHECKER_H
#define CHECKER_H

#include <string>

using namespace std;

class Checker{
	private:
		int player_num;
		bool type;		//False = Standard Piece; True = King piece
	
	public:
		Checker(int);
		Checker(int, bool);
		Checker(const Checker&);
		
		int getPlayer();
		bool isKing();
		
		void king_me();
		string toString();
		
		bool operator==(const Checker&);
};
#endif
