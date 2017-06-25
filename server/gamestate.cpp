/*
	ICS 168
	Project Server | gamestate.cpp
*/

#include "gamestate.h"
#include "utils.h"
#include <string>
#include<iostream>
#include<fstream>

using namespace std;

GameState::GameState()
{
    gameRunning = false;
}

GameState::GameState(std::string player1ID, std::string player2ID)
{
	this->player1ID = player1ID;
	this->player2ID = player2ID;
}

GameState::GameState(std::string player1ID, std::string player2ID, std::string player3ID)
{
	this->player1ID = player1ID;
	this->player2ID = player2ID;
	this->player3ID = player3ID;
}

GameState::GameState(std::string player1ID, std::string player2ID, std::string player3ID, std::string player4ID)
{
	this->player1ID = player1ID;
	this->player2ID = player2ID;
	this->player3ID = player3ID;
	this->player4ID = player4ID;
}

std::string GameState::getPlayerID(int player)
{
    switch(player){
        case 1: return this->player1ID;
        case 2: return this->player2ID;
        case 3: return this->player3ID;
        case 4: return this->player4ID;
        default: return "NULL"; // If Player's name couldn't be found.
    }
}
int GameState::PlayersInGame() 
{
	int total = 0;
	for (int i = 0; i<this->PlayerArrayLength;++i) 
	{
		if (this->playerArray[i].ClientID != -1) //-1 should be set for a nonconnected client
		{
			total++;
		}
	}
	return total;
}
int GameState::PlayerNumber(int _ClientID) 
{
	int PlayerNum = -1; //Set if unable to find number
	for (int i = 0; i<this->playerArray.size(); ++i)
	{
		if (this->playerArray[i].ClientID != -1 && this->playerArray[i].ClientID == _ClientID)
		{
			PlayerNum = i+1;
			break;
		}
	}
	return PlayerNum;
}
void GameState::setGameRunning(bool value) 
{
	this->gameRunning = value;
}

bool GameState::getGameRunning()
{
    return gameRunning;
}

bool GameState::getPlayer1Online()
{
	return this->player1Online;
}

bool GameState::getPlayer2Online()
{
	return this->player2Online;
}

bool GameState::getPlayer3Online()
{
	return this->player3Online;
}

bool GameState::getPlayer4Online()
{
	return this->player4Online;
}

void GameState::setPlayerID(int player, std::string id)
{
    switch(player) {
        case 1:
            this->player1ID = id;
            this->player1Online = true;
            break;
        case 2:
            this->player2ID = id;
            this->player2Online = true;
            break;
        case 3:
            this->player3ID = id;
            this->player3Online = true;
            break;
        case 4:
            this->player4ID = id;
            this->player4Online = true;
            break;
		default:
		    cout << "Error setting playerID: " << player << endl;
	        break;
	}
}
void GameState::setPlayerOnline(int num, bool value) 
{
	if (num == 1) 
	{
		setPlayer1Online(value);
	}
	else if (num == 2) 
	{
		setPlayer2Online(value);
	}
	else if (num == 3) 
	{
		setPlayer3Online(value);
	}
	else if (num == 4) 
	{
		setPlayer4Online(value);
	}
}
void GameState::setPlayer1Online(bool value)
{
	this->player1Online = value;
}

void GameState::setPlayer2Online(bool value)
{
	this->player2Online = value;
}

void GameState::setPlayer3Online(bool value)
{
	this->player3Online = value;
}

void GameState::setPlayer4Online(bool value)
{
	this->player4Online = value;
}

void GameState::resetIDs()
{
	this->player1ID = "Player 1";
	this->player2ID = "Player 2";
	this->player3ID = "Player 3";
  	this->player4ID = "Player 4";
}
void GameState::resetGameState() 
{
	for (int i = 0; i < 4; ++i) 
	{
		if (playerDeaths[i]!=-1) 
		{
			playerDeaths[i] = 0;
		}
	}
	this->playerArray =
	{
		Player(blockSize + 6,blockSize),
		Player(Width + 6 - 2 * blockSize,blockSize),
		Player(blockSize,Height + 6 - 2 * blockSize),
		Player(Width + 6 - 2 * blockSize,Height + 6 - 2 * blockSize)
	};
}
void GameState::resetID(int player)
{
    switch(player) {
        case 1:
            this->player1ID = "Player 1";
            break;
        case 2:
            this->player2ID = "Player 2";
            break;
        case 3:
            this->player3ID = "Player 3";
            break;
        case 4:
            this->player4ID = "Player 4";
            break;
		default:
		    cout << "Error resetting playerID: " << player << endl;
	        break;
	}
}

void GameState::resetGameRunning()
{
    gameRunning = false;
}

void GameState::logGameBoard() 
{
	for (int i = 0; i<15; i++)    //This loops on the rows.
	{
		for (int j = 0; j<15; j++) //This loops on the columns
		{
			cout << this->gameBoard[i][j] << "  ";
		}
		cout << endl;
	}
}

void GameState::logPowerUpsBoard()
{
	for (int i = 0; i<15; i++)    //This loops on the rows.
	{
		for (int j = 0; j<15; j++) //This loops on the columns
		{
			cout << this->boardPowerups[i][j] << "  ";
		}
		cout << endl;
	}
}

