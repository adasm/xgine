#include "TreeEntity.h"

TreeBunchEntity::TreeBunchEntity(TreeType *treeType, Vec3 position, Vec3 rotation, u32 hasShadow)
	: m_treeType(0), m_materialBunch(0), m_hasShadow(hasShadow), m_lod(0)
{
	m_treeType		= treeType;
	m_materialBunch = new Material;
	m_shader		= gEngine.shaderMgr->load("Tree.fx");
	m_pose.position = position;
	m_pose.rotation = rotation;

	m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
	m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;
	m_materialBunch->m_albedoMap = NULL;
	m_materialBunch->m_textureRepeat = 1.0f;
	m_materialBunch->m_textureAlbedoRepeat = 1.0f;
	m_materialBunch->m_heightMapScale = 0.1f;
	m_materialBunch->m_specularExponent = 40.0f;
	m_materialBunch->m_diffuseColor = Vec4(1, 1, 1, 1);
	m_materialBunch->m_useNormalMap = (m_treeType->bunchHiNormalMap)?(1):(0);
	m_materialBunch->m_useSpecular = 1;
	m_materialBunch->m_useParallax = 0;
	m_materialBunch->m_useAlbedo = 0;
	m_materialBunch->m_shadowReceiver = 1;
	m_materialBunch->m_splatting = 0;

	worldMat(&m_pose.world, m_pose.position, m_pose.rotation);
	D3DXMatrixInverse(&m_pose.invWorld, 0, &m_pose.world);

	if(m_treeType->branchMesh)
	{
		Vec3 maxbb = m_treeType->branchMesh->getBoundingBox().Max;
		Vec3 minbb = m_treeType->branchMesh->getBoundingBox().Min;


		if(maxbb.x < m_treeType->bunchLowMesh->getBoundingBox().Max.x)
			maxbb.x = m_treeType->bunchLowMesh->getBoundingBox().Max.x;
		if(maxbb.y < m_treeType->bunchLowMesh->getBoundingBox().Max.y)
			maxbb.y = m_treeType->bunchLowMesh->getBoundingBox().Max.y;
		if(maxbb.z < m_treeType->bunchLowMesh->getBoundingBox().Max.z)
			maxbb.z = m_treeType->bunchLowMesh->getBoundingBox().Max.z;

		if(minbb.x > m_treeType->bunchLowMesh->getBoundingBox().Min.x)
			minbb.x = m_treeType->bunchLowMesh->getBoundingBox().Min.x;
		if(minbb.y > m_treeType->bunchLowMesh->getBoundingBox().Min.y)
			minbb.y = m_treeType->bunchLowMesh->getBoundingBox().Min.y;
		if(minbb.z > m_treeType->bunchLowMesh->getBoundingBox().Min.z)
			minbb.z = m_treeType->bunchLowMesh->getBoundingBox().Min.z;

		if(maxbb.x < m_treeType->bunchHiMesh->getBoundingBox().Max.x)
			maxbb.x = m_treeType->bunchHiMesh->getBoundingBox().Max.x;
		if(maxbb.y < m_treeType->bunchHiMesh->getBoundingBox().Max.y)
			maxbb.y = m_treeType->bunchHiMesh->getBoundingBox().Max.y;
		if(maxbb.z < m_treeType->bunchHiMesh->getBoundingBox().Max.z)
			maxbb.z = m_treeType->bunchHiMesh->getBoundingBox().Max.z;

		if(minbb.x > m_treeType->bunchHiMesh->getBoundingBox().Min.x)
			minbb.x = m_treeType->bunchHiMesh->getBoundingBox().Min.x;
		if(minbb.y > m_treeType->bunchHiMesh->getBoundingBox().Min.y)
			minbb.y = m_treeType->bunchHiMesh->getBoundingBox().Min.y;
		if(minbb.z > m_treeType->bunchHiMesh->getBoundingBox().Min.z)
			minbb.z = m_treeType->bunchHiMesh->getBoundingBox().Min.z;



		m_pose.boundingBox.reset(minbb, maxbb);
		m_pose.boundingBox.setTransform(m_pose.position);

		minbb = m_treeType->bunchHiMesh->getBoundingBox().Min;
		maxbb = m_treeType->bunchHiMesh->getBoundingBox().Max;

		Vec3 r = (maxbb-minbb)/2;
		m_bunchRadius = D3DXVec3Length(&r);
		m_bunchCenter = minbb + r;
		m_bunchMaxY = maxbb.y;
	}
}

