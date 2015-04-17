#pragma once
#include "Headers.h"

class Core
{
public:
	void		init(HINSTANCE hInstance);
	void		close();

	void		proceedLoop();

	Game*		game;
	Dynamics*	dynamics;
protected:
};