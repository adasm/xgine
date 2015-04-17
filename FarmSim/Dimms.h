#pragma once
#include "Headers.h"


struct VehicleParams
{
	float chassisDensity;
	float frontWheelDensity;
	float rearWheelDensity;

	float maxSteerAngle;

	Vec3 chassisDimm;
	float frontWheelsRadius;
	float frontWheelsWidth;

	float rearWheelsRadius;
	float rearWheelsWidth;
	int wheelDriveType;
	int fuelCapacity;
	int fuelConsumptionPerMth;

	Vec3 posFrontLeftWheel;
	Vec3 posFrontRightWheel;
	Vec3 posRearLeftWheel;
	Vec3 posRearRightWheel;

	Vec3 vecFrontLeftWheelRotation;
	Vec3 vecFrontRightWheelRotation;
	Vec3 vecRearLeftWheelRotation;
	Vec3 vecRearRightWheelRotation;
	Vec3 vecChassisForward;

	Vec3 posTriPodFront;
	Vec3 posTriPodBack;

	VehicleParams()
	{
		chassisDensity = 0.0f;
		frontWheelDensity = 0.0f;
		rearWheelDensity = 0.0f;

		maxSteerAngle = 0.0f;

		frontWheelsRadius = 0.0f;
		frontWheelsWidth = 0.0f;

		rearWheelsRadius = 0.0f;
		rearWheelsWidth = 0.0f;
		wheelDriveType = 0;
		fuelCapacity = 0;
		fuelConsumptionPerMth = 0;

		chassisDimm = Vec3(0.0f, 0.0f, 0.0f);
		posFrontLeftWheel = Vec3(0.0f, 0.0f, 0.0f);
		posFrontRightWheel = Vec3(0.0f, 0.0f, 0.0f);
		posRearLeftWheel = Vec3(0.0f, 0.0f, 0.0f);
		posRearRightWheel = Vec3(0.0f, 0.0f, 0.0f);

		vecFrontLeftWheelRotation = Vec3(0.0f, 0.0f, 0.0f);
		vecFrontRightWheelRotation = Vec3(0.0f, 0.0f, 0.0f);
		vecRearLeftWheelRotation = Vec3(0.0f, 0.0f, 0.0f);
		vecRearRightWheelRotation = Vec3(0.0f, 0.0f, 0.0f);
		vecChassisForward = Vec3(0.0f, 0.0f, 0.0f);

		posTriPodFront = Vec3(0.0f, 0.0f, 0.0f);
		posTriPodBack = Vec3(0.0f, 0.0f, 0.0f);
		m_varList = NULL;
	}

	virtual vector<CfgVariable*>*	getVariableList();
protected:
	vector<CfgVariable*> *m_varList;
};

struct VehiclePose
{
	Mat matChassis;
	Mat matFrontLeftWheel;
	Mat matFrontRightWheel;
	Mat matRearLeftWheel;
	Mat matRearRightWheel;
};

struct TrailerParams// : VehicleParams
{
	Vec3 posDumperHinge;
	Vec3 posHarvest;
	Vec3 posDumper;
	Vec3 dumperActionBoxMin;
	Vec3 dumperActionBoxMax;
	Vec3 posPayloadDump;
	float maxHarvestHeight;
	int capacity;

	TrailerParams()
	{
		posPayloadDump = Vec3(0, 0, 0);
		dumperActionBoxMin = Vec3(0, 0, 0);
		dumperActionBoxMax = Vec3(0, 0, 0);
		posDumperHinge = Vec3(0, 0, 0);
		posHarvest = Vec3(0, 0, 0);
		posDumper = Vec3(0, 0, 0);
		maxHarvestHeight = 0.0f;
		capacity = 0;

		m_varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();

protected:
	vector<CfgVariable*> *m_varList;
};



struct WheelSetPose
{
	Mat matWheelLeftFront;
	Mat matWheelRightFront;

	Mat matWheelLeftMiddle;
	Mat matWheelRightMiddle;

	Mat matWheelLeftRear;
	Mat matWheelRightRear;

	Mat matChassis;

