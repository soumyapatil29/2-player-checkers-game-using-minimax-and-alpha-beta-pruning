
#include "CompPlayer.h"
#include "CheckerBoard.h"
#include "Checker.h"
#include "Evaluate.h"
#include <chrono>
#include <string>
#include <iostream>
#include <iomanip>

CompPlayer::CompPlayer(int player_num){
	if(player_num == 2 || player_num < 0)
		player_num = -1;
	else
		player_num = 1;

	this->player_num = player_num;
	this->search_type = 0;
	this->eval_func = 0;
	this->moves_made = 0;
	this->nodes_gen = 0;
	this->nodes_exp = 0;
	this->func_dur = chrono::milliseconds(0);
}

CompPlayer::CompPlayer(int player_num, int search_type, int eval_func_num){
	if(player_num == 2 || player_num < 0)
		player_num = -1;
	else
		player_num = 1;

	this->player_num = player_num;
	this->search_type = search_type;

	if(this->search_type < 0 || this->search_type > 1)
		this->search_type = 0;

	this->eval_func = eval_func_num;
	if(eval_func_num < 0 || eval_func_num > 1)
		this->eval_func = 0;

	this->moves_made = 0;
	this->nodes_gen = 0;
	this->nodes_exp = 0;
	this->func_dur = chrono::milliseconds(0);
}

CompPlayer::CompPlayer(const CompPlayer &p){
	this->player_num = p.player_num;
	this->search_type = p.search_type;
	this->eval_func = p.eval_func;
	this->moves_made = p.moves_made;
	this->nodes_gen = p.nodes_gen;
	this->nodes_exp = p.nodes_exp;
	this->func_dur = p.func_dur;
}

int CompPlayer::getPlayerNum(){ return player_num; }

int CompPlayer::getSearchType(){ return search_type; }

int CompPlayer::getEvalFunct(){
	return this->eval_func;
}

int CompPlayer::evaluate(CheckerBoard& game){
	switch(this->eval_func){
		case 1: return evaluate_1(game, player_num);
		default: return evaluate_def(game, player_num);
	}
}

void CompPlayer::makeMove(CheckerBoard& game){
	Move* moves = nullptr;

	int c_row,
		c_col,
		r_dir,
		c_dir,
		value = 0,
		length = 0;

	if(nodes_gen != 0)
		nodes_gen--;

	auto f_start = chrono::high_resolution_clock::now();

	if(this->search_type == 0)
		minimax_a_b(game, moves, value, 0, length, player_num);
	else
		alpha_beta(game, moves, value, 0, length, -9999, player_num);

	auto f_end = chrono::high_resolution_clock::now();
	func_dur += chrono::duration_cast<chrono::milliseconds>(f_end - f_start);

	for(int i = 0; i < length; i++){
		c_row = moves[i].c_row;
		c_col = moves[i].c_col;
		r_dir = moves[i].r_dir;
		c_dir = moves[i].c_dir;

		if(moves[i].move_type == 0)
			game.moveSingle(c_row, c_col, r_dir, c_dir);
		else
			game.moveHopCapture(c_row, c_col, r_dir, c_dir);
	}

	if(length != 0)
		moves_made++;

	delete [] moves;
}

void CompPlayer::minimax_a_b(CheckerBoard& game, Move*& moves, int& value, int depth, int& length, int player){
	
        int cur_winner = game.getWinner();
	nodes_gen++;

	if(cur_winner != 0){
		if(cur_winner == player)
			value = 9999;
		else
			value = -9999;

		return;
	}

	if(depth > 2){
		value = evaluate(game);
		return;
	}

	int cur_value = -9999,
		num_opts = 0,
		c_row = 0,
		c_col = 0,
		r_dir = 0,
		c_dir = 0,
		k_row = ((((-1 * player) + 1) / 2) + 7) % 8,
		opt_value = 0,
		opt_move_cnt = 0;
		

	Move** opts = nullptr;
	Move* opt_moves = nullptr;
	Move* choice = nullptr;
	int* move_cnt = nullptr;


	CheckerBoard* pos_game = new CheckerBoard(game);
	Checker* local_piece = nullptr;

	num_opts = move_gen(*pos_game, opts, move_cnt, player);
        
	if(num_opts > 0)
		nodes_exp++;

	for(int i = 0; i < num_opts; i++){
		for(int m = 0; m < move_cnt[i]; m++){
			c_row = opts[i][m].c_row;
			c_col = opts[i][m].c_col;
			r_dir = opts[i][m].r_dir;
			c_dir = opts[i][m].c_dir;

			switch(opts[i][m].move_type){
				case 0: pos_game->moveSingle(c_row, c_col, r_dir, c_dir);
						local_piece = pos_game->getChecker(c_row + r_dir, c_col + c_dir);
						break;

				case 1: pos_game->moveHopCapture(c_row, c_col, r_dir, c_dir);
						local_piece = pos_game->getChecker(c_row + (2 * r_dir), c_col + (2 * c_dir));
						c_row += r_dir;
						break;
			}

			if(!(local_piece->isKing()) && (c_row + r_dir) == k_row)
				local_piece->king_me();

			local_piece = nullptr;
		}

		minimax_a_b(*pos_game, opt_moves, opt_value, depth + 1, opt_move_cnt, (player * -1));

		opt_value *= -1;
		delete [] opt_moves;

		if(opt_value > cur_value){
			cur_value = opt_value;

			delete [] choice;
			choice = opts[i];
			opts[i] = nullptr;

			length = move_cnt[i];
		}
		else
			delete [] opts[i];

               
		delete pos_game;
		pos_game = new CheckerBoard(game);
	}

	if(num_opts != 0){

		moves = choice;
		value = cur_value;
	}
	else{
		moves = nullptr;
		length = 0;
		value = -9999;
	}

	choice = nullptr;
	delete [] opts;
	delete [] move_cnt;
	delete pos_game;
}

