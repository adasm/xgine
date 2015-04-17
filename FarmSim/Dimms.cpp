#include "Dimms.h"

vector<CfgVariable*> *VehicleParams::getVariableList()
{
	if(m_varList)
		return m_varList;
	m_varList = new vector<CfgVariable*>;

	m_varList->push_back(new CfgVariable(V_FLOAT, &chassisDensity, getVarName(chassisDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelDensity, getVarName(frontWheelDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelDensity, getVarName(rearWheelDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &maxSteerAngle, getVarName(maxSteerAngle)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsRadius, getVarName(frontWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsWidth, getVarName(frontWheelsWidth)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsRadius, getVarName(rearWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsWidth, getVarName(rearWheelsWidth)));
	m_varList->push_back(new CfgVariable(V_INT, &wheelDriveType, getVarName(wheelDriveType)));

	m_varList->push_back(new CfgVariable(V_VEC3, &posFrontLeftWheel, getVarName(posFrontLeftWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posFrontRightWheel, getVarName(posFrontRightWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posRearLeftWheel, getVarName(posRearLeftWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posRearRightWheel, getVarName(posRearRightWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecFrontLeftWheelRotation, getVarName(vecFrontLeftWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecFrontRightWheelRotation, getVarName(vecFrontRightWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecRearLeftWheelRotation, getVarName(vecRearLeftWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecRearRightWheelRotation, getVarName(vecRearRightWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecChassisForward, getVarName(vecChassisForward)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posTriPodFront, getVarName(posTriPodFront)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posTriPodBack, getVarName(posTriPodBack)));


	return m_varList;
}

vector<CfgVariable*> *TrailerParams::getVariableList()
{

	if(m_varList)
		return m_varList;

	m_varList = new vector<CfgVariable*>;
	m_varList->push_back(new CfgVariable(V_FLOAT, &maxHarvestHeight, getVarName(maxHarvestHeight)));
	m_varList->push_back(new CfgVariable(V_INT, &capacity, getVarName(capacity)));

	m_varList->push_back(new CfgVariable(V_VEC3, &posDumperHinge, getVarName(posDumperHinge)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posHarvest, getVarName(posHarvest)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posDumper, getVarName(posDumper)));

	m_varList->push_back(new CfgVariable(V_VEC3, &dumperActionBoxMin, getVarName(dumperActionBoxMin)));
	m_varList->push_back(new CfgVariable(V_VEC3, &dumperActionBoxMax, getVarName(dumperActionBoxMax)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posPayloadDump, getVarName(posPayloadDump)));

	return m_varList;
}

vector<CfgVariable*> *CultivatorParams::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;
	/*varList->push_back(new CfgVariable(V_FLOAT, &chassisDensity, getVarName(chassisDensity)));
	varList->push_back(new CfgVariable(V_FLOAT, &frontWheelDensity, getVarName(frontWheelDensity)));
	varList->push_back(new CfgVariable(V_FLOAT, &rearWheelDensity, getVarName(rearWheelDensity)));
	varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsRadius, getVarName(frontWheelsRadius)));
	varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsWidth, getVarName(frontWheelsWidth)));
	varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsRadius, getVarName(rearWheelsRadius)));
	varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsWidth, getVarName(rearWheelsWidth)));
	varList->push_back(new CfgVariable(V_FLOAT, &maxSteerAngle, getVarName(maxSteerAngle)));

	varList->push_back(new CfgVariable(V_VEC3, &chassisDimm, getVarName(chassisDimm)));

	varList->push_back(new CfgVariable(V_VEC3, &posFrontLeftWheel, getVarName(posFrontLeftWheel)));
	varList->push_back(new CfgVariable(V_VEC3, &posFrontRightWheel, getVarName(posFrontRightWheel)));
	varList->push_back(new CfgVariable(V_VEC3, &posRearLeftWheel, getVarName(posRearLeftWheel)));
	varList->push_back(new CfgVariable(V_VEC3, &posRearRightWheel, getVarName(posRearRightWheel)));*/

	varList->push_back(new CfgVariable(V_VEC3, &actionBoxDimm, getVarName(actionBoxDimm)));
	varList->push_back(new CfgVariable(V_VEC3, &actionBoxPos, getVarName(actionBoxPos)));

	return varList;
}

vector<CfgVariable*> *CombineParams::getVariableList()
{
	if(m_varList)
		return m_varList;
	m_varList = new vector<CfgVariable*>;

	m_varList->push_back(new CfgVariable(V_FLOAT, &chassisDensity, getVarName(chassisDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelDensity, getVarName(frontWheelDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelDensity, getVarName(rearWheelDensity)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &maxSteerAngle, getVarName(maxSteerAngle)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsRadius, getVarName(frontWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &frontWheelsWidth, getVarName(frontWheelsWidth)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsRadius, getVarName(rearWheelsRadius)));
	m_varList->push_back(new CfgVariable(V_FLOAT, &rearWheelsWidth, getVarName(rearWheelsWidth)));

	m_varList->push_back(new CfgVariable(V_VEC3, &posFrontLeftWheel, getVarName(posFrontLeftWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posFrontRightWheel, getVarName(posFrontRightWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posRearLeftWheel, getVarName(posRearLeftWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posRearRightWheel, getVarName(posRearRightWheel)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecFrontLeftWheelRotation, getVarName(vecFrontLeftWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecFrontRightWheelRotation, getVarName(vecFrontRightWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecRearLeftWheelRotation, getVarName(vecRearLeftWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecRearRightWheelRotation, getVarName(vecRearRightWheelRotation)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecChassisForward, getVarName(vecChassisForward)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posTriPodFront, getVarName(posTriPodFront)));
	m_varList->push_back(new CfgVariable(V_VEC3, &posTriPodBack, getVarName(posTriPodBack)));

	

	m_varList->push_back(new CfgVariable(V_VEC3, &posPipe, getVarName(posPipe)));
	m_varList->push_back(new CfgVariable(V_VEC3, &dumpPoint, getVarName(dumpPoint)));
	m_varList->push_back(new CfgVariable(V_VEC3, &vecPipeRotation, getVarName(vecPipeRotation)));
	m_varList->push_back(new CfgVariable(V_INT, &frontWheelsSteerable, getVarName(frontWheelsSteerable)));
	m_varList->push_back(new CfgVariable(V_INT, &capacity, getVarName(capacity)));
	

	return m_varList;
}

vector<CfgVariable*> *TriPodDimms::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;
	varList->push_back(new CfgVariable(V_FLOAT, &width, getVarName(width)));

	varList->push_back(new CfgVariable(V_VEC3, &posTopPod, getVarName(posTopPod)));
	varList->push_back(new CfgVariable(V_VEC3, &posBottomPod, getVarName(posBottomPod)));
	varList->push_back(new CfgVariable(V_VEC3, &posTopPodTractorConnector, getVarName(posTopPodTractorConnector)));
	varList->push_back(new CfgVariable(V_VEC3, &posBottomPodTractorConnector, getVarName(posBottomPodTractorConnector)));
	varList->push_back(new CfgVariable(V_VEC3, &posTopPodDeviceConnector, getVarName(posTopPodDeviceConnector)));
	varList->push_back(new CfgVariable(V_VEC3, &posBottomPodDeviceConnector, getVarName(posBottomPodDeviceConnector)));

	return varList;
}

vector<CfgVariable*> *AgriDeviceParams::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;
	varList->push_back(new CfgVariable(V_FLOAT, &density, getVarName(density)));
	varList->push_back(new CfgVariable(V_VEC3, &posTriPod, getVarName(posTriPod)));
	varList->push_back(new CfgVariable(V_VEC3, &dimm, getVarName(dimm)));
	return varList;
}

vector<CfgVariable*> *PlowDimms::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;
	varList->push_back(new CfgVariable(V_FLOAT, &density, getVarName(density)));
	varList->push_back(new CfgVariable(V_VEC3, &posTriPod, getVarName(posTriPod)));
	varList->push_back(new CfgVariable(V_VEC3, &dimm, getVarName(dimm)));
	return varList;
}

vector<CfgVariable*> *CutterParams::getVariableList()
{
	if(varList)
		return varList;
	varList = new vector<CfgVariable*>;
	varList->push_back(new CfgVariable(V_FLOAT, &density, getVarName(density)));
	varList->push_back(new CfgVariable(V_VEC3, &posTriPod, getVarName(posTriPod)));
	varList->push_back(new CfgVariable(V_VEC3, &dimm, getVarName(dimm)));
	varList->push_back(new CfgVariable(V_STRING, &cropTypeName, getVarName(cropTypeName)));
	varList->push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));
	varList->push_back(new CfgVariable(V_STRING, &cutterModel, getVarName(cutterModel)));
	
	return varList;
}