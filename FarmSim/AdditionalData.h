#pragma once
#include "Headers.h"

enum AdditionalDataType
{
	DT_VEHICLE,
	DT_TRAFFIC_ACTOR,
	DT_DEVICE,
	DT_TRAILER,
	DT_DYNAMIC,
	DT_STATIC,
	DT_UNDEFINED,
};

struct AdditionalData
{
	AdditionalDataType dataType;
	void* data;
	AdditionalData()
	{
		data = NULL;
		dataType = DT_UNDEFINED;
	}
	AdditionalData(AdditionalDataType dt, void* d)
	{
		data = d;
		dataType = dt;
	}
};