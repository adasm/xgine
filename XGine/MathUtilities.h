#pragma once
#include "XGine.h"

inline void worldMat(Mat *mat, Vec3 pos, Vec3 rot, f32 scale)
{
	static Mat trans, rotate, scaling, scaledRotated;
	D3DXMatrixScaling(&scaling, scale, scale, scale);
	D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
	D3DXMatrixRotationYawPitchRoll(&rotate, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(&scaledRotated, &scaling, &rotate);
	D3DXMatrixMultiply(mat, &scaledRotated, &trans);
}

inline void worldMat(Mat *mat, Vec3 pos, Vec3 rot)
{
	static Mat trans, rotate;
	D3DXMatrixTranslation(&trans, pos.x, pos.y, pos.z);
	D3DXMatrixRotationYawPitchRoll(&rotate, rot.y, rot.x, rot.z);
	D3DXMatrixMultiply(mat, &rotate, &trans);
}

inline void worldMat(Mat *mat, Vec3 pos)
{
	D3DXMatrixTranslation(mat, pos.x, pos.y, pos.z);
}

inline void worldInvMat(Mat *mat, Mat *inv, Vec3 pos)
{
	D3DXMatrixTranslation(mat, pos.x, pos.y, pos.z);
	D3DXMatrixInverse(inv, 0, mat);
}

void buildBoundingBoxLines(Vec3 *min, Vec3 *max, Vec3 *positions/*0-27*/);