void CompPlayer::alpha_beta(CheckerBoard& game, Move*& moves, int& value, int depth, int& length, int a_b, int player){
	int cur_winner = game.getWinner();
	nodes_gen++;

	if(cur_winner != 0){
		if(cur_winner == player)
			value = 9999;
		else
			value = -9999;

		return;
	}

	if(depth > 2){
		value = evaluate(game);
		return;
	}

	int cur_value = -9999,
		num_opts = 0,
		c_row = 0,
		c_col = 0,
		r_dir = 0,
		c_dir = 0,
		k_row = ((((-1 * player) + 1) / 2) + 7) % 8,
		opt_value = 0,
		opt_move_cnt = 0;
		

	Move** opts = nullptr;
	Move* opt_moves = nullptr;
	Move* choice = nullptr;
	int* move_cnt = nullptr;


	a_b *= -1;

	CheckerBoard* pos_game = new CheckerBoard(game);
	Checker* local_piece = nullptr;

	num_opts = move_gen(*pos_game, opts, move_cnt, player);

	if(num_opts > 0)
		nodes_exp++;

	for(int i = 0; i < num_opts; i++){
		for(int m = 0; m < move_cnt[i]; m++){
			c_row = opts[i][m].c_row;
			c_col = opts[i][m].c_col;
			r_dir = opts[i][m].r_dir;
			c_dir = opts[i][m].c_dir;

			switch(opts[i][m].move_type){
				case 0: pos_game->moveSingle(c_row, c_col, r_dir, c_dir);
						local_piece = pos_game->getChecker(c_row + r_dir, c_col + c_dir);
						break;

				case 1: pos_game->moveHopCapture(c_row, c_col, r_dir, c_dir);
						local_piece = pos_game->getChecker(c_row + (2 * r_dir), c_col + (2 * c_dir));
						c_row += r_dir;
						break;
			}

			if(!(local_piece->isKing()) && (c_row + r_dir) == k_row)
				local_piece->king_me();

			local_piece = nullptr;
		}

		alpha_beta(*pos_game, opt_moves, opt_value, depth + 1, opt_move_cnt, cur_value, (player * -1));

		opt_value *= -1;
		delete [] opt_moves;

		if(opt_value > cur_value){
			cur_value = opt_value;

			delete [] choice;
			choice = opts[i];
			length = move_cnt[i];
		}
		else
			delete [] opts[i];

		opts[i] = nullptr;

		if(cur_value > a_b)
			break;

		delete pos_game;
		pos_game = new CheckerBoard(game);
	}

	
	if(num_opts != 0){

		moves = choice;
		value = cur_value;
	}
	else{
		moves = nullptr;
		length = 0;
		value = -9999;
	}

	choice = nullptr;
	delete [] opts;
	delete [] move_cnt;
	delete pos_game;
}

