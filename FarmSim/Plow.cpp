#include "Plow.h"

Plow::Plow(NxVec3 pos, string fname)
{
	m_plowDimms = new PlowDimms;
	m_attachedTriPod = NULL;

	string chassisModel;
	m_plowDimms->getVariableList()->push_back(new CfgVariable(V_STRING, &chassisModel, getVarName(chassisModel)));

	CfgLoader cfg(fname, m_plowDimms->getVariableList());

	//D3DXMatrixRotationYawPitchRoll(&dimm->matChassisRotation, rotateChassis.y, rotateChassis.x, rotateChassis.z);

	ObjectParams temp;
	temp.loadFromFile("Objects\\" + chassisModel);
	m_objChassis = new Surface(temp.meshName, temp.generateMaterial(), Vec3(0, 0, 0));

	Vec3 min = m_objChassis->boundingBox.Min;
	min.y -= 0.5f;
	Vec3 max = m_objChassis->boundingBox.Max;
	//max.y += 0.5f;
	/*min = Vec3(-5, -5, -5);
	max = Vec3(5, 5, 5);*/
	m_actionBox = new ActionBox(min, max);

	core.game->getWorld()->addToWorld(m_objChassis, NO_COLLISION, 0, GROUP_NON_COLLIDABLE, NULL, 1);

	m_plowDimms->dimm = Vec3(m_objChassis->boundingBox.getWidth()/2, m_objChassis->boundingBox.getHeight()/6, m_objChassis->boundingBox.getDepth()/2);

	m_actor = core.dynamics->createBox(pos, NxVec3(m_plowDimms->dimm), m_plowDimms->density);
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

void Plow::update()
{
	static Mat matChassis;
	m_actor->getGlobalPose().getColumnMajor44((NxF32*)&matChassis);

	if(m_objChassis)
	{
		m_objChassis->boundingBox.setTransform(Vec3(matChassis._41, matChassis._42, matChassis._43));
		m_objChassis->world = matChassis;
		m_objChassis->position = Vec3(matChassis._41, matChassis._42, matChassis._43);
	}
	if(isAttached())
	{
		if(!isAttached()->isLifted())
		{
			{
				m_actionBox->setTransform(&matChassis);
				core.game->getWorld()->getGrassManager()->changeTerrain(m_actionBox, Vec4(1, 1, 1, 1));
				//core.game->getWorld()->getGrassManager()->harvestGrass(m_actionBox, GT_WHEAT);
				core.game->getWorld()->getGrassManager()->destroyGrass(m_actionBox);
			}
		}
	}
}

void Plow::attach(TriPod *tripod)
{
	m_attachedTriPod = tripod;
}

void Plow::detach()
{
	m_attachedTriPod = NULL;
}

TriPod *Plow::isAttached()
{
	return m_attachedTriPod;
}

Vec3 Plow::getTriPodPos()
{
	Vec3 retVal;
	D3DXVec3TransformCoord(&retVal, &m_plowDimms->posTriPod, &m_objChassis->world);
	return retVal;
}

void Plow::updateInput()
{
}