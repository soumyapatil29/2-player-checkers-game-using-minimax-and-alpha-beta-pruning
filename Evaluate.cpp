#include "Evaluate.h"
#include "CheckerBoard.h"
#include "Checker.h"
#include <iostream>

int evaluate_def(CheckerBoard& game, int player){ return 0; }

int evaluate_1(CheckerBoard& game, int player){		
	int total_value = 0,
                cur_k=0,
                opp_k=0,
		row = 0,
		col = 0;

	Checker* piece = nullptr;

	for(int i = 0; i < 32; i++){				//Examine each space on the board for checker pieces
		row = i / 4;
		col = (2 * (i - (row * 4))) + ((row + 1) % 2);

		piece = game.getChecker(row, col);
		if(piece == nullptr)					//If there's no checker piece on the board space, move on to next space
			continue;

		if(piece->getPlayer() == player){
                    if(piece->isKing()){cur_k++;}
                    else{total_value++;}
                }
                else{
                    if(piece->isKing()){opp_k++;}
                    else{total_value--;}
                }
                    
           }
        total_value = total_value + 1.5*(cur_k - opp_k);
        return total_value;
        
}	
                    
 