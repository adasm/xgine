#pragma once
#include "XGine.h"

struct XGINE_API BoundingSphere
{
	BoundingSphere() : Radius(0) {}
	BoundingSphere(const Vec3& center, f32 radius) : Center(center), Radius(radius) { }

	inline bool intersect(const BoundingSphere* bs) const
	{
		f32 r = (f32) (Radius + bs->Radius);
		Vec3 v(bs->Center - Center);
		if(v.x > r || v.y > r || v.z > r)
			return false;
		return D3DXVec3LengthSq(&v) <= r*r;
	}

	inline bool intersect(const Plane* frustum, u32 numPlanes = 6) const
	{
		f32 d;
		for(u32 i = 0; i < numPlanes; i++)
		{
			d = frustum[i].a * Center.x + frustum[i].b * Center.y + frustum[i].c * Center.z + frustum[i].d;
			if(d <= -Radius) return false;
		}
		return true;
	}

	Vec3	Center;
	f32		Radius;
};

struct XGINE_API BoundingBox
{
	BoundingBox() { }
	BoundingBox(const Vec3& min, const Vec3& max) : Min(min), Max(max), CurrentMin(min), CurrentMax(max), BSphere( Vec3((min + max) * 0.5f), D3DXVec3Length(&(max-min)) * 0.5f ) { }
	
	inline void reset(const Vec3& min, const Vec3& max)
	{
		Min = min;
		Max = max;
		CurrentMin = min;
		CurrentMax = max;
		BSphere.Center = Vec3((min + max) * 0.5f);
		BSphere.Radius = D3DXVec3Length(&(max-min)) * 0.5f;
	}

	inline void setTransform(const Vec3& translation, f32 scalingFactor = 1)
	{
		CurrentMin = Min * scalingFactor + translation;
		CurrentMax = Max * scalingFactor + translation;
		BSphere.Radius *= scalingFactor;
		BSphere.Center = Vec3((CurrentMin + CurrentMax) * 0.5f);
	}

	inline bool contain(const BoundingBox* bb) const
	{
		if( CurrentMin.x <= bb->CurrentMin.x && CurrentMin.y <= bb->CurrentMin.y && CurrentMin.z <= bb->CurrentMin.z
			&& CurrentMax.x >= bb->CurrentMax.x && CurrentMax.y >= bb->CurrentMax.y && CurrentMax.z >= bb->CurrentMax.z )
			return true;
		return false;
	}

	inline bool intersect(const BoundingBox* bb) const
	{
		if( tmax(CurrentMax.x, bb->CurrentMax.x) - tmin(CurrentMin.x, bb->CurrentMin.x) > getWidth() + bb->getWidth()
			|| tmax(CurrentMax.z, bb->CurrentMax.z) - tmin(CurrentMin.z, bb->CurrentMin.z) > getDepth() + bb->getDepth()
			|| tmax(CurrentMax.y, bb->CurrentMax.y) - tmin(CurrentMin.y, bb->CurrentMin.y) > getHeight() + bb->getHeight() )
			return false;
		return true;
	}

	inline bool intersect(const Plane* frustum, u32 numPlanes = 6) const
	{
		u32 count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMin.x ) + frustum[i].b * ( CurrentMin.y ) + frustum[i].c * ( CurrentMin.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMin.x ) + frustum[i].b * ( CurrentMin.y ) + frustum[i].c * ( CurrentMax.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMax.x ) + frustum[i].b * ( CurrentMin.y ) + frustum[i].c * ( CurrentMax.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMax.x ) + frustum[i].b * ( CurrentMin.y ) + frustum[i].c * ( CurrentMin.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMin.x ) + frustum[i].b * ( CurrentMax.y ) + frustum[i].c * ( CurrentMin.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMin.x ) + frustum[i].b * ( CurrentMax.y ) + frustum[i].c * ( CurrentMax.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMax.x ) + frustum[i].b * ( CurrentMax.y ) + frustum[i].c * ( CurrentMax.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		count = 0;
		for(u32 i = 0; i < numPlanes; i++)
			if(frustum[i].a * ( CurrentMax.x ) + frustum[i].b * ( CurrentMax.y ) + frustum[i].c * ( CurrentMin.z ) + frustum[i].d >= 0)count++;
		if(count==6)return true;
		return false;
	}