void GameState::loadPowerups() 
{
	for (int row = 0; row < 15; ++row)
	{
		for (int col = 0; col < 15; ++col)
		{
			if (this->level[row][col] == 1 && rand()%5 == 0) 
			{
				this->boardPowerups[row][col] = (rand() % 4)+1;
			}
			else 
			{
				this->boardPowerups[row][col] = 0;
			}
		}
	}
}
void GameState::resetGameBoard()
{
	for (int row = 0; row<15; ++row) 
	{
		for (int col = 0; col<15; ++col) 
		{
			if (row == 0 || row == 14 || col == 0 || col == 14)
			{
				this->gameBoard[row][col] = 2;
			}
			else if (row == 3 || row == 5 || row == 7 || row == 9 || row == 11)
			{
				this->gameBoard[row][col] = 1;
			}
			else if (row == 1 || row == 13)
			{
				if (col ==1||col == 2||col ==12||col == 13) 
				{
					this->gameBoard[row][col] = 0;
				}
				else { this->gameBoard[row][col] = 1; }
			}
			else if (row == 2||row == 12) 
			{
				if (col == 1||col ==13) 
				{
					this->gameBoard[row][col] = 0;
				}
				else if (col % 2 == 0) 
				{
					this->gameBoard[row][col] = 2;
				}
				else{ this->gameBoard[row][col] = 1; }
			}
			else {
				if (col % 2 == 0)
				{
					this->gameBoard[row][col] = 2;
				}
				else { this->gameBoard[row][col] = 1; }
			}
			
		}
	}

	loadPowerups();
}

void GameState::resetInit() 
{
	this->resetGameBoard();

	//ResetPlayerPositions
	this->playerArray[0].x = this->blockSize+6;
	this->playerArray[0].y = this->blockSize;
	this->playerArray[1].x = this->Width + 6 - 2 * this->blockSize;
	this->playerArray[1].y = this->blockSize;
    this->playerArray[2].x = this->blockSize;
	this->playerArray[2].y = this->Height + 6 - 2 * this->blockSize;
	this->playerArray[3].x = this->Width + 6 - 2 * this->blockSize;
    this->playerArray[3].y = this->Height + 6 - 2 * this->blockSize;

	this->paused = false;
	this->gameOver = false;
	this->menu = true;
	for (int i = 0; i < 4; ++i)
	{
		if (playerDeaths[i] != -1)
		{
			playerDeaths[i] = 0;
		}
	}
}

void GameState::reset()
{
	this->resetInit();
	this->resetIDs();
    this->resetGameRunning();
}
std::string GameState::stringPowerUpBoard() 
{
	std::string returnString = "";
	for (int i = 0; i<15; i++)    //This loops on the rows.
	{
		for (int j = 0; j<15; j++) //This loops on the columns
		{
			if (i == 14 && j == 14) 
			{
				returnString += std::to_string(this->boardPowerups[i][j]);
			}
			else 
			{
				returnString += std::to_string(this->boardPowerups[i][j]) + ",";
			}
		}
	}	
	return returnString;
}
void GameState::update() 
{
	//for (int i = 0; i < this->playerArray.size(); i++)
	for (int i = 0; i < 2;i++) 
	{
		// Remove exploded bombs (which can happen even when the player is dead)
		for (int j = 0; j < 10; j++)
		{
			if (!this->playerArray[i].bombs[j].isNull
				&& this->playerArray[i].bombs[j].extinguised)
			{
				this->playerArray[i].bombs[j].isNull = true;
			}
		}

		// Update bomb state (which can happen even when the player is dead)
		//for (int j = 0; j < this->playerArray[i].bombs.size(); j++)
		for (int j = 0; j < 10; j++) 
		{
			if (!this->playerArray[i].bombs[j].isNull) 
			{
				this->playerArray[i].bombs[j].updateBomb(*this);
			}
		}

		if (this->playerArray[i].alive) 
		{
			this->playerArray[i].move(*this);

			int boardX = Utils().bitmapPosition(this->playerArray[i].x + this->blockSize/2);
			int boardY = Utils().bitmapPosition(this->playerArray[i].y + this->blockSize / 2);
			int powerUp = this->boardPowerups[boardX][boardY]; //Check if this should be inverted

			if (powerUp != 0) 
			{
				this->boardPowerups[boardX][boardY] = 0;
				this->playerArray[i].add_power_up(powerUp);
			}

			// Release new bombs
			if (this->playerArray[i].release_bomb) {

				// Bombs are placed on board tiles according to the position of the center of the player
				int xBomb = Utils().pixel_position(Utils().bitmapPosition(this->playerArray[i].x + this->blockSize / 2));
				int yBomb = Utils().pixel_position(Utils().bitmapPosition(this->playerArray[i].y + this->blockSize / 2));

				// Add bomb to the player's set of "released bombs"
				int freeBombPosition = 0;
				
				for (int j = 0; j < 10; j++)
				{
					if (!this->playerArray[i].bombs[j].isNull)
					{
						freeBombPosition = j;
						break;
					}
				}
				this->playerArray[i].bombs[freeBombPosition] = Bomb(
					xBomb,
					yBomb,
					this->playerArray[i].bomb_radius);

				this->playerArray[i].release_bomb = false;
			}

		}
	}
}

