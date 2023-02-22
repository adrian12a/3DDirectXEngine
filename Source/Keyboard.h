#pragma once
class Keyboard
{
public:
	Keyboard();
	bool keyIsPressed(const unsigned char keycode);
	void keyPressed(const unsigned char keycode);
	void keyReleased(const unsigned char keycode);
	bool keyStates[256];
};
