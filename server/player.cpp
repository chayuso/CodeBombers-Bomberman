#include "player.h"
#include "utils.h"
#include "gamestate.h"

Player::Player() 
{
	this->isNull = true;
}

Player::Player(int _x, int _y) :
	x(_x), y(_y)
{
	this->alive = true;
	this->invincible = false;
	
	this->velocity = 3;

	this->left = false;
	this->down = false;
	this->left = false;
	this->right = false;
	this->direction = "down";
	
	this->release_bomb = false;
	this->bomb_radius = 1;
	this->bomb_limit = 1;
	this->isNull = false;
}

void Player::setName(std::string _name) 
{
	this->name = _name;
}
void Player::setPlayerNumber(int _num)
{
	this->playerNumber = _num;
}
void Player::setClientID(int _ID)
{
	this->ClientID = _ID;
}

void Player::move(GameState GS)
{
	// Update position
	if (this->left)
		this->x -= this->velocity;
	else if (this->right)
		this->x += this->velocity;
	else if (this->up)
		this->y -= this->velocity;
	else if (this->down)
		this->y += this->velocity;

	// Collision detection (rectifies coordinates)
	int board_x_left = Utils().bitmapPosition(this->x);
	int board_x_right = Utils().bitmapPosition(this->x + this->sprite_width - 1);
	int board_y_up = Utils().bitmapPosition(this->y);
	int board_y_down = Utils().bitmapPosition(this->y + this->sprite_height);

	bool top_left_collision = GS.gameBoard[board_y_up][board_x_left] >= 1;
	bool top_right_collision = GS.gameBoard[board_y_up][board_x_right] >= 1;
	bool bottom_left_collision = GS.gameBoard[board_y_down][board_x_left] >= 1;
	bool bottom_right_collision = GS.gameBoard[board_y_down][board_x_right] >= 1;

	if (this->left)
	{
		if (top_left_collision || bottom_left_collision)
		{
			this->x += this->velocity;
			board_x_left = Utils().bitmapPosition(this->x);
		}
	}

	else if (this->right)
	{
		if (top_right_collision || bottom_right_collision)
		{
			this->x -= this->velocity;
			board_x_right = Utils().bitmapPosition(this->x + this->sprite_width - 1);
		}
	}

	else if (this->up)
	{
		if (top_left_collision || top_right_collision)
		{
			this->y += this->velocity;
			board_y_up = Utils().bitmapPosition(this->y);
		}
	}

	else if (this->down)
	{
		if (bottom_left_collision || bottom_right_collision)
		{
			this->y -= this->velocity;
			board_y_down = Utils().bitmapPosition(this->y + this->sprite_height - 1);
		}
	}

	// Slide on corners
	if (this->left) {
		if (top_left_collision) {
			if (GS.gameBoard[board_y_up + 1][board_x_left] == 0 && GS.gameBoard[board_y_up + 1][board_x_left - 1] == 0)
				this->y++;
		}

		else if (bottom_left_collision) {
			if (GS.gameBoard[board_y_down - 1][board_x_left] == 0 && GS.gameBoard[board_y_down - 1][board_x_left - 1] == 0)
				this->y--;
		}
	}

	else if (this->right) {
		if (top_right_collision) {
			if (GS.gameBoard[board_y_up + 1][board_x_right] == 0 && GS.gameBoard[board_y_up + 1][board_x_right + 1] == 0)
				this->y++;
		}

		else if (bottom_right_collision) {
			if (GS.gameBoard[board_y_down - 1][board_x_right] == 0 && GS.gameBoard[board_y_down - 1][board_x_right + 1] == 0)
				this->y--;
		}
	}

	else if (this->up) {
		if (top_left_collision) {
			if (GS.gameBoard[board_y_up][board_x_left + 1] == 0 && GS.gameBoard[board_y_up - 1][board_x_left + 1] == 0)
				this->x++;
		}

		else if (top_right_collision) {
			if (GS.gameBoard[board_y_up][board_x_right - 1] == 0 && GS.gameBoard[board_y_up - 1][board_x_right - 1] == 0)
				this->x--;
		}
	}

	else if (this->down) {
		if (bottom_left_collision) {
			if (GS.gameBoard[board_y_down][board_x_left + 1] == 0 && GS.gameBoard[board_y_down + 1][board_x_left + 1] == 0)
				this->x++;
		}

		else if (bottom_right_collision) {
			if (GS.gameBoard[board_y_down][board_x_right - 1] == 0 && GS.gameBoard[board_y_down + 1][board_x_right - 1] == 0)
				this->x--;
		}
	}

	// Update animation frames. In our case, updates sprite facing direction
	if (this->left)
		this->direction = "left";
	else if (this->right)
		this->direction = "right";
	else if (this->up)
		this->direction = "up";
	else if (this->down)
		this->direction = "down";
}

void Player::kill()
{
	if (!this->invincible)
		this->alive = false;
}

void Player::add_power_up(int power_up)
{
	switch (power_up)
	{
		case 1: // fire
			this->bomb_radius++;
			break;
		case 2: // skate
			this->velocity++;
			break;
		case 3: // geta
			if (this->velocity > 1)
				this->velocity--;
			break;
		case 4: // bomb
			this->bomb_limit++;
			break;
	}
}