TreeBunchEntity::~TreeBunchEntity()
{
}

void TreeBunchEntity::update(ICamera* cam)
{
}

Texture* TreeBunchEntity::getTexture()
{
	if(m_lod == 0)
	{
		return m_treeType->bunchHiTexture;
	}
	else if(m_lod == 1)
	{
		return m_treeType->bunchLowTexture;
	}
	else if(m_lod == 2)
	{
		return 0;
	}
	else return 0;
}

u32 TreeBunchEntity::rendEnabled()
{
	ICamera* cam = gEngine.renderer->get().camera;
	{
		/*m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
		m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;*/

		if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderHiDetailsDistance)
		{
			m_lod = 0;
			m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;
		}
		/*else if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderMedDetailsDistance)
		{
			m_lod = 1;
			m_materialBunch->m_colorMap = m_treeType->bunchLowTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchLowNormalMap;
		}
		else if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderLowDetailsDistance)
		{
			m_lod = 2;
		}*/
		else
			m_lod = 1000;

	}

	u32 enabled = ( m_lod == 0 );
	m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
	m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;
	return enabled;
}



u32 TreeBunchEntity::renderDeferred()
{	
	if(m_lod == 1000)
		return 0;
	if(m_materialBunch)
	{
		if(m_lod == 0)
		{
			m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;

			gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferred"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchHiMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			
		}
		else if(m_lod == 1)
		{
			m_materialBunch->m_colorMap = m_treeType->bunchLowTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchLowNormalMap;

			gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferred"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchLowMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
		else
		{

		}
		return 1;
	}
	return 0;
}

u32 TreeBunchEntity::renderForward(Light* light)
{
	if(m_lod == 1000)
		return 0;
	if(m_materialBunch)
	{
		if(m_lod == 0)
		{
			//BUNCH
			m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;

			gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlpha"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchHiMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		
		}
		else if(m_lod == 1)
		{
			//BUNCH
			m_materialBunch->m_colorMap = m_treeType->bunchLowTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchLowNormalMap;

			gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlpha"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchLowMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

			
		}
		else
		{

		}

		return 1;
	}
	return 0;
}

u32 TreeBunchEntity::renderRaw()
{
	if(m_lod == 1000)
		return 0;
	if(m_materialBunch)
	{
		if(m_lod == 0)
		{
			m_materialBunch->m_colorMap = m_treeType->bunchHiTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchHiNormalMap;
			
			if(gEngine.renderer->get().pass == REND_SM)
			{
				if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
					gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredVSMZFILL"));
				else
					gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredSMZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			else 
			{
				gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			}
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchHiMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);		
		}
		else if(m_lod == 1)
		{
			m_materialBunch->m_colorMap = m_treeType->bunchLowTexture;
			m_materialBunch->m_normalMap = m_treeType->bunchLowNormalMap;

			if(gEngine.renderer->get().pass == REND_SM)
			{
				if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
					gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredVSMZFILL"));
				else
					gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredSMZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			else 
			{
				gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
			}
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
			m_shader->setBool("g_instancingEnabled", false);
			m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
			m_shader->setF32("g_bunchRadius", m_bunchRadius);
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBunch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->bunchLowMesh->render();
			gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

		}
		else
		{

		}
		return 1;
	}
	return 0;
}

REND_SHADER_TYPE TreeBunchEntity::getShaderType() 
{ 
	return SHADER_CUSTOM; 
}

u32 TreeBunchEntity::setupCustomShader(FuncCustomShader func)
{
	if(func == FUNC_FORWARD)
	{
		gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlpha"));
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
		m_shader->setF32("g_bunchRadius", m_bunchRadius);
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBunch);
	}
	else if(func == FUNC_DEFERRED)
	{
		gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferred"));
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		gEngine.device->getDev()->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
		m_shader->setF32("g_bunchRadius", m_bunchRadius);
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBunch);
	}
	else if(func == FUNC_RAW)
	{
		if(gEngine.renderer->get().pass == REND_SM)
		{
			if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
				gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredVSMZFILL"));
			else
				gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredSMZFILL"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
		else 
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeForwardAlphaDeferredZFILL"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAREF, (DWORD)0x00000066);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
		}
		m_shader->setVec3("g_bunchCenter", &m_bunchCenter);
		m_shader->setF32("g_bunchRadius", m_bunchRadius);
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBunch);
	}
	else return 0;

	return 1;
}

