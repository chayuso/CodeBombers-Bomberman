#ifndef BOMB
#define BOMB

#include <string>

class GameState;

class Bomb
{
	public:
		Bomb();
		Bomb(int _x, int _y, int _radius);
		void updateBomb(GameState GS);
		void updateState();
		void destroyBlocks(GameState GS);
		void killPlayers(GameState GS);
		long long getTime();
		long long currentTime();
		int x;
		int y;
		bool isNull;

		int state;
		// States:
		// 0 = bomb_small
		// 1 = bomb_medium
		// 2 = bomb_large
		// 3 = exploding

		bool explodes;
		bool extinguised;

	private:
		int radiusUp;
		int radiusDown;
		int radiusLeft;
		int radiusRight;

		long long startTime;
		long long timeElapsed;
};

#endif

