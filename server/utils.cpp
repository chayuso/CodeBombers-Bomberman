#include "utils.h"
#include "gamestate.h"

Utils::Utils() 
{} // default constructor

int Utils::bitmapPosition(int pixel) 
{
	int bitmap = pixel / GameState().blockSize;
	return bitmap;
}

int Utils::pixel_position(int bitmap) 
{
	int pixel = bitmap * GameState().blockSize;
	return pixel;
} 