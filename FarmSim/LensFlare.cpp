#include "LensFlare.h"

LensFlare::LensFlare(string fname)
{
	m_flares = new Flare[11];
	string texture0 = "";
	string texture1 = "";
	string texture2 = "";
	string texture3 = "";
	string texture4 = "";
	string texture5 = "";
	string texture6 = "";
	string texture7 = "";
	string texture8 = "";
	string texture9 = "";
	string texture10 = "";

	float dist0;
	float dist1;
	float dist2;
	float dist3;
	float dist4;
	float dist5;
	float dist6;
	float dist7;
	float dist8;
	float dist9;
	float dist10;

	vector<CfgVariable*> vars;
	vars.push_back(new CfgVariable(V_STRING, &texture0, getVarName(texture0)));
	vars.push_back(new CfgVariable(V_STRING, &texture1, getVarName(texture1)));
	vars.push_back(new CfgVariable(V_STRING, &texture2, getVarName(texture2)));
	vars.push_back(new CfgVariable(V_STRING, &texture3, getVarName(texture3)));
	vars.push_back(new CfgVariable(V_STRING, &texture4, getVarName(texture4)));
	vars.push_back(new CfgVariable(V_STRING, &texture5, getVarName(texture5)));
	vars.push_back(new CfgVariable(V_STRING, &texture6, getVarName(texture6)));
	vars.push_back(new CfgVariable(V_STRING, &texture7, getVarName(texture7)));
	vars.push_back(new CfgVariable(V_STRING, &texture8, getVarName(texture8)));
	vars.push_back(new CfgVariable(V_STRING, &texture9, getVarName(texture9)));
	vars.push_back(new CfgVariable(V_STRING, &texture10, getVarName(texture10)));

	vars.push_back(new CfgVariable(V_FLOAT, &dist0, getVarName(dist0)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist1, getVarName(dist1)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist2, getVarName(dist2)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist3, getVarName(dist3)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist4, getVarName(dist4)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist5, getVarName(dist5)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist6, getVarName(dist6)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist7, getVarName(dist7)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist8, getVarName(dist8)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist9, getVarName(dist9)));
	vars.push_back(new CfgVariable(V_FLOAT, &dist10, getVarName(dist10)));
	CfgLoader(fname, &vars);

	
	if(texture0 != "")
	{
		m_flares[0].texture = gEngine.resMgr->load<Texture>(texture0);
		m_flares[0].distance = dist0;
	}
	if(texture1 != "")
	{
		m_flares[1].texture = gEngine.resMgr->load<Texture>(texture1);
		m_flares[1].distance = dist1;
	}
	if(texture2 != "")
	{
		m_flares[2].texture = gEngine.resMgr->load<Texture>(texture2);
		m_flares[2].distance = dist2;
	}
	if(texture3 != "")
	{
		m_flares[3].texture = gEngine.resMgr->load<Texture>(texture3);
		m_flares[3].distance = dist3;
	}
	if(texture4 != "")
	{
		m_flares[4].texture = gEngine.resMgr->load<Texture>(texture4);
		m_flares[4].distance = dist4;
	}
	if(texture5 != "")
	{
		m_flares[5].texture = gEngine.resMgr->load<Texture>(texture5);
		m_flares[5].distance = dist5;
	}
	if(texture6 != "")
	{
		m_flares[6].texture = gEngine.resMgr->load<Texture>(texture6);
		m_flares[6].distance = dist6;
	}
	if(texture7 != "")
	{
		m_flares[7].texture = gEngine.resMgr->load<Texture>(texture7);
		m_flares[7].distance = dist7;
	}
	if(texture8 != "")
	{
		m_flares[8].texture = gEngine.resMgr->load<Texture>(texture8);
		m_flares[8].distance = dist8;
	}
	if(texture9 != "")
	{
		m_flares[9].texture = gEngine.resMgr->load<Texture>(texture9);
		m_flares[9].distance = dist9;
	}
	if(texture10 != "")
	{
		m_flares[10].texture = gEngine.resMgr->load<Texture>(texture10);
		m_flares[10].distance = dist10;
	}
}

void LensFlare::render(ICamera *cam, Vec3 sunPos)
{
	Vec3 sunPosScreen;
	Vec2 vecSunToCenter;
	Mat world;
	Vec2 screenCenter;
	D3DVIEWPORT9 viewport;
	gEngine.device->getDev()->GetViewport(&viewport);
	D3DXMatrixTranslation(&world, 0, 0, 0);
	D3DXVec3Project(&sunPosScreen, &sunPos, &viewport, cam->getProjectionMat(), cam->getViewMat(), &world);
	//gEngine.renderer->addTxt("%.3f %.3f %.3f", sunPosScreen.x, sunPosScreen.y, sunPosScreen.z);
	screenCenter.x = viewport.Width/2;
	screenCenter.y = viewport.Height/2;
	vecSunToCenter = screenCenter - Vec2(sunPosScreen);

	Vec3 sunToCam = sunPos - *cam->getPosition();
	D3DXVec3Normalize(&sunToCam, &sunToCam);
	if(acos(D3DXVec3Dot(&sunToCam, cam->getViewDirection())) < cam->getFOV())

	for(int i = 0; i < 11; i++)
	{
		if(m_flares[i].texture)
		{
			//Vec2 flarePos = vecSunToCenter * m_flares[i].distance + Vec2(sunPosScreen.x, sunPosScreen.y);
			Vec2 flarePos = vecSunToCenter * m_flares[i].distance + Vec2(sunPosScreen.x, sunPosScreen.y);
			gEngine.renderer->r2d->drawQuad(m_flares[i].texture, flarePos.x - m_flares[i].texture->width/2,
																 flarePos.y - m_flares[i].texture->height/2, 
																 flarePos.x + m_flares[i].texture->width/2, 
																 flarePos.y + m_flares[i].texture->height/2, 
																 D3DCOLOR_ARGB(255, 255, 255, 255), 
																 D3DCOLOR_ARGB(255, 255, 255, 255),
																 D3DCOLOR_ARGB(255, 255, 255, 255),
																 D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}
}