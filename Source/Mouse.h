#pragma once
#include <queue>
#include "MouseEvent.h"

class Mouse
{
public:
	int x, y;
	void mouseMoved(int x, int y);
	void rightPressed();
	void rightReleased();
	void leftPressed();
	void leftReleased();
	bool EventBufferIsEmpty();
	MouseEvent ReadEvent();
	std::queue<MouseEvent> eventBuffer;
	bool rightClicked = false;
	int leftClicked = 0;
};
