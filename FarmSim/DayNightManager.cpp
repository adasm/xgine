#include "DayNightManager.h"

vector<CfgVariable*> *LightParams::getVariableList()
{
	if(m_vars)
		return m_vars;
	m_vars = new vector<CfgVariable*>;
	m_vars->push_back(new CfgVariable(V_INT, &type, getVarName(type)));
	m_vars->push_back(new CfgVariable(V_INT, &castsShadows, getVarName(castsShadows)));
	m_vars->push_back(new CfgVariable(V_VEC4, &color, getVarName(color)));
	m_vars->push_back(new CfgVariable(V_FLOAT, &range, getVarName(range)));
	m_vars->push_back(new CfgVariable(V_FLOAT, &phi, getVarName(phi)));
	return m_vars;
}

LIGHT_TYPE LightParams::getType()
{
	return (LIGHT_TYPE)type;
}

DayNightManager::DayNightManager(Scene* scene, float minuteLength, float startTime, string fnameLens)
{
	m_sunEntity = new SunEntity(new LensFlare(fnameLens));
	core.game->getWorld()->addToWorld(m_sunEntity, NO_COLLISION, 0, GROUP_NON_COLLIDABLE);
	if(startTime > 24)
		startTime = 24;
	if(startTime < 0)
		startTime = 0;
	m_scene = scene;
	m_minuteLength = minuteLength;
	m_hours = (int)startTime;
	m_minutes = (startTime - (int)startTime) * 60.0f;
	m_elapsedTime = 0;
	m_scene->getSky()->setTextureFrontScale(0.5f);
}

void DayNightManager::update()
{
	m_elapsedTime += gEngine.kernel->tmr->get();
	if(m_elapsedTime >= m_minuteLength)
	{
		m_minutes++;
		m_elapsedTime = 0;
	}
	if(m_minutes > 59)
	{
		m_minutes = 0;
		m_hours++;
	}
	if(m_hours > 23)
		m_hours = 0;

	if(m_hours == 22)
		core.game->getWorld()->newTurn();

	Mat rot, trans;
	float time = m_hours + ((m_minutes-1) / 60.0f) + ((m_elapsedTime / m_minuteLength) / 60.0f);
	bool pm = false;
	if(time > 12)
		pm = true;

	/*m_scene->setColorFront(Vec4(0.8,0.7,0.2,1));
	m_scene->setColorBack(Vec4(0.6,0.6,0.8,1));*/

	D3DXMatrixRotationZ(&rot, (time / 24.0f) * 2.0f * D3DX_PI);
	D3DXMatrixTranslation(&trans, 0, 50, 0);
	D3DXVec3TransformNormal(&m_sunPos, &Vec3(0, -1, 0), &rot);
	D3DXVec3TransformNormal(&m_sunPos, &m_sunPos, &trans);
	D3DXVec3Normalize(&m_sunPos, &m_sunPos);
	m_scene->setSun(m_sunPos * -1, Vec4(0.8, 0.9, 1, 1));
	float xMultiplier = m_sunPos.x / 1;
	float yMultiplier = D3DXVec3Dot(&m_sunPos, &Vec3(0, 1, 0));
	if(xMultiplier < 0)
		xMultiplier *= -1;
	float timeMultiplier = time / 12;
	if(pm)
		timeMultiplier = (12 - (time - 12)) / 12;
	m_sunPos *= 50000;
	m_scene->getSky()->setColorFront(Vec4(0.8f, 0.7f, 0.2f, 1));
	m_scene->getSky()->setColorFront(Vec4(timeMultiplier * 0.7f + xMultiplier * 0.1f, timeMultiplier * 0.8f, timeMultiplier * 0.8f, 1));
	m_scene->getSky()->setColorBack(Vec4(timeMultiplier * 0.6f + xMultiplier * 0.05f, timeMultiplier * 0.65f, timeMultiplier * 1.0f, 1));

	m_sunEntity->position = m_sunPos;
	worldMat(&m_sunEntity->world, m_sunEntity->position, Vec3(0, 0, 0));
	m_sunEntity->boundingBox.setTransform(m_sunEntity->position);
	gEngine.renderer->addTxt("Time: %d:%d\nSunPos: %.5f %.5f %.5f", m_hours, m_minutes, m_sunPos.x, m_sunPos.y, m_sunPos.z);
	if(m_hours >= 20)
	{
		if(m_lights.size())
			if(!m_lights[0]->getEnabled())
				for(int i = 0; i < m_lights.size(); i++)
					m_lights[i]->setEnabled(true);
	}
	else
	if(m_hours >= 6)
	{
		if(m_lights.size())
			if(m_lights[0]->getEnabled())
				for(int i = 0; i < m_lights.size(); i++)
					m_lights[i]->setEnabled(false);
	}
}

int DayNightManager::getHour()
{
	return m_hours;
}

int DayNightManager::getMinutes()
{
	return m_minutes;
}

void DayNightManager::renderLensFlare()
{
	if(m_sunEntity->isToRender())
		m_sunEntity->renderLens();
}

void DayNightManager::addLight(string fname, Vec3 position, Vec3 direction)
{
	map<string, LightParams*>::iterator it;
	it = m_lightTypes.find(fname);
	if(it != m_lightTypes.end())
		m_lights.push_back(new Light(it->second->getType(), it->second->castsShadows, it->second->color, position, it->second->range, direction, it->second->phi));
	else
	{
		LightParams* light = new LightParams;
		CfgLoader cfg("Lights\\" + fname, light->getVariableList());
		m_lights.push_back(new Light(light->getType(), light->castsShadows, light->color, position, light->range, direction, light->phi));
	}
	core.game->getWorld()->addToWorld(m_lights[m_lights.size()-1]);
}