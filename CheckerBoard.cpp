#include "CheckerBoard.h"
#include "Checker.h"
#include <string>

CheckerBoard::CheckerBoard(){
	board = new Checker*[32];
	populateBoard();
	
	game_over = false;
	turn_player = 1;
	winner = 0;

	genStateCode();
}

CheckerBoard::CheckerBoard(CheckerBoard &c){
	this->board = c.getBoard();
	this->game_over = c.game_over;
	this->turn_player = c.turn_player;
	this->winner = c.winner;
	this->state_code = c.state_code;
}

CheckerBoard::~CheckerBoard(){
	for(int i = 0; i < 32; i++)
		delete board[i];
	delete [] board;
}

void CheckerBoard::populateBoard(){
	for(int i = 0; i < 16; i++){
		if(i > 11)
			board[i] = board[31-i] = nullptr;
		else{
			board[i] = new Checker(1);
			board[31-i] = new Checker(-1);
		}
	}	
}
void CheckerBoard::removeChecker(int index){
	if(index > -1 && index < 32){
		delete board[index];
		board[index] = nullptr;
	}
}

Checker** CheckerBoard::getBoard(){
	Checker** echo = new Checker*[32];
	
	for(int i = 0; i < 32; i++){
		if(board[i] == nullptr)
			echo[i] = nullptr;
		else
			echo[i] = new Checker(*(board[i]));
	}

	return echo;
}

bool CheckerBoard::isGameOver(){ return game_over; }
int CheckerBoard::getTurnPlayer(){ return turn_player; }
int CheckerBoard::getWinner() { return winner; }
string CheckerBoard::getStateCode() { return state_code; }

void CheckerBoard::switchTurns() {
	turn_player *= -1;

	string sub_code = state_code.substr(0,33);
	string end_code = state_code.substr(34,1);

	if(turn_player == -1)
		state_code = sub_code + "2" + end_code;
	else
		state_code = sub_code + "1" + end_code;
}

int CheckerBoard::getNumCheckers(int player){
	int num = 0;
	
	for(int i = 0; i < 32 && num < 12; i++){
		if(board[i] == nullptr)
			continue;
		
		if(board[i]->getPlayer() == player)
			num++;
	}
	
	return num;
}

Checker* CheckerBoard::getChecker(int row, int col){
	//Row values: 0 - 7; Column values: 0 - 7
	if(row < 0 || row > 7 || col < 0 || col > 7)
		return nullptr;
	
	Checker* c = nullptr;
	
	int index = (row * 4) + (col / 2);
		
	if(board[index] != nullptr)
		c = board[index];
	
	return c;
}

void CheckerBoard::moveSingle(int c_row, int c_col, int row_dir, int col_dir){
	//Row values: 0 - 7;  Column values: 0 - 7
	//Directional values: -1(up OR left), 1(down OR right)
	
	int new_col = c_col + col_dir;
	
	if(new_col < 0 || new_col > 7)
		return;
	
	int new_row = c_row + row_dir;
	
	if(new_row < 0 || new_row > 7)
		return;
	
	int cur_index = (c_row * 4) + (c_col / 2);
	int new_index = (new_row * 4) + (new_col / 2);
	
	if(board[cur_index] == nullptr || board[new_index] != nullptr)
		return;
	
	int cur_player = board[cur_index]->getPlayer();

	board[new_index] = board[cur_index];
	board[cur_index] = nullptr;
	
	whoWon(cur_player);
	genStateCode();
}

void CheckerBoard::moveHopCapture(int c_row, int c_col, int row_dir, int col_dir){
	//Row values: 0 - 7;  Column values: 0 - 7
	//Directional values: -1(up OR left), 1(down OR right)

	int mid_col = c_col + col_dir;
	int new_col = c_col + (col_dir * 2);
	
	if(new_col < 0 || new_col > 7)
		return;
	
	int mid_row = c_row + row_dir;
	int new_row = c_row + (row_dir * 2);
	
	if(new_row < 0 || new_row > 7)
		return;
	
	int cur_index = (c_row * 4) + (c_col / 2);
	int mid_index = (mid_row * 4) + (mid_col / 2);
	int new_index = (new_row * 4) + (new_col / 2);
	
	if(board[cur_index] == nullptr || board[new_index] != nullptr || board[mid_index] == nullptr)
		return;
	
	int cur_player = board[cur_index]->getPlayer();
	
	if(board[mid_index]->getPlayer() == cur_player)
		return;
	
	board[new_index] = board[cur_index];
	board[cur_index] = nullptr;
	removeChecker(mid_index);
	
	whoWon(cur_player);
	genStateCode();
}

