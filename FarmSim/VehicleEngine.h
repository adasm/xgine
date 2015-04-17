#pragma once
#include "Headers.h"

struct VehicleEngineParams
{
	int gearNum;
	float *gearMaxSpeeds;
	float rMaxSpeed;		///Must be less then 0

	float *gearAccelerations;		//in m/s^2
	float slowingRate;				//in m/s^2


	/*float maxTorque;
	float minTorque;

	int maxRPM;
	int minRPM;

	int gearNum;
	int *gearsRanges;

	int minRPMToGearUp;*/


	/*vector<CfgVariable*> *getVariableList();
protected:
	vector<CfgVariable*> *m_varList;*/
	void loadFromFile(string fname);
};

class VehicleEngine
{
public:
	VehicleEngine(string fname/*float maxTorque, float minTorque, int maxRPM, int gearNum*/);

	float	getRPM();	//Range 0.0 - 1.0f;
	float	getSpeed();
	int		getGear();	//-1: R

	int		getGearNum() { return m_gearNum; }

	void	setAccelerate();
	void	setBrake();
	void	setReverse();
	void	setSpeed(float speed);

	float	getAcceleration();
	float	getBrake();

	void	update();
protected:
	int m_gearNum;
	vector<float> m_maxSpeeds;
	vector<float> m_accelerations;

	float m_rMaxSpeed;

	float m_slowingRate;

	float m_actualAcceleration;
	float m_actualBrake;
	
	int m_actualGear;
	float m_actualRPM;		//Range 0 - 1.0f
	float m_actualSpeed;

	float m_brakeRate;

	bool m_nowAccelerating;
	bool m_nowBraking;
	bool m_released;
	bool m_nowReversing;
};