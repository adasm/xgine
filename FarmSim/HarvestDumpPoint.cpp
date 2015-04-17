#include "HarvestDumpPoint.h"


vector<CfgVariable*> *HarvestDumpPointParams::getVariableList()
{
	if(m_varList)
		return m_varList;
	m_varList = new vector<CfgVariable*>;

	m_varList->push_back(new CfgVariable(V_VEC3, &actionBoxMin, getVarName(actionBoxMin)));
	m_varList->push_back(new CfgVariable(V_VEC3, &actionBoxMax, getVarName(actionBoxMax)));

	m_varList->push_back(new CfgVariable(V_INT, &grassTypeNo, getVarName(grassTypeNo)));
	m_varList->push_back(new CfgVariable(V_INT, &grassTypeSelection, getVarName(grassTypeSelection)));
	m_varList->push_back(new CfgVariable(V_INT, &capacity, getVarName(capacity)));
	return m_varList;
}

HarvestDumpPoint::HarvestDumpPoint(Vec3 pos, Vec3 rotation, HarvestDumpPointParams* params)
{
	if(!params)
		return;

	m_params = params;
	Mat temp;
	D3DXMatrixRotationYawPitchRoll(&m_pose, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&temp, pos.x, pos.y, pos.z);
	m_pose = m_pose * temp;
	m_grassTypeSelection = GTS_ONLY_THIS_NAME;
	if(m_params->grassTypeSelection == 0)
		m_grassTypeSelection = GTS_ALLTYPES;
	if(m_params->grassTypeSelection == 1)
		m_grassTypeSelection = GTS_ONLY_SAME_TYPES;
	int newSize = 0;
	m_actionBox = new ActionBox(m_params->actionBoxMin, m_params->actionBoxMax);
	m_actionBox->setTransform(&m_pose);
	GrassManager* gm = core.game->getWorld()->getGrassManager();
	switch(m_grassTypeSelection)
	{
		case GTS_ALLTYPES:
			for(int i = 0; i < gm->getCropTypesList()->size(); i++)
			{
				m_payloadValue.insert(make_pair(gm->getCropTypeInfo(i), 0));
			}
			break;
		case GTS_ONLY_SAME_TYPES:
			for(int i = 0; i < gm->getCropTypesList()->size(); i++)
			{
				if((*gm->getCropTypesList())[i]->typeName == gm->getCropTypeInfo(m_params->grassTypeNo)->typeName)
					m_payloadValue.insert(make_pair(gm->getCropTypeInfo(i), 0));
			}
			break;
		case GTS_ONLY_THIS_NAME:
			for(int i = 0; i < gm->getCropTypesList()->size(); i++)
			{
				if((*gm->getCropTypesList())[i] == gm->getCropTypeInfo(m_params->grassTypeNo))
					m_payloadValue.insert(make_pair(gm->getCropTypeInfo(i), 0));
			}
			break;
	}
}

int HarvestDumpPoint::getCapacity()
{
	return m_params->capacity;
}

int HarvestDumpPoint::getActualPayload(CropType* cropType)
{/*
	if(GrassTypeNo == -1 || GrassTypeNo == m_params->grassTypeNo)
		return m_payloadValue[m_params->grassTypeNo];
	else
	{
		if(m_grassTypeSelection == GTS_ALLTYPES)
			return m_payloadValue[GrassTypeNo];
		if(m_grassTypeSelection == GTS_ONLY_SAME_TYPES)
		{
			if(core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_params->grassTypeNo)->typeName == core.game->getWorld()->getGrassManager()->getCropTypeInfo(GrassTypeNo)->typeName)
				return m_payloadValue[GrassTypeNo];
		}
	}*/
	if(m_payloadValue.find(cropType) != m_payloadValue.end())
		return m_payloadValue.find(cropType)->second;
	return -1;
}

GrassTypeSelection HarvestDumpPoint::getGrassTypeSelection()
{
	return m_grassTypeSelection;
}

void HarvestDumpPoint::addPayload(int payloadToAdd, CropType* cropType)
{
	/*m_payloadValue.find(GrassTypeNo)->second
	if(GrassTypeNo == -1 || GrassTypeNo == m_params->grassTypeNo)
		m_payloadValue[m_params->grassTypeNo] += payloadToAdd;
	else
	{
		if(m_grassTypeSelection == GTS_ALLTYPES)
			return m_payloadValue[GrassTypeNo] += payloadToAdd;
		if(m_grassTypeSelection == GTS_ONLY_SAME_TYPES)
		{
			if(core.game->getWorld()->getGrassManager()->getCropTypeInfo(m_params->grassTypeNo)->typeName == core.game->getWorld()->getGrassManager()->getCropTypeInfo(GrassTypeNo)->typeName)
				return m_payloadValue[GrassTypeNo];
		}
	}*/
	if(m_payloadValue.find(cropType) != m_payloadValue.end())
		m_payloadValue.find(cropType)->second += payloadToAdd;
}

ActionBox* HarvestDumpPoint::getActionBox()
{
	return m_actionBox;
}

Mat* HarvestDumpPoint::getPose()
{
	return &m_pose;
}

void HarvestDumpPoint::setPose(Mat* pose)
{
	m_pose = *pose;
	m_actionBox->setTransform(pose);
}