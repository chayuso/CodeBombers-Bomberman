
/*
ICS 168 - CodeBombers
Project Server | main.cpp
*/

#include "websocket.h" // Library authored by the TA.
#include "json11.hpp" // Open-source C++ JSON parser library by MIT at https://github.com/dropbox/json11
#include "gamestate.h" // Class created to keep track of game state.
#include "latency.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <fstream> 
#include <sstream>
#include <streambuf>
#include <set>

using namespace std;

typedef json11::Json JSON;

/* Begin Variables, Structs & Classes */

webSocket server;
std::array<GameState, 4> gameStateArray
{
	GameState(),//0
	GameState(),//1
	GameState(),//2
	GameState(),//3
};

//GameState gameState;


int positionCommunicationSpeed = 500; //Milliseconds
int InGameLimit = 100; // not used yet
std::set<int> clientsInGame;

std::ifstream infile("database.txt");

std::string strFile((std::istreambuf_iterator<char>(infile)),
	std::istreambuf_iterator<char>());

std::ofstream outfile("database.txt");

void updatePositionCommunicationThrottle()
{
	int inGameClinetsSize = clientsInGame.size();
	if (inGameClinetsSize == 0 || inGameClinetsSize >= 400) //cap
	{
		positionCommunicationSpeed = 500; //default value
	}
	else
	{
		// Pattern:
		// 1 Client = 101
		// 2 Clients = 102
		// 3 Clients = 103
		// etc...
		positionCommunicationSpeed = 100 + (inGameClinetsSize * 1);
	}

	JSON msg_obj = JSON::object{ { "event", "updateCommunicationThrottleEvent" },
	{ "communicationSpeed", std::to_string(positionCommunicationSpeed) }, };
	//Only throttle clients in game
	for (std::set<int>::iterator it = clientsInGame.begin(); it != clientsInGame.end(); ++it)
	{
		int clientID = *it;
		lat_msg.push(info(clientID, msg_obj.dump()));
	}
}

int getClientGameNumber(int _clientID)
{
	int clientGameStateNumber = -1;//-1 if the client hasn't joined a game
	for (int i = 0; i < gameStateArray.size(); i++)
	{
		if (clientGameStateNumber == -1)
		{
			for (int j = 0; j < gameStateArray[i].playerArray.size(); ++j)
			{
				if (gameStateArray[i].playerArray[j].ClientID == _clientID)
				{
					clientGameStateNumber = i;
					break;
				}
			}
		}
	}
	return clientGameStateNumber;
}

void addInGameClient(int clientID)
{
	clientsInGame.insert(clientID);
	updatePositionCommunicationThrottle();
}

void removeInGameClient(int clientID)
{
	clientsInGame.erase(clientID);
	updatePositionCommunicationThrottle();
}

/* Begin Helpers */

void log(string message)
{
	std::cout << "[LOG] " << message << std::endl;
}

void new_event(string message) // helps distinguish game state changes from log messages
{
	std::cout << "[EVENT] " << message << std::endl;
}

bool contains(std::vector<int> clientIDs, int clientID)
{
	return std::find(clientIDs.begin(), clientIDs.end(), clientID) != clientIDs.end();
}

