#include "CompPlayer.h"
#include "UserPlayer.h"
#include "CheckerBoard.h"
#include <string>
#include <iostream>
#include <vector>

using namespace std;

int main(){

	int num_users = 0,
		eval_func = 0,
		search_type = 0,
		turn_count = 1,
		index = 0;
	bool state_exists = false,
		 tie = false;

	string cur_state = "";
	string exit_str;

	CheckerBoard cur_game;

	Player* player_1 = nullptr;
	Player* player_2 = nullptr;

	CompPlayer* cp = nullptr;
	UserPlayer* up = nullptr;

	vector<string> game_states;
	vector<int> game_state_hits;

	game_states.push_back(cur_game.getStateCode());
	game_state_hits.push_back(1);

	cout << "Specify the number of non-computer players in the game: ";
	cin >> num_users;

	for(int p = 1; p < 3; p++){
		if(num_users < p){
			cout << " \nWhich search algorithm do you want Player " << p << " to use?\n"
				 << "1) MINIMAX-A-B\n"
				 << "2) ALPHA-BETA\n"
				 << ">";
			cin >> search_type;

			while(search_type < 1 || search_type > 2){
				cout << "\nInvalid selection. Please select 1 or 2 from the list.\n"
					 << ">";
				cin >> search_type;
			}
            eval_func = 1;
			cp = new CompPlayer(p, search_type - 1, eval_func);

			if(p == 1)
				player_1 = cp;
			else
				player_2 = cp;

			cp = nullptr;
			search_type = 0;
			eval_func = 0;
		}
		else{
			up = new UserPlayer(p);

			if(p == 1){
				player_1 = up;
			}
			else{
				player_2 = up;
			}

			up = nullptr;
		}

	}

	do{
		if(turn_count % 2 == 1){
			player_1->makeMove(cur_game);
		}
		else{
			player_2->makeMove(cur_game);
		}

		cur_game.switchTurns();

		cur_state = cur_game.getStateCode();

		for(int s = 0; s < (int) game_states.size(); s++){
			if(game_states[s] == cur_state){
				cout << "Existing Game State Reached\n";

				state_exists = true;
				game_state_hits[s] += 1;

				if(game_state_hits[s] == 3){
					tie = true;
					cout << "Tie Made";
				}
				break;
			}
		}


		if(!state_exists){
			if(game_states.size() >= 24){
				index = turn_count % 24;

				game_states[index] = cur_state;
				game_state_hits[index] = 1;
			}
			else{
				game_states.push_back(cur_state);
				game_state_hits.push_back(1);
			}
		}

		state_exists = false;

		turn_count++;

	}while(cur_game.getWinner() == 0 && !tie);

	string winner = "";
        cout<< "Winner:"<< cur_game.getWinner() ;
	if(tie)
		winner = "None - DRAW";
	else if(cur_game.getWinner() == 1)
		winner = "PLAYER 1";
	else
		winner = "PLAYER 2";

	cout << "\n\nGAME RESULTS\n"
	     << "------------------------------\n"
		 << "Winner: " << winner << "\n"
		 << "Total # of Turns: " << turn_count - 1 << "\n\n"
		 << ">> ";
	player_1->printStats();
	cout << "\n>> ";
	player_2->printStats();
	cout << "Press a character to exit"
         << ">";
    cin  >> exit_str;
}


