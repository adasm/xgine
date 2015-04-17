#include "SkyDome.h"

struct XGINE_API VertexSkyDome
{
	Vec3 pos;
	Vec2 tex;
	Vec2 col;
};

/* hori_res and vert_res: 
   Controls the number of faces along the horizontal axis (30 is a good value) 
   and the number of faces along the vertical axis (8 is a good value). 

   image_percentage: 
   Only the top image_percentage of the image is used, e.g. 0.8 uses the top 80% of the image, 
   1.0 uses the entire image. This is useful as some landscape images have a small banner 
   at the bottom that you don't want. 

   half_sphere_fraction: 
   This controls how far around the sphere the sky dome goes. For value 1.0 you get exactly the upper 
   hemisphere, for 1.1 you get slightly more, and for 2.0 you get a full sphere. It is sometimes useful 
   to use a value slightly bigger than 1 to avoid a gap between some ground place and the sky. This 
   parameters stretches the image to fit the chosen "sphere-size". */ 

SkyDome::SkyDome(Texture* textureFront, Texture* textureBack, f32 textureFrontScale, f32 textureBackScale, Vec4 colorFront, Vec4 colorBack, f32 slideFront, f32 slideBack, Vec2 slideDir, u32 hori_res, u32 vert_res, float image_percentage, float half_sphere_fraction)
	: g_sunLight(0)
{ 
	m_colorFront = colorFront;
	m_colorBack = colorBack;
	m_textureFront = textureFront;
	m_textureBack = textureBack;
	m_textureFrontScale = textureFrontScale;
	m_textureBackScale = textureBackScale;
	m_slideFactorFront = slideFront;
	m_slideFactorBack = slideBack;
	m_slideDir = slideDir;

	m_shader  = gEngine.shaderMgr->load("SkyDome.fx");
	double radiusY = 800;               /* Adjust this to get more or less perspective distorsion. */ 
	double radiusXZ = 800;
	double azimuth, azimuth_step; 
	double elevation, elevation_step; 
	int k, j, c; 

	VertexSkyDome vtx; 

	azimuth_step = 2.*D3DX_PI/(double)hori_res; 
	elevation_step = half_sphere_fraction*D3DX_PI/2./(double)vert_res; 

	NumOfVertices = (hori_res+1)*(vert_res+1); 
	NumOfFaces = (2*vert_res-1)*hori_res; 

	VertexSkyDome *Vertices = new VertexSkyDome[NumOfVertices]; 
	WORD *Indices = new u16[3*NumOfFaces]; 

	//vtx.col = Vec4(1,1,1,1);

	c = 0; 
	for (k = 0, azimuth = 0; k <= hori_res; k++, azimuth += azimuth_step) 
	{ 
		for (j = 0, elevation = D3DX_PI/2.; j <= vert_res; j++, elevation -= elevation_step) 
		{ 
			vtx.pos = Vec3(radiusXZ*cos(elevation)*sin(azimuth),radiusY*sin(elevation),radiusXZ*cos(elevation)*cos(azimuth)); 
			vtx.tex = Vec2((f32)vtx.pos.x/(f32)radiusXZ, (f32)vtx.pos.z/(f32)radiusXZ); //Vec2((f32)k/(f32)hori_res, (f32)j/(f32)vert_res*image_percentage); 
			
			if(vtx.pos.y < 150) vtx.col = Vec2((400 - vtx.pos.y) / 150, 0);
			else vtx.col = Vec2(0,0);
			Vertices[c++] = vtx; 
		} 
	} 

	c = 0; 
	for (k = 0; k < hori_res; k++) 
	{ 
		Indices[c++] = vert_res+2+(vert_res+1)*k; 
		Indices[c++] = 1+(vert_res+1)*k; 
		Indices[c++] = 0+(vert_res+1)*k; 

		for (j = 1; j < vert_res; j++) 
		{ 
			Indices[c++] = vert_res+2+(vert_res+1)*k+j; 
			Indices[c++] = 1+(vert_res+1)*k+j; 
			Indices[c++] = 0+(vert_res+1)*k+j; 

			Indices[c++] = vert_res+1+(vert_res+1)*k+j; 
			Indices[c++] = vert_res+2+(vert_res+1)*k+j; 
			Indices[c++] = 0+(vert_res+1)*k+j; 
		} 
	}
	
	gEngine.device->getDev()->CreateVertexBuffer(sizeof(VertexSkyDome) * NumOfVertices, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb, 0);
	gEngine.device->getDev()->CreateIndexBuffer(NumOfFaces*3*sizeof(WORD), D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_ib, 0);
	VertexSkyDome*	data;
	WORD*			idata;

	m_vb->Lock(0, 0, (void**)&data, 0);
	m_ib->Lock(0, 0, (void**)&idata, 0);
	
	for(u32 i = 0; i < NumOfVertices; i++)
		data[i] = Vertices[i];
	
	for(u32 i = 0; i < 3*NumOfFaces; i++)
		idata[i] = Indices[i];

	m_vb->Unlock();
	m_ib->Unlock();

	delete(Vertices);
	delete(Indices);

	plane = gEngine.resMgr->load<Mesh>("%gm plane 25000 10");
}	