int CompPlayer::move_gen(CheckerBoard& game, Move**& opts, int*& move_cnt, int player){
	Checker** board = game.getBoard();
	CheckerBoard* proxy = new CheckerBoard(game);

	Checker* piece = nullptr;

	Move* new_move = nullptr;
	Move** new_move_set = nullptr;

	int* new_move_cnt = nullptr;

	int pieces = 0,
		k_row = ((((-1 * player) + 1) / 2) + 7) % 8,
		c_row = 0,
		c_col = 0,
		r_start = 0,
		r_end = 0,
		cur_row = 0,
		cur_col = 0,
		local_index = 0,
		cur_length = 0;
	bool king_piece = false,
		 kinged = false;

	for(int i = 0; i < 32 && pieces < 12; i++){
		if(board[i] == nullptr)
			continue;
		if(board[i]->getPlayer() != player)
			continue;

		c_row = i / 4;
		c_col = (2 * (i - (c_row * 4))) + ((c_row + 1) % 2);

		king_piece = board[i]->isKing();

		if(king_piece || player == -1)
			r_start = -1;
		else
			r_start = 1;

		if(king_piece || player == 1)
			r_end = 1;
		else
			r_end = -1;

		for(int r_dir = r_start; r_dir < r_end + 1; r_dir += 2){
			cur_row = c_row + r_dir;

			if(cur_row < 0)
				continue;
			if(cur_row > 7)
				break;

			for(int c_dir = -1; c_dir < 2; c_dir+= 2){
				cur_col = c_col + c_dir;

				if(cur_col < 0)
					continue;
				if(cur_col > 7)
					break;

				local_index = (cur_col / 2) + (cur_row * 4);

				if(board[local_index] == nullptr){			//single move possible
					new_move_set = new Move*[cur_length + 1];
					new_move_cnt = new int[cur_length + 1];
					new_move = new Move[1];

					new_move[0].c_row = c_row;
					new_move[0].c_col = c_col;
					new_move[0].r_dir = r_dir;
					new_move[0].c_dir = c_dir;
					new_move[0].move_type = 0;

					for(int move_index = 0; move_index < cur_length; move_index++){
						new_move_set[move_index] = opts[move_index];
						new_move_cnt[move_index] = move_cnt[move_index];
						opts[move_index] = nullptr;
					}

					new_move_set[cur_length] = new_move;
					new_move_cnt[cur_length] = 1;
					new_move = nullptr;

					delete [] opts;
					delete [] move_cnt;

					opts = new_move_set;
					move_cnt = new_move_cnt;

					new_move_set = nullptr;
					new_move_cnt = nullptr;

					cur_length++;

					continue;
				}

				if(board[local_index]->getPlayer() == player)
					continue;

				if((cur_row + r_dir) < 0 || (cur_row + r_dir) > 7 || (cur_col + c_dir) < 0 || (cur_col + c_dir) > 7)
					continue;

				local_index = ((c_row + r_dir) * 4) + ((c_col + c_dir) / 2);

				if(board[local_index] == nullptr){			//hop capture possible
					new_move_set = new Move*[cur_length + 1];
					new_move_cnt = new int[cur_length + 1];
					new_move = new Move[1];

					new_move[0].c_row = c_row;
					new_move[0].c_col = c_col;
					new_move[0].r_dir = r_dir;
					new_move[0].c_dir = c_dir;
					new_move[0].move_type = 1;

					proxy->moveHopCapture(c_row, c_col, r_dir, c_dir);

					piece = proxy->getChecker(cur_row + r_dir, cur_col + c_dir);

					if(!king_piece && (cur_row + r_dir) == k_row){
						piece->king_me();
						kinged = true;
					}

					if(!kinged && proxy->getWinner() == 0)
						genHopCaptureMoveSet(*proxy, opts, move_cnt, new_move, cur_length, 1, player, cur_row + r_dir, cur_col + c_dir);
					else{
						for(int move_index = 0; move_index < cur_length; move_index++){
							new_move_set[move_index] = opts[move_index];
							new_move_cnt[move_index] = move_cnt[move_index];
							opts[move_index] = nullptr;
						}

						new_move_set[cur_length] = new_move;
						new_move_cnt[cur_length] = 1;

						delete [] opts;
						delete [] move_cnt;

						opts = new_move_set;
						move_cnt = new_move_cnt;

						new_move_set = nullptr;
						new_move_cnt = nullptr;

						cur_length++;
					}

					new_move = nullptr;
					delete proxy;

					proxy = new CheckerBoard(game);
				}
			}
		}
	}

	for(int i = 0; i < 32; i++)
		delete board[i];
	delete [] board;

	delete proxy;

	return cur_length;
}

