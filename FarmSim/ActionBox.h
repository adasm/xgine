#pragma once
#include "Headers.h"


class ActionBox
{
public:
	ActionBox(Vec3 min, Vec3 max);
	void			setTransform(Mat *world);
	bool			isPointInside(Vec3* point);
	Vec3			getMin() { return m_min; }
	Vec3			getMax() { return m_max; }
	Vec3			getCurrentMin() { return m_currentMin; }
	Vec3			getCurrentMax() { return m_currentMax; }
	BoundingBox*	getBoundingBox();
protected:
	BoundingBox*	m_boundingBox;
	Vec3			m_position;
	Vec3			m_min;
	Vec3			m_max;
	Vec3			m_currentMin;
	Vec3			m_currentMax;
	float			m_width;
	float			m_height;
	float			m_depth;
	Mat				m_world;
};