SkyDome::~SkyDome() 
{ 
	DXRELEASE(m_vb);
	DXRELEASE(m_ib);
	gEngine.resMgr->release(m_textureFront);
	gEngine.resMgr->release(m_textureBack);
	gEngine.resMgr->release(plane);
} 

void SkyDome::update(ICamera* cam)
{
}

u32 SkyDome::render() 
{
	LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
	Vec3 pos = *gEngine.renderer->get().camera->getPosition();// - Vec3(0, 150, 0);


	//Sky
	gEngine.renderer->setShader(m_shader->setTech("Sky"));
	worldMat(&world, pos);
	gEngine.renderer->setWorld(&world);

	gEngine.renderer->state.setZEnable(FALSE);
	gEngine.renderer->state.setZWriteEnable(FALSE);
	gEngine.renderer->state.setClipPlaneEnable(FALSE);
	gEngine.renderer->state.setCullMode(D3DCULL_NONE);

	m_shader->setTex("g_skyTextureBack", m_textureBack);
	m_shader->setF32("g_skyTextureBackScale", m_textureBackScale);
	m_shader->setVec4("g_skyColorBack", &m_colorBack);
	m_shader->setF32("g_skySlideFactorBack", m_slideFactorBack);
	m_shader->setF32("g_skyTimeFactor", GetCurrentTime()*0.0001f);

	if(g_sunLight)
	{
		m_shader->setVec3("g_sunDir", g_sunLight->getDir());
		m_shader->setVec4("g_sunColor", g_sunLight->getColor());
	}
	else
	{
		m_shader->setVec3("g_sunDir", &Vec3(0, -1, 0));
		m_shader->setVec4("g_sunColor", &Vec4(1,0,0,1));
	}

	m_shader->setF32("g_sunSizeExp",		r_sunSizeExp);
	
	m_shader->setVec4("g_fogParams",	&r_fogParams);
	m_shader->setVec3("g_fogColor",		&r_fogColor);

	gEngine.renderer->commitChanges();
	
	dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_SKYDOME]);
	dev->SetStreamSource(0, m_vb, 0, sizeof(VertexSkyDome));
	dev->SetIndices(m_ib);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, NumOfVertices, 0, NumOfFaces);
	gEngine.renderer->addRenderedCounts(NumOfVertices, NumOfFaces);

	//Clouds
	gEngine.renderer->setShader(m_shader->setTech("Clouds"));
	worldMat(&world, Vec3(-12500,1000,-12500), Vec3(0,0,0));
	gEngine.renderer->setWorld(&world);

	gEngine.renderer->state.setZEnable(FALSE);
	gEngine.renderer->state.setZWriteEnable(FALSE);
	gEngine.renderer->state.setClipPlaneEnable(FALSE);
	gEngine.renderer->state.setAlphaBlendEnable(TRUE);
	gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
	gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
	gEngine.renderer->state.setDestBlend(D3DBLEND_ONE);
	gEngine.renderer->state.setCullMode(D3DCULL_NONE);

	m_shader->setTex("g_skyTextureFront", m_textureFront);
	m_shader->setF32("g_skyTextureFrontScale", m_textureFrontScale);
	m_shader->setVec4("g_skyColorFront", &m_colorFront);
	m_shader->setF32("g_skySlideFactorFront", m_slideFactorFront);
	m_shader->setVec2("g_skySlideDir", &m_slideDir);

	gEngine.renderer->commitChanges();
	plane->render();
	

	gEngine.renderer->state.setAlphaBlendEnable(FALSE);
	gEngine.renderer->state.setZEnable(TRUE);
	gEngine.renderer->state.setZWriteEnable(TRUE);
	return 1;
}