/*Custom Server Event Senders*/
void sendDestroyAllBlocksCustomEvent(int clientID, int gameStateNumber)
{
	JSON msg_obj = JSON::object{ { "event", "destroyAllBlocksCustomEvent" }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("sendDestroyAllBlocksCustomEvent sent for client ID " + std::to_string(clientID) + " (player# = " + std::to_string(gameStateArray[gameStateNumber].PlayerNumber(clientID)) + ").");

}

/* Begin Event Senders */

void sendUpdatePlayerNumberEvent(int clientID, int player)
{
	int gameNumber = -1;
	for (int i = 0; i<gameStateArray.size(); ++i)
	{
		if (gameNumber == -1)
		{
			for (int j = 0; j<gameStateArray[i].playerArray.size(); ++j)
			{
				if (gameStateArray[i].playerArray[j].ClientID == clientID)
				{
					gameNumber = i;
					break;
				}
			}
		}
	}
	if (gameNumber == -1)
	{
		log("Unknown sendUpdatePlayerNumberEvent Error");
	}
	int active = -1;
	if (gameStateArray[gameNumber].LobbyGameStarted)
	{
		active = 1;
	}
	else if (!gameStateArray[gameNumber].LobbyGameStarted)
	{
		active = 0;
	}
	JSON msg_obj = JSON::object{ { "event", "updatePlayerNumberEvent" },
	{ "player", std::to_string(player) },
	{ "inGame", std::to_string(active) }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("sendUpdatePlayerNumberEvent sent for client ID " + std::to_string(clientID) + " (player# = " + std::to_string(player) + ").");
}

void sendPlayerDisconnectEvent(int clientID, int player)
{
	JSON msg_obj = JSON::object{ { "event", "playerDisconnectEvent" },
	{ "player", std::to_string(player) }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("playerDisconnectEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " disconnected.)");
}

void sendPlayerConnectedEvent(int clientID, int player, std::string id)
{
	JSON msg_obj = JSON::object{ { "event", "playerConnectedEvent" },
	{ "player", std::to_string(player) },
	{ "id", id }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("playerConnectedEvent sent for client ID " + std::to_string(clientID) + " (player #" + std::to_string(player) + " connected.)");
}

void sendGameStartedEvent(int clientID)
{
	JSON msg_obj = JSON::object{ { "event", "gameStartedEvent" } };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("gameStartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

// Send when player enters wrong password.
void sendWrongPasswordEvent(int clientID)
{
	JSON msg_obj = JSON::object{ { "event", "wrongPasswordEvent" } };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("wrongPasswordEvent sent for client ID " + std::to_string(clientID) + ")");
}

// Send when player enters a username that does not exist. Asks for new creation of account.
void sendCreateNewAccountEvent(int clientID)
{
	JSON msg_obj = JSON::object{ { "event", "createNewAccountEvent" } };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("createNewAccountEvent sent for client ID " + std::to_string(clientID) + ")");
}


void sendMenuScreenRestartedEvent(int clientID, int gameStateNumber)
{
	std::string powerUps = gameStateArray[gameStateNumber].stringPowerUpBoard();
	JSON msg_obj = JSON::object{ { "event", "menuScreenRestartedEvent" }, 
								 { "boardPowerUps", powerUps } , };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("menuScreenRestartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

void sendMenuScreenStartedEvent(int clientID,int gameStateNumber)
{
	std::string powerUps = gameStateArray[gameStateNumber].stringPowerUpBoard();
	JSON msg_obj = JSON::object{ { "event", "menuScreenStartedEvent" } ,
								{ "boardPowerUps", powerUps } , };
	lat_msg.push(info(clientID, msg_obj.dump()));
	new_event("menuScreenStartedEvent sent for client ID " + std::to_string(clientID) + ")");
}

void sendGameFinishedEvent(int clientID, int pWinner)
{
	clientsInGame.erase(clientID); //Clients readjust throttle manually when came ends
	JSON msg_obj = JSON::object{ { "event", "gameFinishedEvent" },
	{ "winner",  pWinner }, };
	lat_msg.push(info(clientID, msg_obj.dump()));
	new_event("gameFinishedEvent sent to client: " + std::to_string(clientID));
}

void sendNetworkConnectedEvent(int clientID, std::string id)
{
	JSON msg_obj = JSON::object{ { "event", "networkConnectedEvent" },
	{ "id", id }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("clientNetworkConnectedEvent sent for client ID " + std::to_string(clientID) + " (client" + id + " connected.)");

}

void sendPlayerDeathEvent(int clientID, int player, int gameStateNumber)
{
	log("Deaths");
	for (int i = 0; i<4; ++i)
	{
		log(std::to_string(gameStateArray[gameStateNumber].playerDeaths[i]));
	}
	int gameOver = 0;
	int winner = -1;
	gameStateArray[gameStateNumber].playerDeaths[player - 1] = 1;

	int total = 0;
	for (int i = 0; i < 4; ++i)
	{
		if (gameStateArray[gameStateNumber].playerDeaths[i] == 1)
		{
			++total;
		}
	}

	if (total == gameStateArray[gameStateNumber].playersInGameOnStart - 1)
	{

		gameStateArray[gameStateNumber].setGameRunning(false);
		gameOver = 1;
		gameStateArray[gameStateNumber].isActive = false;
		for (int i = 0; i < 4; ++i)
		{
			if (gameStateArray[gameStateNumber].playerDeaths[i] == 0)
			{
				winner = i + 1;
				break;
			}
		}
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				clientsInGame.erase(gameStateArray[gameStateNumber].playerArray[i].ClientID);
			}
		}
		updatePositionCommunicationThrottle();
	}
	else if (total == gameStateArray[gameStateNumber].playersInGameOnStart)
	{
		gameStateArray[gameStateNumber].setGameRunning(false);
		gameStateArray[gameStateNumber].isActive = false;
		gameOver = 1;
		winner = 0;//tie
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				clientsInGame.erase(gameStateArray[gameStateNumber].playerArray[i].ClientID);
			}
		}
		updatePositionCommunicationThrottle();

	}
	JSON msg_obj = JSON::object{ { "event", "playerDeathEvent" },
	{ "player",  player },
	{ "gameOver", gameOver },
	{ "winner", winner }, };
	lat_msg.push(info(clientID, msg_obj.dump()));
}

void sendLatencyEstimationEvent(int clientID, int latencyID, long long X, int gameStateNumber)
{
	JSON msg_obj = JSON::object{ { "event", "latencyEstimationEvent" },
	{ "id", std::to_string(latencyID) },
	{ "X",  std::to_string(X) },
	{ "Y",  std::to_string(currentTime()) },
	{ "player1X",  std::to_string(gameStateArray[gameStateNumber].playerArray[0].x) },
	{ "player1Y",  std::to_string(gameStateArray[gameStateNumber].playerArray[0].y) },
	{ "player2X",  std::to_string(gameStateArray[gameStateNumber].playerArray[1].x) },
	{ "player2Y",  std::to_string(gameStateArray[gameStateNumber].playerArray[1].y) },
	{ "player3X",  std::to_string(gameStateArray[gameStateNumber].playerArray[2].x) },
	{ "player3Y",  std::to_string(gameStateArray[gameStateNumber].playerArray[2].y) },
	{ "player4X",  std::to_string(gameStateArray[gameStateNumber].playerArray[3].x) },
	{ "player4Y",  std::to_string(gameStateArray[gameStateNumber].playerArray[3].y) }, };
	lat_msg.push(info(clientID, msg_obj.dump()));
}

void sendPlayerChatEvent(int clientID, std::string player, std::string msg)
{
	JSON msg_obj = JSON::object{ { "event", "playerChatEvent" },
	{ "player", player },
	{ "msg", msg }, };
	lat_msg.push(info(clientID, msg_obj.dump(), true));

	new_event("playerChateEvent sent for client ID " + std::to_string(clientID) + ": " + msg);
}

void sendStatistics(int clientID)
{
	std::ifstream infile("database.txt");
	std::string line;
	while (std::getline(infile, line))
	{
		std::string name, password, wins, losses, ties, deaths, kills, bombs;
		std::istringstream iss(line);
		std::getline(iss, name, '\t');
		std::getline(iss, password, '\t');
		std::getline(iss, wins, '\t');
		std::getline(iss, losses, '\t');
		std::getline(iss, ties, '\t');
		std::getline(iss, deaths, '\t');
		std::getline(iss, kills, '\t');
		iss >> bombs;

		JSON msg_obj = JSON::object{ { "event", "statisticsEvent" },
		{ "player", name },{ "wins", wins },{ "losses", losses },{ "ties", ties },{ "deaths",deaths },{ "kills",kills }
		,{ "bombs",bombs }, };
		lat_msg.push(info(clientID, msg_obj.dump(), false, true));
	}
	new_event("Statistics sent to client ID " + std::to_string(clientID));
}

void sendLobbyList(int clientID)
{
	for (int i = 0; i < gameStateArray.size(); ++i)
	{
		int playerCount = 0;
		for (int j = 0; j < gameStateArray[i].playerArray.size(); ++j)
		{
			if (gameStateArray[i].playerArray[j].ClientID != -1)
			{
				++playerCount;
			}
		}
		JSON msg_obj = JSON::object{ { "event", "refreshLobbyListEvent" },
		{ "name", gameStateArray[i].lobbyName },
		{ "players", std::to_string(playerCount) },
		{ "gameStatus", ((gameStateArray[i].LobbyGameStarted) ? "inGame" : "inLobby") }, };
		lat_msg.push(info(clientID, msg_obj.dump(), false, true));
	}
	new_event("Lobby List sent to client ID " + std::to_string(clientID));
}

int matchIdPw(std::string id, std::string pw)
{
	std::ifstream infile("database.txt");
	std::string line;
	while (std::getline(infile, line))
	{
		std::stringstream linestream(line);
		std::string pid;
		std::string ppw;
		std::getline(linestream, pid, '\t');
		linestream >> ppw;
		if (pid == id)
		{
			if (ppw == pw)
				return 1;
			else
			{
				return 0;
			}
		}
	}
	return -1;
}

void sendBombPlacementEvent(int clientID, int player, int xBomb, int yBomb, std::string sentTime, long long recievedTime)
{
	long long YX = currentTime() - recievedTime;
	JSON msg_obj = JSON::object{ { "event", "bombPlacementEvent" },
	{ "player", std::to_string(player) },
	{ "xBomb",  std::to_string(xBomb) },
	{ "yBomb",  std::to_string(yBomb) },
	{ "serverBombElapsedTime",  std::to_string(YX) },
	{ "clientSentTime",  sentTime }, };
	lat_msg.push(info(clientID, msg_obj.dump()));
	new_event("Server Sent Bomb. Client: " + std::to_string(clientID) + "; Player Number: " + std::to_string(player));
}

void sendPowerUpsEvent(int clientID, int player, int xPowerUp, int yPowerUp, std::string sentTime, int powerUp, std::string addOrReduce)
{
	JSON msg_obj = JSON::object{ { "event", "powerUpsEvent" },
	{ "player", std::to_string(player) },
	{ "xPowerUp", std::to_string(xPowerUp) },
	{ "yPowerUp", std::to_string(yPowerUp) },
	{ "clientSentTime", sentTime },
	{ "powerUp", powerUp },
	{ "addOrReduce", addOrReduce }, };
	lat_msg.push(info(clientID, msg_obj.dump()));
}

/* Begin Event Handlers */
void  bombPlacementEventHandler(int clientID, int player, int xBomb, int yBomb, std::string clientSentTime, long long serverRecievedTime, int gameStateNumber)
{												//player = 1-4
	new_event("bombPlacementEventHandler fired.");

	for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)//Send to all Players
	{
		if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
		{
			sendBombPlacementEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, player, xBomb, yBomb, clientSentTime, serverRecievedTime);
		}
	}
	if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
	{
		for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
			{
				sendBombPlacementEvent(gameStateArray[gameStateNumber].spectatorArray[i], player, xBomb, yBomb, clientSentTime, serverRecievedTime);
			}
		}
	}
}
void playerDeathHandler(int player, int gameStateNumber)
{
	new_event("playerDeathEventHandler fired.");
	for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
	{
		if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
		{
			sendPlayerDeathEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, player, gameStateNumber);
		}
	}
	if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
	{
		for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
			{
				sendPlayerDeathEvent(gameStateArray[gameStateNumber].spectatorArray[i], player, gameStateNumber);
			}
		}
	}
}
void powerUpsEventHandler(int clientID, int player, int xPowerUp, int yPowerUp, std::string sendTime, int powerUp, int gameStateNumber)
{
	new_event("powerUpsEventHandler fired for ClientID: " + std::to_string(clientID));
	if (gameStateArray[gameStateNumber].boardPowerups[yPowerUp][xPowerUp] != 0)
	{

		gameStateArray[gameStateNumber].boardPowerups[yPowerUp][xPowerUp] = 0;
		gameStateArray[gameStateNumber].timeRecord[yPowerUp][xPowerUp] = sendTime;
		gameStateArray[gameStateNumber].playerGetPowerUp[yPowerUp][xPowerUp] = player;

		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				sendPowerUpsEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, player, xPowerUp, yPowerUp, sendTime, powerUp, "+");
			}
		}
		if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
		{
			for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
			{
				if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
				{
					sendPowerUpsEvent(gameStateArray[gameStateNumber].spectatorArray[i], player, xPowerUp, yPowerUp, sendTime, powerUp, "+");
				}
			}
		}
	}
	else
	{
		/*if (std::stoi(sendTime) < std::stoi(gameStateArray[gameStateNumber].timeRecord[yPowerUp][xPowerUp]))
		{
			int wrongPowerUpGiven = gameStateArray[gameStateNumber].playerGetPowerUp[yPowerUp][xPowerUp];
			gameStateArray[gameStateNumber].timeRecord[yPowerUp][xPowerUp] = sendTime;
			gameStateArray[gameStateNumber].playerGetPowerUp[yPowerUp][xPowerUp] = gameStateArray[gameStateNumber].PlayerNumber(clientID) - 1;

			for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
			{
				if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
				{
					sendPowerUpsEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, player, xPowerUp, yPowerUp, sendTime, powerUp, "+");
					sendPowerUpsEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, wrongPowerUpGiven, xPowerUp, yPowerUp, sendTime, powerUp, "-");

				}
			}
			if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
			{
				for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
				{
					if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
					{
						sendPowerUpsEvent(gameStateArray[gameStateNumber].spectatorArray[i], player, xPowerUp, yPowerUp, sendTime, powerUp, "+");
						sendPowerUpsEvent(gameStateArray[gameStateNumber].spectatorArray[i], wrongPowerUpGiven, xPowerUp, yPowerUp, sendTime, powerUp, "-");
					}
				}
			}
		}*/
		//log("Compare");
	}
}

