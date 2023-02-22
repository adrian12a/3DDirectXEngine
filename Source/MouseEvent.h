#pragma once

class MouseEvent
{
	int x;
	int y;
public:
	MouseEvent();
	MouseEvent(int x, int y);
	int getMouseX();
	int getMouseY();
};