void CheckerBoard::whoWon(int player){
	bool first_player = false,
		 second_player = false,
		 player_moves = false,
		 ongoing = false,
		 king_piece = false;
	int ckr_plyr = 0,
		row = 0,
		col = 0,
		r_start = 0,
		r_end = 0,
		cur_row = 0,
		cur_col = 0,
		local_index = 0;

	for(int i = 0; i < 32 && !ongoing; i++){
		if(board[i] == nullptr)
			continue;
		
		ckr_plyr = board[i]->getPlayer();
		
		switch(ckr_plyr){
			case 1: first_player = true;
					 break;
			case -1: second_player = true;
					 break;
		}
		
		if(ckr_plyr != player){
			row = i / 4;											
			col = (2 * (i - (row * 4))) + ((row + 1) % 2); 			
			
			king_piece = board[i]->isKing();
				
			if(king_piece || ckr_plyr == -1)						
				r_start = -1;										
			else														
				r_start = 1;
			
			if(king_piece || ckr_plyr == 1)							
				r_end = 1;										
			else														
				r_end = -1;
			
			for(int r = r_start; (r < r_end + 1) && (!player_moves); r+= 2){		
				cur_row = row + r;
				
				if(cur_row < 0) continue;																
				if(cur_row > 7) break;
				
				for(int c = -1; c < 2; c += 2){
					cur_col = col + c;
					if(cur_col < 0) continue;																		
					if(cur_col > 7) break;
					
					local_index = (cur_col / 2) + (cur_row * 4);														
					
					if(board[local_index] == nullptr){														
						player_moves = true;
						break;
					}
									
					if(board[local_index]->getPlayer() == player)
						continue;
																							
					if((cur_row + r) < 0 || (cur_row + r) > 7 || (cur_col + c) < 0 || (cur_col + c) > 7)	
						continue;
					
					local_index = ((cur_row + r) * 4) + ((cur_col + c) / 2);								
					
					if(board[local_index] == nullptr){														
						player_moves = true;
						break;
					}
				}																							
			}
		}
		
		ongoing = first_player && second_player && player_moves;
	}
	
	if(first_player && !second_player)
		winner = 1;
	else if(!first_player && second_player)
		winner = -1;
	else if(!player_moves)
		winner = player;
}

void CheckerBoard::newGame(){
	for(int i = 0; i < 32; i++)
		delete board[i];
	
	populateBoard();

	game_over = false;
	winner = 0;
	turn_player = 1;
}

void CheckerBoard::setGame(CheckerBoard &c){
	for(int i = 0; i < 32; i++)
		delete board[i];
	delete [] board;
	
	board = c.getBoard();
	
	this->turn_player = c.turn_player;
	this->game_over = c.game_over;
	this->winner = c.winner;
}

string CheckerBoard::toString(){
	string nps = "####";
	string buf = "    ";
	string r_div = "|----+----+----+----+----+----+----+----|\n";
	string boarder = "O---------------------------------------O\n";
	string c_div = "|";
	string col_index = " -0-- -1-- -2-- -3-- -4-- -5-- -6-- -7-- \n";
	
	string board_print = col_index + boarder;
	
	string checker = "";
	string cell_spacing = "";

	int row = 0;
	int col = 0;
	
	for (int i = 0; i < 32; i++){
		row = i / 4;
		col = i % 4;
		
		if(board[i] == nullptr)
			checker = buf;
		else
			checker = board[i]->toString();
			
		if(col == 0){
			if(row % 2 == 0)
				cell_spacing = c_div + nps + c_div + buf + c_div + nps + c_div + buf + c_div + nps + c_div + buf + c_div + nps + c_div + buf + c_div +"\n";
			else
				cell_spacing = c_div + buf + c_div + nps + c_div + buf + c_div + nps + c_div + buf + c_div + nps + c_div + buf + c_div + nps + c_div +"\n";
			
			board_print = board_print + cell_spacing + c_div;
		}
		
		if(row % 2 == 0)
			board_print = board_print + nps + c_div + checker + c_div;
		else
			board_print = board_print + checker + c_div + nps + c_div;
		
		if(col == 3){
			board_print = board_print + " -" + to_string(row) + "-\n" + cell_spacing;
			
			if(row != 7)
				board_print = board_print + r_div;
			else
				board_print = board_print + boarder;
		}
	}
	
	return board_print;
}

void CheckerBoard::genStateCode(){
	string code = "";

	for(int i = 0; i < 32; i++){
		if(board[i] == nullptr)
			code = code + "0";
		else if(board[i]->getPlayer() == -1)
			code = code + "2";
		else
			code = code + "1";
	}

	code = code + "-";

	if(turn_player == -1)
		code = code + "2";
	else
		code = code + "1";

	if(winner == -1)
		code = code + "2";
	else
		code = code + to_string(winner);

	state_code = code;
}

bool CheckerBoard::operator==(const CheckerBoard &c){
	if(this->winner != c.winner)
		return false;
	
	if(this->turn_player != c.turn_player)
		return false;
	
	if(this->game_over != c.game_over)
		return false;
	
	bool match = true;
	
	for(int i = 0; i < 32; i++){
		if(this->board[i] == nullptr && c.board[i] == nullptr)
			continue;
		
		if(this->board[i] != nullptr && c.board[i] != nullptr)
			match = (*(this->board[i]) == *(c.board[i]));
		else
			match = false;
		
		if(!match)
			break;
	}
	
	return match;
}