void setPlayerGameEventHandler(int clientID, std::string id, int gameStateNumber, bool isSpectating)
{
	log("setPlayerGameEventHandler fired.");
	if (isSpectating == true) // Inserts clientID into spectatorArray
	{
		if (gameStateArray[gameStateNumber].spectatorArray.size() < 5)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (gameStateArray[gameStateNumber].spectatorArray[i] == -1)
				{
					gameStateArray[gameStateNumber].spectatorArray[i] = clientID;
					log("Spectator " + id + " has connected.");
					break;
				}
			}
		}
		else
		{
			log("Rejecting SpectatorJoin..." + gameStateArray[gameStateNumber].lobbyName + " lobby full.");
			JSON msg_obj = JSON::object{ { "event", "rejectSpectatorFullEvent" }, };
			lat_msg.push(info(clientID, msg_obj.dump()));
		}
	}
	else if (!gameStateArray[gameStateNumber].getPlayer1Online())
	{
		gameStateArray[gameStateNumber].playerDeaths[0] = 0;
		log("Player 1 (" + id + ") has connected to lobby " + gameStateArray[gameStateNumber].lobbyName);
		std::string previousID = gameStateArray[gameStateNumber].getPlayerID(1);
		gameStateArray[gameStateNumber].setPlayerID(1, id);
		gameStateArray[gameStateNumber].playerArray[0].ClientID = clientID;
		gameStateArray[gameStateNumber].playerArray[0].setName(id); //Set Player Name to Player Object
		gameStateArray[gameStateNumber].playerArray[0].setPlayerNumber(0); //Set Player Number
		new_event("New player ID set for " + previousID + ": " + id);
		sendUpdatePlayerNumberEvent(clientID, 1);//Example: client 0 gets playernumber 1 or client 3 gets player number 1 if were first to join game session
	}
	else if (!gameStateArray[gameStateNumber].getPlayer2Online())
	{
		gameStateArray[gameStateNumber].playerDeaths[1] = 0;
		log("Player 2 (" + id + ") has connected to lobby " + gameStateArray[gameStateNumber].lobbyName);
		std::string previousID = gameStateArray[gameStateNumber].getPlayerID(2);
		gameStateArray[gameStateNumber].setPlayerID(2, id);
		gameStateArray[gameStateNumber].playerArray[1].ClientID = clientID;
		gameStateArray[gameStateNumber].playerArray[1].setName(id); //Set Player Name to Player Object
		gameStateArray[gameStateNumber].playerArray[1].setPlayerNumber(1); //Set Player Number
		new_event("New player ID set for " + previousID + ": " + id);
		sendUpdatePlayerNumberEvent(clientID, 2);
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[1].ClientID, PLAYER_1, gameStateArray[gameStateNumber].getPlayerID(PLAYER_1)); // Tell (Player 2) the information for Player 1
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[0].ClientID, PLAYER_2, gameStateArray[gameStateNumber].getPlayerID(PLAYER_2)); // Tell (Player 1) the information for Player 2
	}
	else if (!gameStateArray[gameStateNumber].getPlayer3Online())
	{
		gameStateArray[gameStateNumber].playerDeaths[2] = 0;
		log("Player 3 (" + id + ") has connected to lobby " + gameStateArray[gameStateNumber].lobbyName);
		std::string previousID = gameStateArray[gameStateNumber].getPlayerID(3);
		gameStateArray[gameStateNumber].setPlayerID(3, id);
		gameStateArray[gameStateNumber].playerArray[2].ClientID = clientID;
		gameStateArray[gameStateNumber].playerArray[2].setName(id); //Set Player Name to Player Object
		gameStateArray[gameStateNumber].playerArray[2].setPlayerNumber(2); //Set Player Number
		new_event("New player ID set for " + previousID + ": " + id);
		sendUpdatePlayerNumberEvent(clientID, 3);
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[2].ClientID, PLAYER_1, gameStateArray[gameStateNumber].getPlayerID(PLAYER_1)); // Tell Client ID 2 (Player 3) the information for Player 1
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[2].ClientID, PLAYER_2, gameStateArray[gameStateNumber].getPlayerID(PLAYER_2)); // Tell Client ID 2 (Player 3) the information for Player 2
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[0].ClientID, PLAYER_3, gameStateArray[gameStateNumber].getPlayerID(PLAYER_3)); // Tell Client ID 0 (Player 1) the information for Player 3
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[1].ClientID, PLAYER_3, gameStateArray[gameStateNumber].getPlayerID(PLAYER_3)); // Tell Client ID 1 (Player 2) the information for Player 3
	}
	else if (!gameStateArray[gameStateNumber].getPlayer4Online())
	{
		gameStateArray[gameStateNumber].playerDeaths[3] = 0;
		log("Player 4 (" + id + ") has connected to lobby " + gameStateArray[gameStateNumber].lobbyName);
		std::string previousID = gameStateArray[gameStateNumber].getPlayerID(4);
		gameStateArray[gameStateNumber].setPlayerID(4, id);
		gameStateArray[gameStateNumber].playerArray[3].ClientID = clientID;
		gameStateArray[gameStateNumber].playerArray[3].setName(id); //Set Player Name to Player Object
		gameStateArray[gameStateNumber].playerArray[3].setPlayerNumber(3); //Set Player Number
		new_event("New player ID set for " + previousID + ": " + id);
		sendUpdatePlayerNumberEvent(clientID, 4);
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[3].ClientID, PLAYER_1, gameStateArray[gameStateNumber].getPlayerID(PLAYER_1)); // Tell Client ID 3 (Player 4) the information for Player 1
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[3].ClientID, PLAYER_2, gameStateArray[gameStateNumber].getPlayerID(PLAYER_2)); // Tell Client ID 3 (Player 4) the information for Player 2
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[3].ClientID, PLAYER_3, gameStateArray[gameStateNumber].getPlayerID(PLAYER_3)); // Tell Client ID 3 (Player 4) the information for Player 3
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[0].ClientID, PLAYER_4, gameStateArray[gameStateNumber].getPlayerID(PLAYER_4)); // Tell Client ID 0 (Player 1) the information for Player 4
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[1].ClientID, PLAYER_4, gameStateArray[gameStateNumber].getPlayerID(PLAYER_4)); // Tell Client ID 1 (Player 2) the information for Player 4
		sendPlayerConnectedEvent(gameStateArray[gameStateNumber].playerArray[2].ClientID, PLAYER_4, gameStateArray[gameStateNumber].getPlayerID(PLAYER_4)); // Tell Client ID 2 (Player 3) the information for Player 4
	}
	else
	{
		log("Rejecting lobbyJoin..." + gameStateArray[gameStateNumber].lobbyName + " lobby full.");
		JSON msg_obj = JSON::object{ { "event", "rejectLobbyFullEvent" }, };
		lat_msg.push(info(clientID, msg_obj.dump()));
	}
}
void setPlayerIDEventHandler(int clientID, int size, std::string id, std::string pw)
{
	new_event("setPlayerIDEventHandler fired.");
	int match = matchIdPw(id, pw);
	if (match == 0)
	{
		// valid id but invalid password
		log("Wrong password");
		sendWrongPasswordEvent(clientID);
		return;
		// dc client 
		/*New Note: Clients DC locally for efficient Disconnect
		Clients DC when they recieve wrong pass. Before, they dc
		before they could recieve a wrong pass message*/
		//server.wsClose(clientID);
	}
	else if (match == -1)
	{
		log("Create New Account?");
		sendCreateNewAccountEvent(clientID);
	}
	else if (match == 1)
	{
		log("Login Successful");
		for (int i = 0; i < server.getClientIDs().size(); i++)
		{
			if (i != clientID)
			{
				sendNetworkConnectedEvent(i, id);
			}
		}
		JSON msg_obj = JSON::object{ { "event", "clientNetworkConnectedEvent" },
		{ "id", id }, };
		lat_msg.push(info(clientID, msg_obj.dump()));

		new_event("clientNetworkConnectedEvent sent for client ID " + std::to_string(clientID) + " (client" + id + " connected.)");

	}
}

