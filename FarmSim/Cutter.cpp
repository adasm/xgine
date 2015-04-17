#include "Cutter.h"

Cutter::Cutter(NxVec3 pos, string fname)
{
	m_cutterHeight = 0.0f;
	m_cutterRotateAngle = 0.0f;
	m_cutterON = false;
	m_cutterParams = new CutterParams;
	m_attachedTriPod = NULL;

	CfgLoader cfg(fname, m_cutterParams->getVariableList());

	//D3DXMatrixRotationYawPitchRoll(&dimm->matChassisRotation, rotateChassis.y, rotateChassis.x, rotateChassis.z);

	ObjectParams temp;
	temp.loadFromFile("Objects\\" + m_cutterParams->chassisModel);
	m_objChassis = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));
	temp.loadFromFile("Objects\\" + m_cutterParams->cutterModel);
	m_objCutter = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	Vec3 min = m_objChassis->boundingBox.Min;
	//min.y -= 1.0f;
	Vec3 max = m_objChassis->boundingBox.Max;
	//max.y += 0.5f;
	m_actionBox = new ActionBox(min, max);

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);
	core.game->getWorld()->addToWorld(m_objCutter, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	m_cutterParams->dimm = Vec3(m_objChassis->boundingBox.getWidth()/2, m_objChassis->boundingBox.getHeight()/6, m_objChassis->boundingBox.getDepth()/2);

	m_actor = core.dynamics->createBox(pos, NxVec3(m_cutterParams->dimm), m_cutterParams->density);
	SetActorCollisionGroup(m_actor, GROUP_COLLIDABLE_NON_PUSHABLE);
	NxMaterial *anisoMaterial = NULL;
	//if(!anisoMaterial)
	{
		//Create an anisotropic material
		NxMaterialDesc material; 
		//Anisotropic friction material
		material.restitution = 0.01f;
		material.staticFriction = 0.01f;
		material.dynamicFriction = 0.01f;
		material.dynamicFrictionV = 0.1f;
		material.staticFrictionV = 0.1f;
		material.dirOfAnisotropy.set(1,0,0);
		material.flags = NX_MF_ANISOTROPIC;
		anisoMaterial = core.dynamics->getScene()->createMaterial(material);
	}
	m_actor->getShapes()[0]->setMaterial(anisoMaterial->getMaterialIndex());
}

void Cutter::update()
{
	static Mat matChassis;
	m_actor->getGlobalPose().getColumnMajor44((NxF32*)&matChassis);

	if(m_cutterON)
	{
		m_cutterRotateAngle += gEngine.kernel->tmr->get() * 0.005f;
		if(m_cutterHeight > 0)
		{
			m_cutterHeight -= gEngine.kernel->tmr->get() * 0.001f;
			if(m_cutterHeight < 0)
				m_cutterHeight = 0;
		}
	}
	else
	{
		if(m_cutterHeight < 1)
		{
			m_cutterHeight += gEngine.kernel->tmr->get() * 0.001f;
			if(m_cutterHeight > 1)
				m_cutterHeight = 1;
		}
	}

	//Cutter matrix
	Mat rotate, heightModifier, cutter;
	D3DXMatrixRotationZ(&rotate, m_cutterRotateAngle);
	D3DXMatrixTranslation(&heightModifier, 0, m_cutterHeight, 0);
	cutter = rotate * matChassis * heightModifier;


	if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(matChassis._41, matChassis._42, matChassis._43));
		m_objChassis->world = matChassis;
		m_objChassis->position = Vec3(matChassis._41, matChassis._42, matChassis._43);
	}
	if(m_objCutter)
	{
		/*m_objCutter->boundingBox.setTransform(Vec3(cutter._41, cutter._42, cutter._43));
		m_objCutter->world = cutter;
		m_objCutter->position = Vec3(cutter._41, cutter._42, cutter._43);*/
		setSurfaceTransform(m_objCutter, &cutter, 0);
	}

	if(isAttached())
	{
		if(!isAttached()->isLifted())
		{
			{
				Combine *combine = (Combine*)isAttached()->getVehicle();
				if(combine->getPayloadValue() < combine->getCapacity())
				{
					m_actionBox->setTransform(&matChassis);
					CropType *type = combine->getLoadedCropType();
					int toAdd = 0;
					int cropID = -1;
					if(!type)
					{
						toAdd = core.game->getWorld()->getGrassManager()->harvestGrass(m_actionBox, m_cutterParams->cropTypeName, &cropID);
						type = core.game->getWorld()->getGrassManager()->getCropTypeInfo(cropID);
					}
					else
						toAdd = core.game->getWorld()->getGrassManager()->harvestGrass(m_actionBox, type);
					combine->addPayload(toAdd, type);
				}
				//core.game->getWorld()->getGrassManager()->changeTerrain(m_actionBox, Vec4(1, 1, 1, 1));
				//core.game->getWorld()->getGrassManager()->harvestGrass(m_actionBox, GT_WHEAT);
			}
		}
	}
}

void Cutter::attach(TriPod *tripod)
{
	m_attachedTriPod = tripod;
}

void Cutter::detach()
{
	m_attachedTriPod = NULL;
}

TriPod *Cutter::isAttached()
{
	return m_attachedTriPod;
}

Vec3 Cutter::getTriPodPos()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_cutterParams->posTriPod, &m_objChassis->world);
	return retVal;
}

void Cutter::updateInput()
{
	if(core.game->getKeyboardManager()->isAction2())
		m_cutterON = !m_cutterON;
}