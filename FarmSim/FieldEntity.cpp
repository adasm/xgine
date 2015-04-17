#include "FieldEntity.h"

FieldEntity::FieldEntity(Vec3 position, int width, int height, FieldSectorPTR* sectors, int secNum)
{
	mesh = 0;

	m_sectors		= sectors;
	m_width			= width;
	m_height		= height;
	m_sectorNum		= secNum;

	position = position;
	rotation = Vec3(0,0,0);

	worldMat(&world, position, rotation);
	D3DXMatrixInverse(&invWorld, 0, &world);
	m_isEmpty = false;
	m_regenerateRenderList = true;
	for(int i = 0; i < m_sectorNum; i++)
	{
		m_sectors[i]->setFieldEntity(this);
	}

	//if(m_mesh)
	{
		//boundingBox = (grassBlade->getBoundingBox());
		//boundingBox.reset(position, Vec3(position.x + (width * core.game->getWorld()->getGrassManager()->getTileSize() * core.game->getWorld()->getGrassManager()->getSectorSize()), 100, position.x + (height * core.game->getWorld()->getGrassManager()->getTileSize() * core.game->getWorld()->getGrassManager()->getSectorSize())));
		Vec3 diagonal = Vec3((core.game->getWorld()->getGrassManager()->getMapSize() / g_grassSectorSize / g_grassEntitiesNum) * width * core.game->getWorld()->getGrassManager()->getTileSize(), 500, (core.game->getWorld()->getGrassManager()->getMapSize() / g_grassSectorSize / g_grassEntitiesNum) * height * core.game->getWorld()->getGrassManager()->getTileSize()) * 0.5f;
		boundingBox = BoundingBox(-diagonal, diagonal);
		boundingBox.setTransform(position);
		//gEngine.kernel->log->prnEx("%f %f %f ;;; %d %d", position.x, position.y, position.z, width, height);
	}
}

FieldEntity::~FieldEntity()
{
	//gEngine.resMgr->release(m_mesh);	m_mesh = 0;
}

void FieldEntity::update(ICamera* cam)
{
	int dump = 0;
	m_isEmpty = true;
	DWORD time = GetTickCount();
	for(int i = 0; i < m_sectorNum; i++)
	{
		if(m_sectors[i]->getTimeCounter() + g_grassSectorExistTime < time)
		{
			dump++;
			m_sectors[i]->dumpData();
		}
		if(!m_sectors[i]->isEmpty())
			m_isEmpty = false;
	}
	if(m_regenerateRenderList)
	{
		m_regenerateRenderList = false;
		m_renderList.clear();
		for(int i = 0; i < m_sectorNum; i++)
		{
			if(!m_sectors[i]->isEmpty())
				m_renderList.push_back(i);
		}
	}/*
	if(dump && dump != 256)
		gEngine.renderer->addTxt("Dumped data: %d", dump);*/
}

u32 FieldEntity::rendEnabled()
{
	return 1;
}