u32	TreeBunchEntity::renderSV(Light* light)
{
	return 0;
}

REND_TYPE TreeBunchEntity::getRendType()
{ 
	return REND_DEFERRED; 
}

Mesh* TreeBunchEntity::getMesh()
{
	if(m_lod == 0)
		return m_treeType->bunchHiMesh;
	else if(m_lod == 1)
		return m_treeType->bunchLowMesh;

	return NULL;
}

Shader* TreeBunchEntity::getShader()
{
	return m_shader + 1;
}


////////////////////////////////////////BRANCH

#include "TreeEntity.h"

TreeBranchEntity::TreeBranchEntity(TreeType *treeType, Vec3 position, Vec3 rotation, u32 hasShadow)
	: m_treeType(0), m_materialBranch(0), m_hasShadow(hasShadow), m_lod(0), m_bunchMaxY(20)
{
	m_treeType		= treeType;
	m_materialBranch= new Material;
	m_shader		= gEngine.shaderMgr->load("Tree.fx");
	m_pose.position = position;
	m_pose.rotation = rotation;

	m_materialBranch->m_colorMap = m_treeType->branchTexture;
	m_materialBranch->m_normalMap = m_treeType->branchNormalMap;
	m_materialBranch->m_albedoMap = NULL;
	m_materialBranch->m_textureRepeat = 15.0f;
	m_materialBranch->m_textureAlbedoRepeat = 4.0f;
	m_materialBranch->m_heightMapScale = 0.1f;
	m_materialBranch->m_specularExponent = 40.0f;
	m_materialBranch->m_diffuseColor = Vec4(1, 1, 1, 1);
	m_materialBranch->m_useNormalMap = (m_treeType->branchNormalMap)?(1):(0);;
	m_materialBranch->m_useSpecular = 1;
	m_materialBranch->m_useParallax = 1;
	m_materialBranch->m_useAlbedo = 0;
	m_materialBranch->m_shadowReceiver = 1;
	m_materialBranch->m_splatting = 0;

	worldMat(&m_pose.world, m_pose.position, m_pose.rotation);
	D3DXMatrixInverse(&m_pose.invWorld, 0, &m_pose.world);

	if(m_treeType->branchMesh)
	{
		Vec3 maxbb = m_treeType->branchMesh->getBoundingBox().Max;
		Vec3 minbb = m_treeType->branchMesh->getBoundingBox().Min;


		if(maxbb.x < m_treeType->bunchLowMesh->getBoundingBox().Max.x)
			maxbb.x = m_treeType->bunchLowMesh->getBoundingBox().Max.x;
		if(maxbb.y < m_treeType->bunchLowMesh->getBoundingBox().Max.y)
			maxbb.y = m_treeType->bunchLowMesh->getBoundingBox().Max.y;
		if(maxbb.z < m_treeType->bunchLowMesh->getBoundingBox().Max.z)
			maxbb.z = m_treeType->bunchLowMesh->getBoundingBox().Max.z;

		if(minbb.x > m_treeType->bunchLowMesh->getBoundingBox().Min.x)
			minbb.x = m_treeType->bunchLowMesh->getBoundingBox().Min.x;
		if(minbb.y > m_treeType->bunchLowMesh->getBoundingBox().Min.y)
			minbb.y = m_treeType->bunchLowMesh->getBoundingBox().Min.y;
		if(minbb.z > m_treeType->bunchLowMesh->getBoundingBox().Min.z)
			minbb.z = m_treeType->bunchLowMesh->getBoundingBox().Min.z;



		if(maxbb.x < m_treeType->bunchHiMesh->getBoundingBox().Max.x)
			maxbb.x = m_treeType->bunchHiMesh->getBoundingBox().Max.x;
		if(maxbb.y < m_treeType->bunchHiMesh->getBoundingBox().Max.y)
			maxbb.y = m_treeType->bunchHiMesh->getBoundingBox().Max.y;
		if(maxbb.z < m_treeType->bunchHiMesh->getBoundingBox().Max.z)
			maxbb.z = m_treeType->bunchHiMesh->getBoundingBox().Max.z;

		if(minbb.x > m_treeType->bunchHiMesh->getBoundingBox().Min.x)
			minbb.x = m_treeType->bunchHiMesh->getBoundingBox().Min.x;
		if(minbb.y > m_treeType->bunchHiMesh->getBoundingBox().Min.y)
			minbb.y = m_treeType->bunchHiMesh->getBoundingBox().Min.y;
		if(minbb.z > m_treeType->bunchHiMesh->getBoundingBox().Min.z)
			minbb.z = m_treeType->bunchHiMesh->getBoundingBox().Min.z;



		m_pose.boundingBox.reset(minbb, maxbb);
		m_pose.boundingBox.setTransform(m_pose.position);
	}
}

