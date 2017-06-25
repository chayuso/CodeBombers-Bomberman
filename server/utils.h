#ifndef UTILS
#define UTILS

#include "gamestate.h"

class Utils
{
public:
	Utils(); // default constructor
	
	int bitmapPosition(int pixel); // Converts from pixel coordinates to bitmap coordinates
	int pixel_position(int bitmap); //// Convert from bitmap coordinates to pixel coordinates
};

#endif