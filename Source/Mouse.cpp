#include "Mouse.h"

void Mouse::mouseMoved(int x, int y)
{
	eventBuffer.push(MouseEvent(x, y));
	this->x = x;
	this->y = y;
}

bool Mouse::EventBufferIsEmpty()
{
	return eventBuffer.empty();
}

MouseEvent Mouse::ReadEvent()
{
	if (eventBuffer.empty())
	{
		return MouseEvent();
	}
	else
	{
		MouseEvent event = eventBuffer.front();
		eventBuffer.pop();
		return event;
	}
}

void Mouse::rightPressed()
{
	rightClicked = true;
}

void Mouse::rightReleased()
{
	rightClicked = false;
}

void Mouse::leftPressed()
{
	leftClicked = 1;
}

void Mouse::leftReleased()
{
	leftClicked = 0;
}