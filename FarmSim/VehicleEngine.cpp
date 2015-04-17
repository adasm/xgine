//#include "VehicleEngine.h"
///*
//public:
//	VehicleEngine(/*float maxTorque, float minTorque, int maxRPM, int gearNum);
//
//	float	getTorque();
//	int		getRPM();
//
//	int		getGear();
//
//	void	accelerate();
//
//	void	update();*/
//
//VehicleEngine::VehicleEngine()
//{
//	m_maxTorque = 4.0f;
//	m_minTorque = 0.0f;
//
//	m_maxRPM = 1000;
//	m_gearNum = 6;
//	
//	m_actualGear = 0;
//	m_actualRPM = 0;
//
//	m_lastAccelerate = 0;
//	m_thisFrameAccelerated = false;
//	m_nowAccelerating = false;
//	m_lastRelease = 0;
//	m_released = false;
//}
//
//float VehicleEngine::getTorque()
//{
//	float actualGear = m_actualGear;
//	actualGear += (float)m_actualRPM / (float)m_maxRPM;
//	float torquePercent = (-10 + m_gearNum-1) * (actualGear*actualGear) + 100;
//	torquePercent *= 0.01f;
//	return m_maxTorque * torquePercent;
//}
//
//int VehicleEngine::getRPM()
//{
//	return m_actualRPM;
//}
//
//int VehicleEngine::getGear()
//{
//	return m_actualGear;
//}
//
//void VehicleEngine::accelerate()
//{
//	if(!m_nowAccelerating)
//		m_lastAccelerate = GetTickCount();
//	m_nowAccelerating = true;
//	m_thisFrameAccelerated = true;
//	m_released = false;
//}
//
//void VehicleEngine::update()
//{
//	if(m_nowAccelerating)
//	{
//		if(m_actualRPM < m_maxRPM)
//			m_actualRPM += pow(GetTickCount() - m_lastAccelerate, 2) * 0.00001f;
//		else
//		{
//			if(m_actualGear < m_gearNum-1)
//			{
//				m_actualGear++;
//				m_actualRPM = 0;
//			}
//			else
//				m_actualRPM = m_maxRPM;
//		}
//	}
//	else
//	{
//		if(!m_released)
//		{
//			m_lastRelease = GetTickCount();
//			m_released = true;
//		}
//		if(m_actualRPM > 0)
//			m_actualRPM -= pow(GetTickCount() - m_lastRelease, 2) * 0.001f;
//		else
//		{
//			if(m_actualGear > 0)
//			{
//				m_actualGear--;
//				m_actualRPM = m_maxRPM;
//			}
//			else
//				m_actualRPM = 0;
//		}
//	}
//	if(!m_thisFrameAccelerated)
//		m_nowAccelerating = false;
//	m_thisFrameAccelerated = false;
//	gEngine.renderer->addTxt("Gear: %d RPM: %d torque: %f lastAccelerate: %d", m_actualGear, m_actualRPM, getTorque(), m_lastAccelerate);
//}


#include "VehicleEngine.h"

VehicleEngine::VehicleEngine(string fname)
{

	///loading params
	vector<CfgVariable*> var;
	var.push_back(new CfgVariable(V_INT, &m_gearNum, "gearNum"));
	var.push_back(new CfgVariable(V_FLOAT, &m_slowingRate, "slowingRate"));
	var.push_back(new CfgVariable(V_FLOAT, &m_brakeRate, "brakeRate"));
	var.push_back(new CfgVariable(V_FLOAT, &m_rMaxSpeed, "reverseMaxSpeed"));
	CfgLoader(fname, &var);
	m_maxSpeeds.resize(m_gearNum);
	m_accelerations.resize(m_gearNum);
	for(int i = 0; i < m_gearNum; i++)
	{
		var.push_back(new CfgVariable(V_FLOAT, &m_maxSpeeds[i], "maxSpeed" + toString<int>(i)));
		var.push_back(new CfgVariable(V_FLOAT, &m_accelerations[i], "acceleration" + toString<int>(i)));
	}
	CfgLoader(fname, &var);

	m_actualAcceleration = 0;
	
	m_actualGear = 0;
	m_actualRPM = 0.0f;		//Range 0 - 1.0f
	m_actualSpeed = 0.0f;
	m_actualBrake = 0.0f;

	m_nowAccelerating = false;
	m_nowBraking = false;
	m_nowReversing = false;
	m_released = false;
}

void VehicleEngine::setSpeed(float speed)
{
	m_actualSpeed = speed;
}

float VehicleEngine::getRPM()
{
	if(m_actualGear <= 0)
		m_actualRPM = m_actualSpeed / m_maxSpeeds[0];
	else
		m_actualRPM = (m_actualSpeed - m_maxSpeeds[m_actualGear-1]) / (m_maxSpeeds[m_actualGear] - m_maxSpeeds[m_actualGear-1]);
	return m_actualRPM;
}

int VehicleEngine::getGear()
{
	return m_actualGear;
}

float VehicleEngine::getSpeed()
{
	return m_actualSpeed;
}

void VehicleEngine::setAccelerate()
{
	m_nowAccelerating = true;
	m_released = false;
}

void VehicleEngine::setReverse()
{
	m_nowReversing = true;
	m_released = false;
}

void VehicleEngine::setBrake()
{
	m_nowBraking = true;
	m_released = false;
}

float VehicleEngine::getAcceleration()
{
	return m_actualAcceleration;
}

void VehicleEngine::update()
{
	//Gówno prawda z timeModifierem... PhysX ma w dupie czas...
	float timeModifier = 20.0f;//1000.0f / gEngine.renderer->getFrameTime();//core.dynamics->getDeltaTime() * 200.0f;//gEngine.kernel->tmr->get() * 0.005f;
	m_actualAcceleration = 0;
	m_actualBrake = 0;
	if(m_nowAccelerating)
	{
		if(m_actualSpeed < m_maxSpeeds[m_gearNum-1])
			m_actualAcceleration += m_accelerations[m_actualGear] / timeModifier;
	}
	else
	if(m_nowBraking)
	{
		m_actualBrake += m_brakeRate / timeModifier;
	}
	else
	if(m_nowReversing)
	{
		if(m_actualSpeed > -m_rMaxSpeed)
			m_actualAcceleration -= m_accelerations[0] / timeModifier;
	}
	else
	{
		if(m_actualSpeed > 0)
			m_actualAcceleration -= m_slowingRate / timeModifier;
		else
			m_actualAcceleration = 0;
	}
	if(m_actualGear < m_gearNum-1)
		if(m_actualSpeed > m_maxSpeeds[m_actualGear])
			m_actualGear++;
	if(m_actualGear > 0)
		if(m_actualSpeed < m_maxSpeeds[m_actualGear-1] - (m_maxSpeeds[m_actualGear-1] * 0.1f))
			m_actualGear--;
	
	//gEngine.renderer->addTxt("RPM: %.3f ACTUAL GEAR: %d SPEED: %.3f", getRPM(), m_actualGear+1, m_actualSpeed);
	m_actualSpeed += m_actualAcceleration;
	m_nowReversing = m_nowAccelerating = m_nowBraking = false;
}

float VehicleEngine::getBrake()
{
	return m_actualBrake;
}