void setNewPlayerIDEventHandler(int clientID, int size, std::string id, std::string pw, int gameStateNumber)
{
	//new player, create new account
	infile.open("database.txt");

	std::string tempStrFile((std::istreambuf_iterator<char>(infile)),
		std::istreambuf_iterator<char>());

	outfile.open("database.txt");
	outfile << tempStrFile;

	//iss >> name >> password >> wins >> losses >> ties >> deaths >> kills >> bombs)
	outfile << id << "\t" << pw << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 << "\t" << 0 << std::endl;
	infile.close();
	outfile.close();
	log("New player");
	new_event("setNewPlayerIDEventHandler fired.");
	for (int i = 0; i < server.getClientIDs().size(); i++)
	{
		if (i != clientID)
		{
			sendNetworkConnectedEvent(i, id);
		}
	}
	JSON msg_obj = JSON::object{ { "event", "clientNetworkConnectedEvent" },
	{ "id", id }, };
	lat_msg.push(info(clientID, msg_obj.dump()));

	new_event("clientNetworkConnectedEvent sent for client ID " + std::to_string(clientID) + " (client" + id + " connected.)");

}

void gameStartEventHandler(int gameStateNumber)
{
	gameStateArray[gameStateNumber].resetInit();
	gameStateArray[gameStateNumber].LobbyGameStarted = true;
	new_event("gameStartEventHandler fired.");
	for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
	{
		if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
		{
			sendGameStartedEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID);
		}
	}
	if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
	{
		for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
			{
				sendGameStartedEvent(gameStateArray[gameStateNumber].spectatorArray[i]);
			}
		}
	}
}

