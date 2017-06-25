#ifndef PLAYER
#define PLAYER

#include <string>
#include <queue>
#include <chrono>
#include <array>
#include "bomb.h"

class GameState;

class Player
{
	public:
		Player();
		Player(int _x, int _y);
		void setName(std::string _name);
		void setPlayerNumber(int _num);
		void move(GameState GS);
		void kill();
		void add_power_up(int power_up);
		void Player::setClientID(int _ID);
		int ClientID = -1;
		int x;
		int y;
		std::string name = "null";
		bool isNull;

		// Bomb information
		std::array<Bomb, 10> bombs{Bomb(),
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb(), 
			Bomb() };

		bool alive;
		bool invincible;

		int velocity;

		bool up;
		bool down;
		bool left;
		bool right;

		std::string direction;

		bool release_bomb;
		int bomb_radius;
		int bomb_limit;

		int sprite_width;
		int sprite_height;

		int playerNumber;
};

#endif