void CompPlayer::genHopCaptureMoveSet(CheckerBoard& game, Move**& opts, int*& move_cnt, Move* move_path, int& set_length, int path_length, int player, int c_row, int c_col){
	CheckerBoard* proxy = new CheckerBoard(game);
	Checker* piece = proxy->getChecker(c_row, c_col);

	Move** new_move_set = nullptr;
	Move* new_move = nullptr;

	int* new_move_cnt = nullptr;

	int k_row = ((((-1 * player) + 1) / 2) + 7) % 8,
		r_start = 1,
		r_end = 1,
		cur_row = 0,
		cur_col = 0;

	bool king_piece = piece->isKing(),
		 kinged = false,
		 more_moves = false;

	if(king_piece || player == -1)
			r_start = -1;
		else
			r_start = 1;

		if(king_piece || player == 1)
			r_end = 1;
		else
			r_end = -1;

	for(int r_dir = r_start; r_dir < r_end + 1; r_dir += 2){
		cur_row = c_row + r_dir;

		if(cur_row < 0 || cur_row + r_dir < 0)
			continue;
		if(cur_row > 7 || cur_row + r_dir > 7)
			break;

		for(int c_dir = -1; c_dir < 2; c_dir += 2){
			cur_col = c_col + c_dir;

			if(cur_col < 0 || cur_col + c_dir < 0)
				continue;
			if(cur_col > 7 || cur_col + c_dir > 7)
				break;

			piece = proxy->getChecker(cur_row, cur_col);

			if(piece == nullptr)
				continue;
			if(piece->getPlayer() == player)
				continue;

			piece = proxy->getChecker(cur_row + r_dir, cur_col + c_dir);

			if(piece != nullptr)
				continue;

			new_move_cnt = new int[set_length + 1];

			new_move = new Move[path_length + 1];

			for(int i = 0; i < path_length; i++){
				new_move[0].c_row = move_path[i].c_row;
				new_move[0].c_col = move_path[i].c_col;
				new_move[0].r_dir = move_path[i].r_dir;
				new_move[0].c_dir = move_path[i].c_dir;
				new_move[0].move_type = 1;
			}

			new_move[path_length].c_row = c_row;
			new_move[path_length].c_col = c_col;
			new_move[path_length].r_dir = r_dir;
			new_move[path_length].c_dir = c_dir;
			new_move[path_length].move_type = 1;

			more_moves = true;

			proxy->moveHopCapture(c_row, c_col, r_dir, c_dir);
			piece = proxy->getChecker(cur_row + r_dir, cur_col + c_dir);

			if(!king_piece && (cur_row + r_dir) == k_row){
				piece->king_me();
				kinged = true;
			}

			if(!kinged && proxy->getWinner() == 0)
				genHopCaptureMoveSet(*proxy, opts, move_cnt, new_move, set_length, path_length + 1, player, cur_row + r_dir, cur_col + c_dir);
			else{
				new_move_set = new Move*[set_length + 1];

				for(int move_index = 0; move_index < set_length; move_index++){
					new_move_set[move_index] = opts[move_index];
					new_move_cnt[move_index] = move_cnt[move_index];
					opts[move_index] = nullptr;
				}

				new_move_set[set_length] = new_move;
				new_move_cnt[set_length] = path_length + 1;

				delete [] opts;
				delete [] move_cnt;

				opts = new_move_set;
				move_cnt = new_move_cnt;

				new_move_set = nullptr;
				new_move_cnt = nullptr;

				set_length++;
			}

			new_move = nullptr;
			delete proxy;

			proxy = new CheckerBoard(game);

			kinged = false;
		}
	}

	if(!more_moves){
		new_move_set = new Move*[set_length + 1];

		for(int move_index = 0; move_index < set_length; move_index++){
			new_move_set[move_index] = opts[move_index];
			new_move_cnt[move_index] = move_cnt[move_index];
			opts[move_index] = nullptr;
		}

		new_move_set[set_length] = move_path;
		new_move_cnt[set_length] = path_length;

		delete [] opts;
		delete [] move_cnt;

		opts = new_move_set;
		move_cnt = new_move_cnt;

		new_move_set = nullptr;
		new_move_cnt = nullptr;

		set_length++;
	}

	delete proxy;
}

void CompPlayer::printStats(){
	int p_num = (player_num + 3) % 3;

	string st = "";
	if(search_type == 0)
		st = "MINIMAX_A_B";
	else
		st = "ALPHA_BETA";

	cout << "Player " << p_num << " - Stats\n"
		 << "Computer-driven Player\n"
		 << "Search Type: " << st << "\n"
		 << "Evaluation Function #: " << getEvalFunct() << "\n"
		 << "Number of Turns: " << moves_made << "\n"
		 << "Nodes Generated: " << nodes_gen << "\n"
		 << "Nodes Expanded: " << nodes_exp << "\n";
	cout << fixed << setprecision(4)
		 << "Ave. Search Function Execution Time: " << (func_dur.count() / (float) moves_made) << "ms.\n";
}
