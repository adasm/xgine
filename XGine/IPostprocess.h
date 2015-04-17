/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API IPostprocess
{
	virtual ~IPostprocess() { }
	virtual void process() = 0;
};