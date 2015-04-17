#pragma once
#include "Headers.h"

struct LightParams
{
	int type;
	LIGHT_TYPE getType();
	u32 castsShadows;
	Vec4 color;
	f32 range;
	f32 phi;
	vector<CfgVariable*> *getVariableList();
	LightParams()
	{
		type = 0;
		castsShadows = 0;
		color = Vec4(1,1,1,1);
		range = 50.0f;
		phi = 0;
		m_vars = NULL;
	}
protected:
	vector<CfgVariable*> *m_vars;
};

class DayNightManager
{
public:
	DayNightManager(Scene* scene, float minuteLength, float startTime, string fnameLens);

	void			addLight(string fname, Vec3 position, Vec3 direction);
	void			renderLensFlare();
	void			update();
	int				getHour();
	int				getMinutes();
	void			setTime(int hour, int minute) { m_hours = hour; m_minutes = minute; }
	Vec3			getSunPos() { return m_sunPos; }
protected:
	SunEntity*		m_sunEntity;
	Scene*			m_scene;
	float			m_minuteLength;
	float			m_elapsedTime;
	int				m_hours;
	int				m_minutes;
	Vec3			m_sunPos;
	Vec4			m_sunColor;
	vector<Light*>	m_lights;
	map<string, LightParams*> m_lightTypes;
};