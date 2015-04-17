#pragma once
#include "XGine.h"

struct XGINE_API InstanceDataPacked
{
	D3DXFLOAT16 x, y, z, s;

	InstanceDataPacked() : x(0), y(0), z(0), s(1) {}

	InstanceDataPacked(f32 X, f32 Y, f32 Z, f32 Scale)
	{
		D3DXFloat32To16Array(&x, &X, 1);
		D3DXFloat32To16Array(&y, &Y, 1);
		D3DXFloat32To16Array(&z, &Z, 1);
		D3DXFloat32To16Array(&s, &Scale, 1);
	}
};

struct XGINE_API InstanceData
{
	f32 x, y, z, s;

	InstanceData() : x(0), y(0), z(0), s(1) {}
	InstanceData(f32 X, f32 Y, f32 Z, f32 Scale) : x(X), y(Y), z(Z), s(Scale) {}
};


struct XGINE_API Surface : Node<Surface>
{
	Surface(string meshName, Material* _material,  Vec3 _position, Vec3 _rotation = Vec3(0,0,0), f32 _scale = 1, u32 _allowInstancing = 1);
	Surface(Mesh   *meshPtr, Material* _material,  Vec3 _position, Vec3 _rotation = Vec3(0,0,0), f32 _scale = 1, u32 _allowInstancing = 1);
	~Surface();

				void	updRotBB(Vec3 rot);
				void	generateLODs();

	f32					scale;
	InstanceData		instanceData;
	InstanceDataPacked	instanceDataPacked;
	Vec3				sphereCenter;
	f32					sphereRadius;
	f32					sphereMaxY;
	u32					allowInstancing;
};