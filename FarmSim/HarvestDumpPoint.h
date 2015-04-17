#pragma once
#include "Headers.h"

enum GrassTypeSelection
{
	GTS_ALLTYPES = 0,
	GTS_ONLY_SAME_TYPES = 1,
	GTS_ONLY_THIS_NAME = 2
};

struct HarvestDumpPointParams
{
	Vec3 actionBoxMin;
	Vec3 actionBoxMax;
	int grassTypeNo;
	int grassTypeSelection;
	int capacity;

	HarvestDumpPointParams()
	{
		m_varList = NULL;
		actionBoxMin = Vec3(0, 0, 0);
		actionBoxMax = Vec3(0, 0, 0);
		grassTypeNo = 0;
		grassTypeSelection = 0;
		capacity = 0;
	}

	vector<CfgVariable*> *getVariableList();
protected:
	vector<CfgVariable*> *m_varList;
};

class HarvestDumpPoint
{
public:
	HarvestDumpPoint(Vec3 pos, Vec3 rotation, HarvestDumpPointParams* params);

	void									setPose(Mat* pose);
	int										getCapacity();
	int										getActualPayload(CropType* cropType);
	GrassTypeSelection						getGrassTypeSelection();

	void									addPayload(int payloadToAdd, CropType* cropType);
	ActionBox*								getActionBox();
	Mat*									getPose();
protected:
	ActionBox*								m_actionBox;
	Mat										m_pose;

	HarvestDumpPointParams*					m_params;

	GrassTypeSelection						m_grassTypeSelection;
	map<CropType*, int>						m_payloadValue;
	map<CropType*, int>::iterator			m_payloadValueIt;
};