void menuScreenRestartEventHandler(int gameStateNumber)
{
	if (!gameStateArray[gameStateNumber].isActive)
	{
		gameStateArray[gameStateNumber].resetInit();
		gameStateArray[gameStateNumber].isActive = true;
		gameStateArray[gameStateNumber].round++;
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				clientsInGame.insert(gameStateArray[gameStateNumber].playerArray[i].ClientID);
			}
		}
		updatePositionCommunicationThrottle();
		gameStateArray[gameStateNumber].setGameRunning(true);
		new_event("menuScreenRestartEventHandler fired.");
		log("Power Ups Board:");
		gameStateArray[gameStateNumber].logPowerUpsBoard();
		/*for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
		if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
		{
		sendGameBoardPowerUpsEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, gameStateNumber);
		}
		}*/
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				sendMenuScreenRestartedEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, gameStateNumber);
			}
		}
		if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
		{
			for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
			{
				if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
				{
					sendMenuScreenRestartedEvent(gameStateArray[gameStateNumber].spectatorArray[i], gameStateNumber);
				}
			}
		}
		gameStateArray[gameStateNumber].playersInGameOnStart = gameStateArray[gameStateNumber].PlayersInGame();
	}
}

void menuScreenStartEventHandler(int gameStateNumber)
{
	if (!gameStateArray[gameStateNumber].isActive)
	{
		gameStateArray[gameStateNumber].isActive = true;
		gameStateArray[gameStateNumber].round++;
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				clientsInGame.insert(gameStateArray[gameStateNumber].playerArray[i].ClientID);
			}
		}
		updatePositionCommunicationThrottle();
		gameStateArray[gameStateNumber].setGameRunning(true);
		new_event("menuScreenStartEventHandler fired.");
		log("Power Ups Board:");
		gameStateArray[gameStateNumber].logPowerUpsBoard();
		/*for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				sendGameBoardPowerUpsEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, gameStateNumber);
			}
		}*/
		for (int i = 0; i < gameStateArray[gameStateNumber].playerArray.size(); ++i)
		{
			if (gameStateArray[gameStateNumber].playerArray[i].ClientID != -1)
			{
				sendMenuScreenStartedEvent(gameStateArray[gameStateNumber].playerArray[i].ClientID, gameStateNumber);
			}
		}
		if (gameStateArray[gameStateNumber].spectatorArray.size() != 0)//Send to all Spectators, if any
		{
			for (int i = 0; i < gameStateArray[gameStateNumber].spectatorArray.size(); ++i)
			{
				if (gameStateArray[gameStateNumber].spectatorArray[i] != -1)
				{
					sendMenuScreenStartedEvent(gameStateArray[gameStateNumber].spectatorArray[i], gameStateNumber);
				}
			}
		}
		gameStateArray[gameStateNumber].playersInGameOnStart = gameStateArray[gameStateNumber].PlayersInGame();
	}
}