	inline BoundingBox getOctreeSubBox(u32 i)
	{
		u32 x = i & 1;
		u32 y = (i & 2) / 2; //{0,1}
		u32 z = (i & 4) / 4;
		return BoundingBox( Vec3( CurrentMin.x + x * getWidth() / 2.0f, CurrentMin.y + y * getHeight() / 2.0f, CurrentMin.z + z * getDepth() / 2.0f ),
			Vec3( CurrentMin.x + (x+1) * getWidth() / 2.0f, CurrentMin.y + (y+1) * getHeight() / 2.0f, CurrentMin.z + (z+1) * getDepth() / 2.0f ) );
	}

	inline BoundingBox getQuadtreeSubBox(u32 i)
	{
		if(i == 0)
		{
			return BoundingBox( Vec3( CurrentMin.x, CurrentMin.y, CurrentMin.z),
			Vec3( CurrentMin.x + getWidth() / 2.0f, CurrentMax.y, CurrentMin.z + getDepth() / 2.0f ) );
		}
		else if(i == 1)
		{
			return BoundingBox( Vec3( CurrentMin.x + getWidth() / 2.0f, CurrentMin.y, CurrentMin.z),
			Vec3( CurrentMax.x, CurrentMax.y, CurrentMin.z + getDepth() / 2.0f ) );
		}
		else if(i == 2)
		{
			return BoundingBox( Vec3( CurrentMin.x, CurrentMin.y, CurrentMin.z + getDepth() / 2.0f),
			Vec3(CurrentMin.x + getWidth() / 2.0f, CurrentMax.y, CurrentMax.z ) );
		}
		else
		{
			return BoundingBox( Vec3( CurrentMin.x + getWidth() / 2.0f, CurrentMin.y, CurrentMin.z + getDepth() / 2.0f),
			Vec3( CurrentMax.x, CurrentMax.y, CurrentMax.z) );
		}
		
	}

	inline u32 getOctreeSubBoxIndex(const Vec3& pt)
	{
		u32 x = 0;
		u32 y = 0;
		u32 z = 0;
		if(pt.x >= CurrentMin.x + getWidth() / 2.0f) x = 1;
		if(pt.y >= CurrentMin.y + getHeight() / 2.0f) y = 2;
		if(pt.z >= CurrentMin.z + getDepth() / 2.0f) z = 4;
		return x | y | z;
	}

	inline u32 getQuadtreeSubBoxIndex(const Vec3& pt)
	{
		u32 x = 0;
		u32 y = 0;
		if(pt.x >= CurrentMin.x + getWidth() / 2.0f) x = 1;
		if(pt.y >= CurrentMin.y + getHeight() / 2.0f) y = 2;
		return x | y;
	}

	inline BoundingSphere*	getBoundingSphere()	{ return &BSphere; }
	inline f32				getWidth() const	{ return CurrentMax.x - CurrentMin.x; }
	inline f32				getHeight() const	{ return CurrentMax.y - CurrentMin.y; }
	inline f32				getDepth() const	{ return CurrentMax.z - CurrentMin.z; }
	inline const Vec3&		getCenter() const	{ return BSphere.Center; }

	Vec3			Min;
	Vec3			Max;
	Vec3			CurrentMin;
	Vec3			CurrentMax;
	BoundingSphere	BSphere;
};

struct XGINE_API BoundingCone
{
	BoundingCone() { }
	BoundingCone(const Vec3* vertex, const Vec3* axis, f32 angle) : Vertex(*vertex), Axis(*axis), Angle(angle), SinAngle(sinf(angle)), CosAngle(cosf(angle)) { }

	inline bool intersect(const BoundingSphere* bs) const
	{
		f32 invSin = 1.0f / SinAngle;
		f32 cosSq  = CosAngle * CosAngle;

		Vec3 V = bs->Center - Vertex;
		Vec3 D = V + (bs->Radius * invSin) * Axis;
		f32 DLenSq  = D3DXVec3LengthSq(&D);
		f32 E       = D3DXVec3Dot(&D, &Axis);

		if(E > 0.0f && E*E >= DLenSq * cosSq)
		{
			f32 sinSq = SinAngle * SinAngle;
			DLenSq      = D3DXVec3LengthSq(&V);
			E			= -D3DXVec3Dot(&V, &Axis);

			if(E > 0.0f && E*E >= DLenSq * sinSq)
				return DLenSq <= bs->Radius * bs->Radius;
			return true;
		}
		return false;
	}
	
	inline void setAxis(const Vec3* a)	 { D3DXVec3Normalize(&Axis, a); }
	inline void setVertex(const Vec3* v) { Vertex = *v; }
	inline void setAngle(f32 angle)		 { Angle = angle; SinAngle = sinf(angle); CosAngle = cosf(angle); }

	Vec3	Axis; //normalized
	Vec3	Vertex;
	f32		Angle;
	f32		SinAngle;
	f32		CosAngle;
};