TreeBranchEntity::~TreeBranchEntity()
{
}

void TreeBranchEntity::update(ICamera* cam)
{
}

Texture* TreeBranchEntity::getTexture()
{
	if(m_lod == 0)
	{
		return m_treeType->branchTexture;
	}
	else if(m_lod == 1)
	{
		return m_treeType->branchTexture;
	}
	else if(m_lod == 2)
	{
		return m_treeType->treeLowTexture;
	}
	else return 0;
}

u32 TreeBranchEntity::rendEnabled()
{
	ICamera* cam = gEngine.renderer->get().camera;
	{
		if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderHiDetailsDistance)
		{
			m_materialBranch->m_colorMap = m_treeType->branchTexture;
			m_materialBranch->m_normalMap = m_treeType->branchNormalMap;
			m_lod = 0;
		}
		/*else if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderMedDetailsDistance)
		{
			m_materialBranch->m_colorMap = m_treeType->branchTexture;
			m_materialBranch->m_normalMap = m_treeType->branchNormalMap;
			m_lod = 1;
		}
		else if(D3DXVec3Length(&Vec3(m_pose.boundingBox.getCenter() - *cam->getPosition())) < g_treeRenderLowDetailsDistance)
		{
			m_materialBranch->m_colorMap = m_treeType->treeLowTexture;
			m_materialBranch->m_normalMap = m_treeType->treeLowNormalMap;
			m_lod = 2;
		}*/
		else
			m_lod = 1000;
	}

	u32 enabled = ( m_lod == 0 );
	return enabled;
}

u32 TreeBranchEntity::renderDeferred()
{
	if(m_lod == 1000)
		return 0;
	if(m_materialBranch)
	{
		if(m_lod == 0)
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeDeferred"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
		}
		else if(m_lod == 1)
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeDeferred"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
		}
		else
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeDeferred"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->treeLowMesh->render();			
		}
		return 1;
	}
	return 0;
}

