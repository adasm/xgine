#pragma once
#include "Headers.h"

struct Flare
{
	Texture *texture;
	float distance;
	Flare()
	{
		texture = NULL;
		distance = 0.0f;
	}
};

class LensFlare
{
public:
	LensFlare(string fname);
	void			render(ICamera *cam, Vec3 sunPos);
protected:
	Flare*			m_flares;
};