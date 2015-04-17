#include "Headers.h"

Core core;


float g_grassEntityRenderDistance = 150;	//Whole entity distance rendering
float g_grassSectorRenderDistance = 60;		//Sector distance rendering
float g_grassSectorLowRenderDistance = 100;	//LowPoly sector distance rendering
float g_grassTileSize = 1.0f;				//Size of one tile
float g_grassTextureCellWidth = 128;		//Width of one cell in Texture with crop Types;
int g_grassSectorSize = 32;					//Size of sector in tiles
int g_grassEntitiesNum = 16;				//Number of entities in whole world
int g_grassSectorExistTime = 1000;

float g_treeRenderHiDetailsDistance = 50.0f;	//Distance in which is rendered the best quality of trees
float g_treeRenderMedDetailsDistance = 100.0f;	//Distance in which is rendered good quality of trees
float g_treeRenderLowDetailsDistance = 200.0f;	//Distance in which is rendered bad quality of trees

string g_appCache = "data/farmSimCache.xpak";

Vec3 getVec3FromString(string vec)
{
	Vec3 retVal;
	std::stringstream conv;
	conv << vec;
	conv >> retVal.x >> retVal.y >> retVal.z;

	return retVal;
}

Vec4 getVec4FromString(string vec)
{
	Vec4 retVal;
	std::stringstream conv;
	conv << vec;
	conv >> retVal.x >> retVal.y >> retVal.z >> retVal.w;

	return retVal;
}

int	getIntFromString(string entry)
{
	int retVal;
	std::stringstream conv;
	conv << entry;
	conv >> retVal;

	return retVal;
}

float getFloatFromString(string entry)
{
	float retVal;
	std::stringstream conv;
	conv << entry;
	conv >> retVal;

	return retVal;
}


void setSurfaceTransform(Surface* entity, Vec3 pos, Vec3 rot, bool rotateBB)
{
	entity->position = pos;
	entity->rotation = rot;
	//worldMat(&entity->world, entity->position, entity->rotation);
	Mat rotate;
	D3DXMatrixRotationYawPitchRoll(&rotate, rot.y, rot.x, rot.z);
	D3DXMatrixTranslation(&entity->world, entity->position.x, entity->position.y, entity->position.z);
	entity->world = rotate * entity->world;
	D3DXMatrixInverse(&entity->invWorld, 0, &entity->world);
	if(rotateBB)
		entity->updRotBB(rot);
	else
		entity->boundingBox.setTransform(pos);
}

void setSurfaceTransform(Surface* entity, Mat *mat, bool rotateBB)
{
	Vec3 v1, v2;
	D3DXQUATERNION quat;
	D3DXMatrixDecompose(&v1, &quat, &v2, mat);
	D3DXQuaternionNormalize(&quat, &quat);
	QuatToEuler(&quat, &entity->rotation);
	entity->position = Vec3(mat->_41, mat->_42, mat->_43);
	entity->world = *mat;
	if(rotateBB)
		entity->updRotBB(entity->rotation);
	else
		entity->boundingBox.setTransform(entity->position);
}

void QuatToEuler(const D3DXQUATERNION *quat, Vec3 *rot)
{
	double sqw;
	double sqx;
	double sqy;
	double sqz;

	sqw = quat->w * quat->w;
	sqx = quat->x * quat->x;
	sqy = quat->y * quat->y;
	sqz = quat->z * quat->z;

	rot->x = atan2l(2.0 * ( quat->y * quat->z + quat->x * quat->w ) , ( -sqx - sqy + sqz + sqw ));
	rot->y = asinl(-2.0 * ( quat->x * quat->z - quat->y * quat->w ));
	rot->z = atan2l(2.0 * ( quat->x * quat->y + quat->z * quat->w ) , (  sqx - sqy - sqz + sqw ));

	return;
}

Vec2 getCoordinatesFromIndex(int index, float width)
{
	Vec2 retVal;
	retVal.y = index / width;
	retVal.y = floor(retVal.y);
	retVal.x = index - retVal.y * width;
	return retVal;
}

Vec3 getVec3MaxParameter(Vec3 *vec)
{
	Vec3 retVal = Vec3(0, 0, 0);
	float max = vec->x;
	retVal = Vec3(max, 0, 0);
	if(max < vec->y)
	{
		max = vec->y;
		retVal = Vec3(0, max, 0);
	}
	if(max < vec->z)
	{
		max = vec->z;
		retVal = Vec3(0, 0, max);
	}
	return retVal;
}

Vec3 getVec3MinParameter(Vec3 *vec)
{
	Vec3 retVal = Vec3(0, 0, 0);
	float min = vec->x;
	retVal = Vec3(min, 0, 0);
	if(min > vec->y)
	{
		min = vec->y;
		retVal = Vec3(0, min, 0);
	}
	if(min > vec->z)
	{
		min = vec->z;
		retVal = Vec3(0, 0, min);
	}
	return retVal;
}