	WheelSetPose()
	{
		D3DXMatrixIdentity(&matChassis);
		D3DXMatrixIdentity(&matWheelLeftFront);
		D3DXMatrixIdentity(&matWheelRightFront);
		D3DXMatrixIdentity(&matWheelLeftMiddle);
		D3DXMatrixIdentity(&matWheelRightMiddle);
		D3DXMatrixIdentity(&matWheelLeftRear);
		D3DXMatrixIdentity(&matWheelRightRear);
	}
};


struct TrailerPose : WheelSetPose
{
	Mat matDumper;
	Mat matHarvest;

	TrailerPose()
	{
		D3DXMatrixIdentity(&matChassis);
		D3DXMatrixIdentity(&matWheelLeftFront);
		D3DXMatrixIdentity(&matWheelRightFront);
		D3DXMatrixIdentity(&matWheelLeftMiddle);
		D3DXMatrixIdentity(&matWheelRightMiddle);
		D3DXMatrixIdentity(&matWheelLeftRear);
		D3DXMatrixIdentity(&matWheelRightRear);

		D3DXMatrixIdentity(&matDumper);
		D3DXMatrixIdentity(&matHarvest);
	}
};

struct CombinePose : VehiclePose
{
	Mat matPipe;
};

struct CombineParams : VehicleParams
{
	Vec3 dumpPoint;
	Vec3 vecPipeRotation;
	Vec3 posPipe;
	int frontWheelsSteerable;
	int capacity;

	CombineParams()
	{
		dumpPoint = Vec3(0, 0, 0);
		vecPipeRotation = Vec3(0, 0, 0);
		posPipe = Vec3(0, 0, 0);
		frontWheelsSteerable = 0;
		capacity = 0;

		m_varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();
/*protected:
	vector<CfgVariable*> *varList;*/
};

struct TriPodDimms
{
	Vec3 posTopPod;
	Vec3 posBottomPod;

	float width;

	Vec3 posTopPodTractorConnector;
	Vec3 posBottomPodTractorConnector;

	Vec3 posTopPodDeviceConnector;
	Vec3 posBottomPodDeviceConnector;

	TriPodDimms()
	{
		Vec3 posTopPod = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 posBottomPod = Vec3(0.0f, 0.0f, 0.0f);

		float width = 0.0f;

		Vec3 posTopPodTractorConnector = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 posBottomPodTractorConnector = Vec3(0.0f, 0.0f, 0.0f);

		Vec3 posTopPodDeviceConnector = Vec3(0.0f, 0.0f, 0.0f);
		Vec3 posBottomPodDeviceConnector = Vec3(0.0f, 0.0f, 0.0f);
		varList = NULL;
	}

	vector<CfgVariable*> *getVariableList();
protected:
	vector<CfgVariable*> *varList;
};

struct AgriDeviceParams
{
	string chassisModel;
	Vec3 posTriPod;
	float density;
	Vec3 dimm;
	AgriDeviceParams()
	{
		chassisModel = "";
		density = 0;
		dimm = Vec3(0, 0, 0);
		posTriPod = Vec3(0, 0, 0);
		varList = NULL;
	}
	virtual vector<CfgVariable*> *getVariableList();
protected:
	vector<CfgVariable*> *varList;
};

struct CultivatorParams : AgriDeviceParams
{
	Vec3 actionBoxDimm;
	Vec3 actionBoxPos;
	CultivatorParams()
	{
		actionBoxDimm = Vec3(0, 0, 0);
		actionBoxPos = Vec3(0, 0, 0);

		varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();
};

struct PlowDimms : AgriDeviceParams
{
	PlowDimms()
	{
		density = 0;
		dimm = Vec3(0, 0, 0);
		posTriPod = Vec3(0, 0, 0);
		varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();
};

struct CutterParams : AgriDeviceParams
{
	string cutterModel;
	string cropTypeName;
	CutterParams()
	{
		cutterModel = "";
		cropTypeName = "";
		density = 0;
		dimm = Vec3(0, 0, 0);
		posTriPod = Vec3(0, 0, 0);
		varList = NULL;
	}
	vector<CfgVariable*> *getVariableList();
};