void NTPEventHandler(int clientID, int latencyID, int xPos, int yPos, long long ServerRecievedTime, int gameStateNumber)
{
	if (xPos != 1000)
	{
		try
		{
			if (gameStateNumber != -1)
			{
				gameStateArray[gameStateNumber].playerArray[gameStateArray[gameStateNumber].PlayerNumber(clientID) - 1].x = xPos;
				gameStateArray[gameStateNumber].playerArray[gameStateArray[gameStateNumber].PlayerNumber(clientID) - 1].y = yPos;
			}
		}
		catch (std::string&e) { log("NTP error on unknown client"); }
	}

	long long X = ServerRecievedTime;

	//long long Y = currentTime();

	sendLatencyEstimationEvent(clientID, latencyID, X, gameStateNumber);
}

void playerChatHandler(int clientID, std::string playerID, std::string msg)
{
	sendPlayerChatEvent(clientID, playerID, msg);
}

/* Begin Network Handlers */

void openHandler(int clientID)
{
	new_event("Connection opened. Client ID: " + std::to_string(clientID));
	std::vector<int> clientIDs = server.getClientIDs();

	new_event("Connection established.");
	lat_msg.push(info(clientID, "Connection established."));

}

void closeHandler(int clientID)
{
	new_event("Connection closed. Client ID: " + std::to_string(clientID));
	removeInGameClient(clientID);
	std::vector<int> clientIDs = server.getClientIDs();
	lat_msg.push(info(clientID, "Connection closed."));
	int clientGameStateNumber = getClientGameNumber(clientID);
	int pnum = -1;

	if (clientGameStateNumber != -1)
	{
		for (int j = 0; j < gameStateArray[clientGameStateNumber].playerArray.size(); j++)
		{
			if (gameStateArray[clientGameStateNumber].playerArray[j].ClientID == clientID)
			{
				pnum = gameStateArray[clientGameStateNumber].PlayerNumber(clientID);
				gameStateArray[clientGameStateNumber].setPlayerOnline(pnum, false);
				gameStateArray[clientGameStateNumber].playerArray[j].ClientID = -1;
				gameStateArray[clientGameStateNumber].resetID(pnum);
				int playersInGameAfterDC = gameStateArray[clientGameStateNumber].PlayersInGame();
				if (playersInGameAfterDC == 0)
				{
					gameStateArray[clientGameStateNumber].lobbyName = "<None>";	//Reset Lobby Name if empty
					gameStateArray[clientGameStateNumber].resetGameState();
					gameStateArray[clientGameStateNumber].LobbyGameStarted = false;
					gameStateArray[clientGameStateNumber].isActive = false;
					for (int p = 0; p<4; p++)
					{
						gameStateArray[clientGameStateNumber].playerDeaths[p] = -1;
						gameStateArray[clientGameStateNumber].setPlayerOnline(p, false);
					}
				}
				else if (playersInGameAfterDC == 1)
				{
					int pnum2 = -1;
					for (int k = 0; k < gameStateArray[clientGameStateNumber].playerArray.size(); k++)
					{
						if (gameStateArray[clientGameStateNumber].playerArray[k].ClientID != -1)
						{
							pnum2 = gameStateArray[clientGameStateNumber].PlayerNumber(gameStateArray[clientGameStateNumber].playerArray[k].ClientID);
							sendPlayerDeathEvent(gameStateArray[clientGameStateNumber].playerArray[k].ClientID, pnum, clientGameStateNumber);
							sendPlayerDisconnectEvent(gameStateArray[clientGameStateNumber].playerArray[k].ClientID, pnum);

							gameStateArray[clientGameStateNumber].setPlayerOnline(pnum2, false);
							gameStateArray[clientGameStateNumber].playerArray[k].ClientID = -1;
							gameStateArray[clientGameStateNumber].resetID(pnum2);

							gameStateArray[clientGameStateNumber].lobbyName = "<None>";	//Reset Lobby Name if empty
							gameStateArray[clientGameStateNumber].resetGameState();
							gameStateArray[clientGameStateNumber].LobbyGameStarted = false;
							gameStateArray[clientGameStateNumber].isActive = false;
							for (int p = 0; p<4; p++)
							{
								gameStateArray[clientGameStateNumber].playerDeaths[p] = -1;
								gameStateArray[clientGameStateNumber].setPlayerOnline(p, false);
							}
						}
						gameStateArray[clientGameStateNumber].playerDeaths[k] = -1;
					}
				}
				else
				{
					if (gameStateArray[clientGameStateNumber].isActive)
					{
						for (int i = 0; i < gameStateArray[clientGameStateNumber].playerArray.size(); i++)
						{
							if (gameStateArray[clientGameStateNumber].playerArray[i].ClientID != -1)
							{
								sendPlayerDeathEvent(gameStateArray[clientGameStateNumber].playerArray[i].ClientID, pnum, clientGameStateNumber);
				
								log("PlayerLeftGame");
								JSON msg_obj = JSON::object{ { "event", "playerGameLeaveEvent" },
								{ "playerNumber", std::to_string(pnum) }, };
								lat_msg.push(info(gameStateArray[clientGameStateNumber].playerArray[i].ClientID, msg_obj.dump()));
							}
						}
						gameStateArray[clientGameStateNumber].playerDeaths[pnum - 1] = -1;
						gameStateArray[clientGameStateNumber].playersInGameOnStart--;
					}
					else if (!gameStateArray[clientGameStateNumber].isActive)
					{
						for (int i = 0; i < gameStateArray[clientGameStateNumber].playerArray.size(); i++)
						{
							if (gameStateArray[clientGameStateNumber].playerArray[i].ClientID != -1)
							{
								log("PlayerLeftLobby");
								JSON msg_obj = JSON::object{ { "event", "playerLobbyLeaveEvent" },
								{ "playerNumber", std::to_string(pnum) }, };
								lat_msg.push(info(gameStateArray[clientGameStateNumber].playerArray[i].ClientID, msg_obj.dump()));
							}
						}
						gameStateArray[clientGameStateNumber].lobbyName = "<None>";	//Reset Lobby Name if empty
						gameStateArray[clientGameStateNumber].resetGameState();
						gameStateArray[clientGameStateNumber].LobbyGameStarted = false;
						gameStateArray[clientGameStateNumber].isActive = false;
						for (int p = 0; p<4; p++)
						{
							gameStateArray[clientGameStateNumber].playerDeaths[p] = -1;
							gameStateArray[clientGameStateNumber].setPlayerOnline(p, false);
						}
					}
				}
			}
		}
	}
	clientsInGame.erase(clientID);
	server.wsClose(clientID);
}