u32 FieldEntity::renderDeferred()
{
	int rendered = 0;
	if(!r_enabledZPrePass)
		update(gEngine.renderer->get().camera);
	if(m_isEmpty)
		return 0;
	if(D3DXVec3Length(&Vec3(boundingBox.getCenter() - *core.game->getCamera()->getPosition())) > boundingBox.getWidth() + g_grassEntityRenderDistance)
		return 0;
	
	if(core.game->getWorld()->getGrassManager()->getGrassShader())
	{
		BoundingBox box(Vec3(-g_grassSectorSize/2.0f * g_grassTileSize, -50, -g_grassSectorSize/2.0f * g_grassTileSize), Vec3(g_grassSectorSize/2.0f * g_grassTileSize, 50, g_grassSectorSize/2.0f * g_grassTileSize));
		gEngine.renderer->setShader(core.game->getWorld()->getGrassManager()->getGrassShader()->setTech("Grass2Deferred")); 

		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", core.game->getWorld()->getGrassManager()->getCropTexture());

		float angleLimit = 1.57f;
		FieldSector *actualSector = NULL;
		for(int i = 0; i < m_renderList.size(); i++)
		{
			actualSector = m_sectors[m_renderList[i]];
			Vec3 sectorCenter = actualSector->getWorldPos();
			sectorCenter.x += g_grassSectorSize * g_grassTileSize * 0.5f;
			sectorCenter.y = core.game->getCamera()->getPosition()->y;
			sectorCenter.z += g_grassSectorSize * g_grassTileSize * 0.5f;
			if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorLowRenderDistance)
				continue;
			box.setTransform(sectorCenter);
			
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumSphere()))
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumPlanes()))
			{
				Mat temp;
				D3DXMatrixTranslation(&temp, actualSector->getWorldPos().x, actualSector->getWorldPos().y, actualSector->getWorldPos().z);
				gEngine.renderer->setWorld(&temp);
				
				{
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("DispMap", actualSector->getHeightMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("GrassTypeMap", actualSector->getGrassTypeMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("cellWidth", g_grassTextureCellWidth);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("textureWidth", core.game->getWorld()->getGrassManager()->getCropTexture()->width);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("time", GetTickCount());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("sectorSize", g_grassSectorSize);

					gEngine.renderer->commitChanges();

					if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorRenderDistance)
						core.game->getWorld()->getGrassManager()->getGrassMeshLow()->render();
					else
						core.game->getWorld()->getGrassManager()->getGrassMesh()->render();
					actualSector->setTimeCounter(GetTickCount());
					rendered++;
				}
			}
		}
		return 1;
	}
	return 0;
}

u32 FieldEntity::renderForward(Light* light)
{
	int rendered = 0;
	update(gEngine.renderer->get().camera);
	if(m_isEmpty)
		return 0;
	if(D3DXVec3Length(&Vec3(boundingBox.getCenter() - *core.game->getCamera()->getPosition())) > boundingBox.getWidth() + g_grassEntityRenderDistance)
		return 0;
	
	if(core.game->getWorld()->getGrassManager()->getGrassShader())
	{
		BoundingBox box(Vec3(-g_grassSectorSize/2.0f * g_grassTileSize, -g_grassSectorSize/2.0f * g_grassTileSize, -g_grassSectorSize/2.0f * g_grassTileSize), Vec3(g_grassSectorSize/2.0f * g_grassTileSize, g_grassSectorSize/2.0f * g_grassTileSize, g_grassSectorSize/2.0f * g_grassTileSize));

		gEngine.renderer->setShader(core.game->getWorld()->getGrassManager()->getGrassShader()->setTech("Grass2"));

		Vec4 g_sunColor = *light->getColor();
		Vec3 g_sunDir = *light->getDir();

		f32 eee = D3DXVec3Dot(&Vec3(0,1,0), &Vec3(-g_sunDir));
		if(eee <0) eee = 0;
		eee += 0.2f;
		if(g_sunColor.w > 1) g_sunColor.w = 1;
		Vec4 color = g_sunColor * eee * g_sunColor.w; color.w = 1;
		core.game->getWorld()->getGrassManager()->getGrassShader()->setVec4("g_sunColor", &color);
		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", core.game->getWorld()->getGrassManager()->getCropTexture());

		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		gEngine.device->getDev()->SetRenderState(D3DRS_ZENABLE,				TRUE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ZWRITEENABLE,		TRUE);

		float angleLimit = 1.57f;
		FieldSector *actualSector = NULL;
		for(int i = 0; i < m_renderList.size(); i++)
		{
			actualSector = m_sectors[m_renderList[i]];
			Vec3 sectorCenter = actualSector->getWorldPos();
			sectorCenter.x += g_grassSectorSize * g_grassTileSize * 0.5f;
			sectorCenter.y = core.game->getCamera()->getPosition()->y;
			sectorCenter.z += g_grassSectorSize * g_grassTileSize * 0.5f;
			if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorLowRenderDistance)
				continue;

			box.setTransform(sectorCenter);
			
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumSphere()))
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumPlanes()))
			{
				Mat temp;
				D3DXMatrixTranslation(&temp, actualSector->getWorldPos().x, actualSector->getWorldPos().y, actualSector->getWorldPos().z);
				gEngine.renderer->setWorld(&temp);
				
				{
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("DispMap", actualSector->getHeightMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("GrassTypeMap", actualSector->getGrassTypeMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("cellWidth", g_grassTextureCellWidth);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("textureWidth", core.game->getWorld()->getGrassManager()->getCropTexture()->width);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("time", GetTickCount());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("sectorSize", g_grassSectorSize);

					gEngine.renderer->commitChanges();

					if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorRenderDistance)
						core.game->getWorld()->getGrassManager()->getGrassMeshLow()->render();
					else
						core.game->getWorld()->getGrassManager()->getGrassMesh()->render();
					actualSector->setTimeCounter(GetTickCount());
					rendered++;
				}
			}
		}
		return 1;
	}
	return 0;
}