u32 TreeBranchEntity::renderForward(Light* light)
{
	if(m_lod == 1000)
		return 0;
	if(m_materialBranch)
	{
		if(m_lod == 0)
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeForward"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
			
		}
		else if(m_lod == 1)
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeForward"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
		}
		else
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeForward"));
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setupLight(light);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->treeLowMesh->render();
		}
		return 1;
	}
	return 0;
}

u32 TreeBranchEntity::renderRaw()
{
	if(m_lod == 1000)
		return 0;
	if(m_materialBranch)
	{
		if(m_lod == 0)
		{
			if(gEngine.renderer->get().pass == REND_SM)
			{
				if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredVSMZFILL"));
				else
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredSMZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			else 
			{
				gEngine.renderer->setShader(m_shader->setTech("TreeDeferredRaw"));
			}
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
			
		}
		else if(m_lod == 1)
		{
			if(gEngine.renderer->get().pass == REND_SM)
			{
				if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredVSMZFILL"));
				else
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredSMZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			else 
			{
				gEngine.renderer->setShader(m_shader->setTech("TreeDeferredRaw"));
			}
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->branchMesh->render();
		}
		else
		{
			if(gEngine.renderer->get().pass == REND_SM)
			{
				if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredVSMZFILL"));
				else
					gEngine.renderer->setShader(m_shader->setTech("TreeDeferredSMZFILL"));
				gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
			}
			else 
			{
				gEngine.renderer->setShader(m_shader->setTech("TreeDeferredRaw"));
			}
			m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
			m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
			gEngine.renderer->setMaterial(m_materialBranch);
			gEngine.renderer->setWorld(&m_pose.world);
			gEngine.renderer->commitChanges();
			m_treeType->treeLowMesh->render();
		}
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		gEngine.device->getDev()->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		return 1;
	}
	return 0;
}

u32	TreeBranchEntity::renderSV(Light* light)
{
	if(m_treeType->branchMesh && light)
	{
		gEngine.renderer->setDefaultShader();
		gEngine.renderer->setupLight(light);
		gEngine.renderer->setWorld(&m_pose.world);
		gEngine.renderer->commitChanges();
		m_treeType->branchMesh->renderSV();
		return 1;
	}
	return 0;
}

REND_TYPE TreeBranchEntity::getRendType()
{ 
	return REND_DEFERRED; 
}

Mesh* TreeBranchEntity::getMesh()
{
	if(m_lod == 0)
	{
		return m_treeType->branchMesh;
	}
	else if(m_lod == 1)
	{
		return m_treeType->branchMesh;
	}
	else
	{
		return m_treeType->treeLowMesh;
	}
}

Shader* TreeBranchEntity::getShader()
{
	return m_shader;
}

REND_SHADER_TYPE TreeBranchEntity::getShaderType() 
{ 
	return SHADER_CUSTOM; 
}

u32 TreeBranchEntity::setupCustomShader(FuncCustomShader func)
{
	if(func == FUNC_FORWARD)
	{
		gEngine.renderer->setShader(m_shader->setTech("TreeForward"));
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBranch);
	}
	else if(func == FUNC_DEFERRED)
	{
		gEngine.renderer->setShader(m_shader->setTech("TreeDeferred"));
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBranch);
	}
	else if(func == FUNC_RAW)
	{
		if(gEngine.renderer->get().pass == REND_SM)
		{
			if(r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)
				gEngine.renderer->setShader(m_shader->setTech("TreeDeferredVSMZFILL"));
			else
				gEngine.renderer->setShader(m_shader->setTech("TreeDeferredSMZFILL"));
			gEngine.device->getDev()->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		}
		else 
		{
			gEngine.renderer->setShader(m_shader->setTech("TreeDeferredRaw"));
		}
		m_shader->setF32("g_bunchMaxY", m_bunchMaxY);
		m_shader->setF32("g_time", gEngine.kernel->tmr->getFrameTime());
		gEngine.renderer->setMaterial(m_materialBranch);
	}
	else return 0;

	return 1;
}