// When the server receives message from clients, push it to lat_event to be processed later after some delay.
void messageHandler(int clientID, std::string message)
{
	lat_event.push(info(clientID, message));
}

void processEvent(info &i)
{
	std::string err; // This string is updated with an error message if the json parser fails.
	auto json = JSON::parse(i.msg, err);
	std::string firedEvent = json["event"].string_value();

	std::vector<int> clientIDs = server.getClientIDs();
	int clientID = i.clientID;
	int clientGame = getClientGameNumber(clientID);

	// Begin Event Handling
	if (firedEvent == "setPlayerIDEvent") // JSON example -> {"event": "setPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		setPlayerIDEventHandler(clientID, clientIDs.size(), json["id"].string_value(), json["pw"].string_value());
	}
	else if (firedEvent == "confirmationEvent") // JSON example: JSON example -> {"event": "newPlayerIDEvent", "player": 1, "id": "TTaiN"}
	{
		setNewPlayerIDEventHandler(clientID, clientIDs.size(), json["id"].string_value(), json["pw"].string_value(), clientGame);
	}
	else if (firedEvent == "rejectionEvent")
	{
		server.wsClose(clientID);
	}
	else if (firedEvent == "playerDeathEvent")
	{
		playerDeathHandler(json["player"].int_value(), clientGame);
	}
	else if (firedEvent == "gameStartEvent") // JSON example -> {"event": "gameStartedEvent"}
	{
		gameStartEventHandler(clientGame);
	}
	else if (firedEvent == "destroyAllBlocksCustomEvent")
	{
		new_event("destroyAllBlocksCustomEventHandler fired.");
		if (clientGame != -1)
		{
			for (int i = 0; i < 4; ++i)
			{
				if (gameStateArray[clientGame].playerArray[i].ClientID != -1)
				{
					sendDestroyAllBlocksCustomEvent(gameStateArray[clientGame].playerArray[i].ClientID, clientGame);
				}
			}
		}
	}
	else if (firedEvent == "createJoinEvent")
	{
		int gameStateNumber = -1;	//Lobby Number set to -1 if no lobbies are available
		int FilledLobbies;
		FilledLobbies = 0;			//Number of filled lobbies to check if all lobbies are named and taken
		bool foundLobby = false;	//Set true if typed lobby name also shares same name as lobby found in lobby gameStateArray
		int lastFreeLobby = -1;		//Used to create and rename a lobby using the last available lobby number
		std::string lobbyName = "default";
		if (json["lobbyName"].string_value() != "" && json["lobbyName"].string_value() != "<None>")
		{
			lobbyName = json["lobbyName"].string_value();
		}
		for (int i = 0; i<gameStateArray.size(); i++) //Go through all lobbies in gameStateArray to find matching lobby name
		{
			if (!foundLobby && gameStateArray[i].lobbyName == lobbyName)
			{
				gameStateNumber = i;
				foundLobby = true;
			}
			if (gameStateArray[i].PlayersInGame() > 0)
			{
				FilledLobbies++;	//Counts everytime a lobby has been created with at least 1 player.
			}
			else
			{
				lastFreeLobby = i;
			}
		}
		if (foundLobby) //Used if a lobby under the same name is requested is found/joining lobby
		{
			if (!gameStateArray[gameStateNumber].isActive && !gameStateArray[gameStateNumber].LobbyGameStarted) //Cannot Join mid game, must wait until current game ends
			{
				bool alreadyIn = false;
				for (int l = 0; l < 4; l++)//search to make sure player isnt in lobby already or in a different lobby
				{
					for (int p = 0; p < 4; p++)
					{
						if (gameStateArray[l].playerArray[p].name == json["id"].string_value())
						{
							alreadyIn = true;
							break;
						}
					}
				}
				if (!alreadyIn)
				{
					setPlayerGameEventHandler(clientID, json["id"].string_value(), gameStateNumber, false);//Handles if lobby is full
				}
			}
			else
			{
				new_event("Rejecting lobbyJoin...lobby InGame");
				JSON msg_obj = JSON::object{ { "event", "rejectLobbyJoinEvent" }, };
				lat_msg.push(info(clientID, msg_obj.dump()));
			}
		}
		else //create a new lobby using requested name
		{
			if (FilledLobbies == gameStateArray.size()) //do not create a new lobby if all lobby spaces are taken
			{
				new_event("Rejecting lobbyJoin...noavailableLobbies");
				JSON msg_obj = JSON::object{ { "event", "rejectLobbyCreateEvent" }, };
				lat_msg.push(info(clientID, msg_obj.dump()));
			}
			else //Create new lobby with name 
			{
				bool alreadyIn = false;
				for (int l = 0; l < 4; l++)//search to make sure player isnt in lobby already or in a different lobby
				{
					for (int p = 0; p < 4; p++)
					{
						if (gameStateArray[l].playerArray[p].name == json["id"].string_value())
						{
							alreadyIn = true;
							break;
						}
					}
				}
				if (!alreadyIn)
				{
					gameStateArray[lastFreeLobby].lobbyName = lobbyName;
					setPlayerGameEventHandler(clientID, json["id"].string_value(), lastFreeLobby, false);//Handles if lobby is full
				}
				else
				{
					new_event("Rejecting lobbyJoin...Already In Different Lobby");
					JSON msg_obj = JSON::object{ { "event", "rejectMultipleLobbyCreateEvent" }, };
					lat_msg.push(info(clientID, msg_obj.dump()));
				}
			}
		}
	}
	else if (firedEvent == "joinSpectateEvent")
	{
		int gameStateNumber = -1;	//Lobby Number set to -1 if no lobbies are available
		int FilledLobbies;
		FilledLobbies = 0;			//Number of filled lobbies to check if all lobbies are named and taken
		bool foundLobby = false;	//Set true if typed lobby name also shares same name as lobby found in lobby gameStateArray
		int lastFreeLobby = -1;		//Used to create and rename a lobby using the last available lobby number
		std::string lobbyName = "default";
		if (json["lobbyName"].string_value() != "" && json["lobbyName"].string_value() != "<None>")
		{
			lobbyName = json["lobbyName"].string_value();
		}
		for (int i = 0; i<gameStateArray.size(); i++) //Go through all lobbies in gameStateArray to find matching lobby name
		{
			if (!foundLobby && gameStateArray[i].lobbyName == lobbyName)
			{
				gameStateNumber = i;
				foundLobby = true;
			}
			if (gameStateArray[i].PlayersInGame() > 0)
			{
				FilledLobbies++;	//Counts everytime a lobby has been created with at least 1 player.
			}
			else
			{
				lastFreeLobby = i;
			}
		}
		if (foundLobby) //Used if a lobby under the same name is requested is found/joining lobby
		{
			setPlayerGameEventHandler(clientID, json["id"].string_value(), gameStateNumber, true);

			log("Spectator " + json["id"].string_value() + " is now watching Lobby " + json["lobbyName"].string_value());

			JSON msg_obj = JSON::object{ { "event", "acceptSpecJoinEvent" },
			{ "id", json["id"].string_value() }, };
			lat_msg.push(info(clientID, msg_obj.dump()));
		}
		else
		{
			log("No Lobby of name " + json["lobbyName"].string_value() + " was found...");
			JSON msg_obj = JSON::object{ { "event", "rejectSpecJoinEvent" },
			{ "lobby", json["lobbyName"].string_value() }, };
			lat_msg.push(info(clientID, msg_obj.dump()));
		}
	}
	else if (firedEvent == "menuScreenStart")
	{
		menuScreenStartEventHandler(clientGame);
	}
	else if (firedEvent == "menuScreenRestart")
	{
		menuScreenRestartEventHandler(clientGame);
	}
	else if (firedEvent == "bombPlacementEvent")
	{
		bombPlacementEventHandler(clientID, json["player_number"].int_value(), json["xBomb"].int_value(), json["yBomb"].int_value(), json["sentTime"].string_value(), currentTime(), clientGame);
		new_event("Server Recieved Bomb. Client: " + std::to_string(clientID) + "; Player Number: " + std::to_string(gameStateArray[clientGame].PlayerNumber(clientID)));
	}
	else if (firedEvent == "powerUpsEvent")
	{
		powerUpsEventHandler(clientID, json["player_number"].int_value(), json["xPowerUp"].int_value(), json["yPowerUp"].int_value(), json["sentTime"].string_value(), json["powerUp"].int_value(), clientGame);
	}
	else if (firedEvent == "latencyEstimationEvent")
	{
		NTPEventHandler(clientID, json["id"].int_value(), json["xPosition"].int_value(), json["yPosition"].int_value(), currentTime(), clientGame);
	}
	else if (firedEvent == "playerChatEvent")
	{
		playerChatHandler(clientID, json["playerID"].string_value(), json["msg"].string_value());
	}
	else if (firedEvent == "addVirtualClientEvent")
	{
		addInGameClient(json["client"].int_value());
	}
	else if (firedEvent == "removeVirtualClientEvent")
	{
		removeInGameClient(json["client"].int_value());
	}
	else if (firedEvent == "statisticsEvent")
	{
		sendStatistics(clientID);
	}
	else if (firedEvent == "refreshLobbyListEvent")
	{
		sendLobbyList(clientID);
		log("==============Lobbies=============");
		for (int i = 0; i<4; i++)
		{
			log(gameStateArray[i].lobbyName);//Prints all current lobbys
			for (int j = 0; j<4; j++)
			{
				log(gameStateArray[i].playerArray[j].name);
			}
		}
		log("==================================");
	}
	else
	{
		log("Error (if any): " + err);
		lat_msg.push(info(clientID, "Received unexpected message: " + i.msg));
	}

}