u32 FieldEntity::renderRaw()
{
	if(gEngine.renderer->get().pass == REND_SM)
	{
		return 0;
	}
	else if(gEngine.renderer->get().pass == REND_UNLIT)
	{
		return 0;
	}

	int rendered = 0;
	update(gEngine.renderer->get().camera);
	if(m_isEmpty)
		return 0;
	if(D3DXVec3Length(&Vec3(boundingBox.getCenter() - *core.game->getCamera()->getPosition())) > boundingBox.getWidth() + g_grassEntityRenderDistance)
		return 0;
	if(core.game->getWorld()->getGrassManager()->getGrassShader())
	{
		BoundingBox box(Vec3(-g_grassSectorSize/2.0f * g_grassTileSize, -g_grassSectorSize/2.0f * g_grassTileSize, -g_grassSectorSize/2.0f * g_grassTileSize), Vec3(g_grassSectorSize/2.0f * g_grassTileSize, g_grassSectorSize/2.0f * g_grassTileSize, g_grassSectorSize/2.0f * g_grassTileSize));

		gEngine.renderer->setShader(core.game->getWorld()->getGrassManager()->getGrassShader()->setTech("Grass2Raw")); 

		core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("ColorMap", core.game->getWorld()->getGrassManager()->getCropTexture());

		float angleLimit = 1.57f;
		FieldSector *actualSector = NULL;
		for(int i = 0; i < m_renderList.size(); i++)
		{
			actualSector = m_sectors[m_renderList[i]];
			Vec3 sectorCenter = actualSector->getWorldPos();
			sectorCenter.x += g_grassSectorSize * g_grassTileSize * 0.5f;
			sectorCenter.y = core.game->getCamera()->getPosition()->y;
			sectorCenter.z += g_grassSectorSize * g_grassTileSize * 0.5f;
			if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorLowRenderDistance)
				continue;

			box.setTransform(sectorCenter);
			
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumSphere()))
			if(box.BSphere.intersect(core.game->getCamera()->getFrustumPlanes()))
			{
				Mat temp;
				D3DXMatrixTranslation(&temp, actualSector->getWorldPos().x, actualSector->getWorldPos().y, actualSector->getWorldPos().z);
				gEngine.renderer->setWorld(&temp);
				
				{
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("DispMap", actualSector->getHeightMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setTex("GrassTypeMap", actualSector->getGrassTypeMap());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("cellWidth", g_grassTextureCellWidth);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("textureWidth", core.game->getWorld()->getGrassManager()->getCropTexture()->width);
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("time", GetTickCount());
					core.game->getWorld()->getGrassManager()->getGrassShader()->setF32("sectorSize", g_grassSectorSize);

					gEngine.renderer->commitChanges();

					if(D3DXVec3Length(&Vec3(*core.game->getCamera()->getPosition() - sectorCenter)) >= g_grassSectorRenderDistance)
						core.game->getWorld()->getGrassManager()->getGrassMeshLow()->render();
					else
						core.game->getWorld()->getGrassManager()->getGrassMesh()->render();
					actualSector->setTimeCounter(GetTickCount());
					rendered++;
				}
			}
		}
		return 1;
	}
	return 0;
}

REND_TYPE FieldEntity::getRendType()
{ 
	return REND_DEFERRED;
}

Shader* FieldEntity::getShader()
{
	return core.game->getWorld()->getGrassManager()->getGrassShader();
}

Texture* FieldEntity::getTexture()
{
	return core.game->getWorld()->getGrassManager()->getCropTexture();
}