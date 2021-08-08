#include "UserPlayer.h"
#include <iostream>

UserPlayer::UserPlayer(int player_num){
	

	if(player_num == 2 || player_num < 0)
		player_num = -1;
	else
		player_num = 1;
        this->player_num = player_num;
	this->moves_made = 0;
}

UserPlayer::UserPlayer(const UserPlayer& u){
	this->player_num = u.player_num;
	this->moves_made = u.moves_made;
}

int UserPlayer::getPlayerNum(){ return player_num; }

void UserPlayer::makeMove(CheckerBoard& game){
	int choice = 0,
		row = 0,
		col = 0,
		new_r = 0,
		new_c = 0,
		r_start = 0,
		r_end = 0,
		r_dir = 0,
		c_dir = 0,
		k_row = ((((-1 * player_num) + 1) / 2) + 7) % 8,
		move_s_val = 0,
		move_h_val = 0,
		back_val = 0;
	bool move_good = true,
		 king_piece = false,
		 can_single = false,
		 can_hop = false;

	Checker* selected_piece = nullptr;
	Checker* local_piece = nullptr;
	string color = "";

	if(player_num == 1)
		color = "W";
	else
		color = "B";

	cout << "Player " << to_string((player_num + 3) % 3) << " [" + color + "]\n";
	cout << game.toString() << "\n\n";

	do{
		do{
			do{
				cout << "Enter the row number for the checker you want to move: ";
				cin >> row;

				if(row < 0 || row > 7){
					cout << "\nInvalid row number entered.\n";
					row = -1;
				}
			}while(row == -1);

			do{
				cout << "Enter the column number for the checker you want to move: ";
				cin >> col;

				if(col < 0 || col > 7 || ((col - ((row + 1) % 2)) % 2 != 0)){
					cout << "\nInvalid column number entered.\n";
					col = -1;
				}
			}while(col == -1);

			selected_piece = game.getChecker(row, col);
                        cout<< "Row: "<<row;
                        cout<< "Col: "<<col;
                        cout<<"turn"<<selected_piece->getPlayer();
                        cout<<"ply_num"<<player_num;
			if(selected_piece == nullptr){
				cout << "\nNo checker is on that space.\n";
			}
			else if(selected_piece->getPlayer() != player_num){
				cout << "\nThe selected checker is your opponent's checker. Please choose one of your checker.\n";
			    selected_piece = nullptr;
			}
		}while(selected_piece == nullptr);

		king_piece = selected_piece->isKing();

		if(king_piece || player_num == -1)
			r_start = -1;
		else
			r_start = 1;

		if(king_piece || player_num == 1)
			r_end = 1;
		else
			r_end = -1;

		for(r_dir = r_start; r_dir < r_end + 1; r_dir += 2){
			new_r = row + r_dir;

			if(new_r < 0)
				continue;
			if(new_r > 7)
				break;

			for(c_dir = -1; c_dir < 2; c_dir+= 2){
				new_c = col + c_dir;

				if(new_c < 0)
					continue;
				if(new_c > 7)
					break;

				local_piece = game.getChecker(new_r, new_c);

				if(local_piece == nullptr){			//single move possible
					can_single = true;
					continue;
				}

				if(local_piece->getPlayer() == player_num)
					continue;

				if((new_r + r_dir) < 0 || (new_r + r_dir) > 7 || (new_c + c_dir) < 0 || (new_c + c_dir) > 7)
					continue;

				local_piece = game.getChecker(new_r + r_dir, new_c + c_dir);

				if(local_piece == nullptr)		//hop capture possible
					can_hop = true;

				local_piece = nullptr;
			}
		}

		cout << "\nWhat do you want to do?\n";

		back_val = 1;

		if(can_single){
			move_s_val = back_val;
			back_val++;
			cout << move_s_val << ") Move a single space\n";
		}

		if(can_hop){
			move_h_val = back_val;
			back_val++;
			cout << move_h_val << ") Hop-capture an enemy piece\n";

		}

		cout << back_val << ") Select a different checker piece\n";

		do{
			cout << ">";
			cin >> choice;

			if(choice == back_val)
				choice = 3;

			switch(choice){
				case 1:
				case 2: do{
							move_good = true;

							do{
								cout << "Enter the row number of the space you want to move to: ";
								cin >> new_r;

								if(new_r < 0 || new_r > 7){
									cout << "\nInvalid row number entered.\n";
									new_r = -1;
								}
							}while(new_r == -1);

							do{
								cout << "Enter the column number of the space you want to move to: ";
								cin >> new_c;

								if(new_c < 0 || new_c > 7 || ((new_c - ((new_r + 1) % 2)) % 2 != 0)){
									cout << "\nInvalid column number entered.\n";
									new_c = -1;
								}
							}while(new_c == -1);

							r_dir = new_r - row;
							c_dir = new_c - col;

							move_good = true;

							if(choice == move_s_val)
								choice = 1;
							else if(choice == move_h_val)
								choice = 2;

							if(abs(r_dir) != choice || abs(c_dir) != choice){
								cout << "\nCannot perform that action to that space.\n";
								move_good = false;
							}
							else if(choice == 1 && game.getChecker(new_r, new_c) != nullptr){
								cout << "\nSpace is already occupied by a piece. Please select another space.\n";
								move_good = false;
							}
							else if(choice == 2){
								r_dir /= 2;
								c_dir /= 2;

								local_piece = game.getChecker(row + r_dir, col + c_dir);

								if(local_piece == nullptr){
									cout << "\nCannot perform that action due to missing middle piece. Please select another space.\n";
									move_good = false;
								}
								else if((game.getChecker(new_r, new_c)) != nullptr){
									cout << "\nSpace is already occupied by a piece. Please select another space.\n";
									move_good = false;
								}
								else if(local_piece->getPlayer() == player_num){
									cout << "\nCannot hop and capture over your own piece. Please select another space.\n";
									move_good = false;
								}
							}

							if(move_good && choice == 1){
								game.moveSingle(row, col, r_dir, c_dir);

								local_piece = game.getChecker(new_r, new_c);
								king_piece = local_piece->isKing();

								if(!king_piece && new_r == k_row)
									local_piece->king_me();
							}
							else if(move_good && choice == 2){
								game.moveHopCapture(row, col, r_dir, c_dir);

								local_piece = game.getChecker(new_r, new_c);
								king_piece = local_piece->isKing();

								if(!king_piece && new_r == k_row){
									local_piece->king_me();
									choice = 1;
								}
								else if(game.getWinner() != 0)
									choice = 1;
								else{
									row = new_r;
									col = new_c;
									can_hop = false;

									if(king_piece || player_num == -1)
										r_start = -1;
									else
										r_start = 1;

									if(king_piece || player_num == 1)
										r_end = 1;
									else
										r_end = -1;

									for(r_dir = r_start; r_dir < r_end + 1; r_dir += 2){
										new_r = row + r_dir;

										if(new_r < 0)
											continue;
										if(new_r > 7)
											break;

										for(c_dir = -1; c_dir < 2; c_dir+= 2){
											new_c = col + c_dir;

											if(new_c < 0)
												continue;
											if(new_c > 7)
												break;

											local_piece = game.getChecker(new_r, new_c);

											if(local_piece == nullptr)
												continue;

											if(local_piece->getPlayer() == player_num)
												continue;

											if((new_r + r_dir) < 0 || (new_r + r_dir) > 7 || (new_c + c_dir) < 0 || (new_c + c_dir) > 7)
												continue;

											local_piece = game.getChecker(new_r + r_dir, new_c + c_dir);

											if(local_piece == nullptr)		//hop capture possible
												can_hop = true;

											local_piece = nullptr;
										}
									}

									if(!can_hop)
										choice = 1;
									else{
										cout << "\n" << game.toString() << "\n\n"
											 << "\nAnother hop can be made from new space.\n";
									}
								}
							}

						}while(!move_good || choice == 2);
						break;
				case 3: choice = -1;
					    break;
				default: cout << "\nInvalid choice given.\n";
						 choice = 0;
			}
		}while(choice == 0);

		move_s_val = 0;
		move_h_val = 0;
		back_val = 0;
	}while(choice == -1);

	moves_made++;
}

void UserPlayer::printStats(){
	int p_num = (player_num + 3) % 3;

	cout << "Player " << p_num << "- Stats\n"
		 << "User-driven Player\n"
		 << "Number of Turns: " << moves_made << "\n";
}
