#include "Keyboard.h"

Keyboard::Keyboard()
{
	for (int i = 0; i < 256; i++)
		this->keyStates[i] = false;
}

void Keyboard::keyPressed(const unsigned char keycode)
{
	keyStates[keycode] = true;
}

void Keyboard::keyReleased(const unsigned char keycode)
{
	keyStates[keycode] = false;
}

bool Keyboard::keyIsPressed(const unsigned char keycode)
{
	return this->keyStates[keycode];
}