u32 SkyDome::renderSun()
{
	LPDIRECT3DDEVICE9 dev = gEngine.device->getDev();
	Vec3 pos = *gEngine.renderer->get().camera->getPosition();// - Vec3(0, 150, 0);

	gEngine.renderer->state.setZEnable(FALSE);
	gEngine.renderer->state.setZWriteEnable(FALSE);

	gEngine.renderer->setShader(m_shader->setTech("SunOnly"));
	worldMat(&world, pos);
	gEngine.renderer->setWorld(&world);

	gEngine.renderer->state.setClipPlaneEnable(FALSE);
	gEngine.renderer->state.setCullMode(D3DCULL_NONE);

	if(g_sunLight)
	{
		m_shader->setVec3("g_sunDir", g_sunLight->getDir());
	}
	else
	{
		m_shader->setVec3("g_sunDir", &Vec3(0, -1, 0));
	}

	m_shader->setF32("g_sunSizeExp",		r_sunSizeExp);
	gEngine.renderer->commitChanges();
	
	dev->SetVertexDeclaration(gEngine.renderer->m_vd[VD_SKYDOME]);
	dev->SetStreamSource(0, m_vb, 0, sizeof(VertexSkyDome));
	dev->SetIndices(m_ib);
	dev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, NumOfVertices, 0, NumOfFaces);

	gEngine.renderer->addRenderedCounts(NumOfVertices, NumOfFaces);

	//Clouds
	gEngine.renderer->setShader(m_shader->setTech("CloudsB"));
	worldMat(&world, Vec3(-12500,1000,-12500), Vec3(0,0,0));
	gEngine.renderer->setWorld(&world);

	gEngine.renderer->state.setClipPlaneEnable(FALSE);

	m_shader->setTex("g_skyTextureFront", m_textureFront);
	m_shader->setF32("g_skyTextureFrontScale", m_textureFrontScale);
	m_shader->setVec4("g_skyColorFront", &m_colorFront);
	m_shader->setF32("g_skySlideFactorFront", m_slideFactorFront);
	m_shader->setVec2("g_skySlideDir", &m_slideDir);

	gEngine.renderer->state.setZEnable(FALSE);
	gEngine.renderer->state.setZWriteEnable(FALSE);
	gEngine.renderer->state.setClipPlaneEnable(FALSE);
	gEngine.renderer->state.setAlphaBlendEnable(TRUE);
	gEngine.renderer->state.setBlendOp(D3DBLENDOP_ADD);
	gEngine.renderer->state.setSrcBlend(D3DBLEND_SRCALPHA);
	gEngine.renderer->state.setDestBlend(D3DBLEND_INVSRCALPHA);
	gEngine.renderer->state.setCullMode(D3DCULL_NONE);

	gEngine.renderer->commitChanges();
	plane->render();

	gEngine.renderer->state.setZEnable(TRUE);
	gEngine.renderer->state.setZWriteEnable(TRUE);
	return 1;
}