// Check if any event should now be processed.
void checkEvent()
{
	std::vector<int> clientIDs = server.getClientIDs();

	while (lat_event.size() != 0)
	{
		info i = lat_event.top();
		if (i.timeToProcess > currentTime())  break;
		if (contains(clientIDs, i.clientID))
		{
			processEvent(i);
		}
		lat_event.pop();
	}
}

// Check if there is any message to be sent to clients.
void checkMessage()
{
	std::vector<int> clientIDs = server.getClientIDs();

	while (lat_msg.size() != 0)
	{
		info i = lat_msg.top();
		if (i.timeToProcess > currentTime())  break;
		if (i.isStats == true)
		{
			vector<int> clientIDs = server.getClientIDs();
			for (int j = 0; j < clientIDs.size(); j++)
			{
				if (j == i.clientID)
				{
					server.wsSend(j, i.msg);
				}
			}
		}
		else if (i.isChat == true)
		{
			vector<int> clientIDs = server.getClientIDs();
			for (int j = 0; j < clientIDs.size(); j++)
			{
				if (j != i.clientID)
				{
					server.wsSend(j, i.msg);
				}
			}
		}
		else if (contains(clientIDs, i.clientID))
		{
			server.wsSend(i.clientID, i.msg);
		}
		lat_msg.pop();
	}
}

/* Called 100 times every 1 second */
void periodicHandler()
{
	checkEvent();         // Check if any event ready to be processed, if so, process.
	checkMessage();       // Check if any message to client is ready to be sent, if so, sent. 
}

/* Begin Main Function */
int main()
{
	log("Current Accounts:\n" + strFile);
	outfile << strFile;
	outfile.close();
	infile.close();
	int port;

	std::cout << "[INIT] Please enter the desired port: ";
	std::cin >> port;

	srand(time(NULL));

	server.setOpenHandler(openHandler);
	server.setCloseHandler(closeHandler);
	server.setMessageHandler(messageHandler);
	server.setPeriodicHandler(periodicHandler);

	server.startServer(port);


	return 0;
}