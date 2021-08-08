#include "Checker.h"
#include <string>

Checker::Checker(int player_num){
	this->player_num = player_num;
	this->type = false;
}

Checker::Checker(int player_num, bool type){
	this->player_num = player_num;
	this->type = type;
}

Checker::Checker(const Checker &c){
	this->player_num = c.player_num;
	this->type = c.type;
}

int Checker::getPlayer(){ return player_num; }

bool Checker::isKing(){ return type; }

void Checker::king_me() { type = true; }

string Checker::toString(){
	string p_str;
	string t_str;
	
	if(player_num > 0)
		p_str = "W";
	else
		p_str = "B";
	
	if(type)
		t_str = "K";
	else
		t_str = "N";
	
	string c_str = "[" + p_str + t_str + "]";
	
	return c_str;
}

bool Checker::operator==(const Checker &c){
	return ((this->player_num == c.player_num) && (this->type == c.type));
}
