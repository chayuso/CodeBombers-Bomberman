#include "bomb.h"
#include "utils.h"
#include "gamestate.h"
#include "player.h"
#include <chrono>

Bomb::Bomb() 
{
	this->isNull = true;
}

Bomb::Bomb(int _x, int _y, int _radius) :
	x(_x),y(_y),
	radiusUp(_radius),radiusDown(_radius),
	radiusLeft(_radius), radiusRight(_radius)
{
	this->startTime = currentTime();
	this->state = 0;
	this->explodes = false;
	this->extinguised = false;
	this->isNull = false;
}

long long Bomb::currentTime()
{
	return std::chrono::system_clock::now().time_since_epoch() / std::chrono::milliseconds(1);
}

long long Bomb::getTime()
{
	return this->currentTime() - this->startTime;
}

void Bomb::destroyBlocks(GameState GS) 
{
	this->timeElapsed = this->getTime();
	int bombY = Utils().bitmapPosition(this->y);
	int bombX = Utils().bitmapPosition(this->x);
	
	// Destroy above blocks
	for (int i = 1; i<=this->radiusUp;i++) 
	{
		if (GS.gameBoard[bombY - 1 * i][bombX] == 1)
		{
			GS.gameBoard[bombY - 1 * i][bombX] = 0;
			this->radiusUp = i;
			break;
		}
		else if (GS.gameBoard[bombY - 1 * i][bombX] == 2)
		{
			this->radiusUp = i - 1;
			break;
		}
	}

	// Destroy left blocks
	for (int i = 1; i <= this->radiusLeft; i++)
	{
		if (GS.gameBoard[bombY][bombX - 1 * i] == 1)
		{
			GS.gameBoard[bombY][bombX - 1 * i] = 0;
			this->radiusLeft = i;
			break;
		}
		else if (GS.gameBoard[bombY][bombX - 1 * i] == 2)
		{
			this->radiusLeft = i - 1;
			break;
		}
	}

	// Destroy below blocks
	for (int i = 1; i <= this->radiusDown; i++)
	{
		if (GS.gameBoard[bombY + 1 * i][bombX] == 1)
		{
			GS.gameBoard[bombY + 1 * i][bombX] = 0;
			this->radiusDown = i;
			break;
		}
		else if (GS.gameBoard[bombY + 1 * i][bombX] == 2)
		{
			this->radiusDown = i - 1;
			break;
		}
	}

	// Destroy right blocks
	for (int i = 1; i <= this->radiusRight; i++)
	{
		if (GS.gameBoard[bombY][bombX + 1 * i] == 1)
		{
			GS.gameBoard[bombY][bombX + 1 * i] = 0;
			this->radiusRight = i;
			break;
		}
		else if (GS.gameBoard[bombY][bombX + 1 * i] == 2)
		{
			this->radiusRight = i - 1;
			break;
		}
	}
}

void Bomb::killPlayers(GameState GS) {}

void Bomb::updateBomb(GameState GS)
{
	this->timeElapsed = this->getTime();

	if (this->timeElapsed >= 2000 && this->timeElapsed < 2500) 
	{
		if (this->explodes == false) 
		{
			this->destroyBlocks(GS);
			this->explodes = true;
		}
		this->killPlayers(GS);
	}
	else if(this->timeElapsed >= 2500)
	{
		this->extinguised = true;
	}
}

void Bomb::updateState() 
{
	this->timeElapsed = this->getTime();
	if (this->timeElapsed <= 1000) 
	{
		this->state = 0;
	}
	else if (this->timeElapsed <= 1500)
	{
		this->state = 1;
	}
	else if (this->timeElapsed <= 2000)
	{
		this->state = 2;
	}
	else if(this->explodes)
	{
		this->state = 3;
	}
}
