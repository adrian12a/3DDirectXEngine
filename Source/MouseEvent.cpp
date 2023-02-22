#include "MouseEvent.h"

MouseEvent::MouseEvent()
{
	x = 0;
	y = 0;
}

MouseEvent::MouseEvent(int x, int y)
{
	this->x = x;
	this->y = y;
}

int MouseEvent::getMouseX()
{
	return x;
}

int MouseEvent::getMouseY()
{
	return y;
}