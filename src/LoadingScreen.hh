#ifndef LOADINGSCREEN_HH
#define LOADINGSCREEN_HH

#include <iostream>

#include <raylib.h>
#include <raymath.h>

#include "Constants.hh"

class LoadingScreen
{
public:
	LoadingScreen();
	~LoadingScreen();

	void DrawLoadingScreen(const int width, const int height);

private:
	Texture2D _sprLoadingScreen;
};

#endif