/*
	ICS 168
	Project Server | gamestate.h
*/

#ifndef GAMESTATE
#define GAMESTATE

#define PLAYER_1 1
#define PLAYER_2 2
#define PLAYER_3 3
#define PLAYER_4 4


#include <string>
#include <set>
#include "player.h"

class GameState 
{
	public:
		GameState(); // default constructor
		GameState(std::string player1ID, std::string player2ID); // constructor if you know two ids beforehand
		GameState(std::string player1ID, std::string player2ID, std::string player3ID); // constructor if you know three ids beforehand
		GameState(std::string player1ID, std::string player2ID, std::string player3ID, std::string player4ID); // constructor if you know four ids beforehand

		void update();

		std::string getPlayerID(int player); // returns the id for a player
		std::string lobbyName = "<None>";
		void setPlayerOnline(int num, bool value);
		bool getPlayer1Online();
		void setPlayer1Online(bool value);
		bool getPlayer2Online();
   		void setPlayer2Online(bool value);
   		bool getPlayer3Online();
        void setPlayer3Online(bool value);
        bool getPlayer4Online();
        void setPlayer4Online(bool value);
        bool getGameRunning();
		bool LobbyGameStarted = false;
		void setPlayerID(int player, std::string id); // sets player ID for a player.
    
		void setGameRunning(bool value);
		void resetInit(); //Also used as Init().

		void reset(); // resets ids to empty string and gamerunning
		void resetIDs(); // resets only IDs to empty string
		void resetID(int player); // resets ID for only a single player
        void resetGameRunning();
		void resetGameState();//called when all clients dc

		void loadPowerups();
		void resetGameBoard();
		void logGameBoard(); //prints the game board
		void logPowerUpsBoard(); //prints the powerups game board
		int PlayersInGame();
		std::string stringPowerUpBoard();
		int playerDeaths[4]{-1,-1,-1,-1};//0 alive, 1 dead, -1 notconnected
		int gameBoard[15][15];	//gameboard changes
		
		int boardPowerups[15][15];
		bool isActive = false;
		int round = 0;
		int playersInGameOnStart = 0;
		int blockSize = 40;
		int Width = 600;
		int Height = 600;
		int PlayerArrayLength = 4; //Note: Manually change this number if below number is changed 
		int PlayerNumber(int _ClientID);//1-4
		std::array<Player, 4> playerArray
		{
			Player(blockSize+6,blockSize),
			Player(Width + 6 - 2 * blockSize,blockSize),
			Player(blockSize,Height + 6 - 2 * blockSize),
			Player(Width + 6 - 2 * blockSize,Height + 6 - 2 * blockSize)
		};
		std::string timeRecord[15][15];
		int playerGetPowerUp[15][15];

		std::array<int, 4> spectatorArray = { -1,-1,-1,-1 }; //Capped 4 slots

		bool paused = false;
		bool menu = true;
		bool gameOver = false;
		std::string gameOverWinner;
		bool gameRunning;  // When true, the server will keep sending message to tell clients to update their images




	private:
		std::string player1ID = "Player 1";
		std::string player2ID = "Player 2";
		std::string player3ID = "Player 3";
   		std::string player4ID = "Player 4";

		std::string powerups[5] =
		{
			"none","fire","skate","geta","bomb"
		};

		bool player1Online = false; //True when first client connects
		bool player2Online = false; //True when second client connects
		bool player3Online = false; //True when third client connects
		bool player4Online = false; //True when fourth client connects

		int  level[15][15] = //level never changes
		{
			{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 },
			{ 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2 },
			{ 2, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0, 2 },
			{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
			{ 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 },
			{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
			{ 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 },
			{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
			{ 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 },
			{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
			{ 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2 },
			{ 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2 },
			{ 2, 0, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 0, 2 },
			{ 2, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 2 },
			{ 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2 }
		}; 
};

#endif

