#include "ActionBox.h"

ActionBox::ActionBox(Vec3 min, Vec3 max)
{
	m_position = Vec3(0, 0, 0);
	m_min = m_currentMin = min;
	m_max = m_currentMax = max;
	m_width = m_max.x - m_min.x;
	m_height = m_max.y - m_min.y;
	m_depth = m_max.z - m_max.z;

	D3DXMatrixIdentity(&m_world);
	if(m_width < 0)
		m_width *= -1;
	if(m_height < 0)
		m_height *= -1;
	if(m_depth < 0)
		m_depth *= -1;
	m_boundingBox = new BoundingBox(m_min, m_max);
}

void ActionBox::setTransform(Mat *world)
{
	m_world = *world;
	m_position = Vec3(m_world._41, m_world._42, m_world._43);
	/*m_world._41 = 0;
	m_world._42 = 0;
	m_world._43 = 0;*/
	D3DXVec3TransformCoord(&m_currentMin, &m_min, &m_world);
	D3DXVec3TransformCoord(&m_currentMax, &m_max, &m_world);
	/*m_currentMin += m_position;
	m_currentMax += m_position;*/
}

bool ActionBox::isPointInside(Vec3 *point)
{
	/*if(!(point->y < m_currentMax.y && point->y > m_currentMin.y))
		return false;*/

	/*point->y = 100.0f;
	Vec3 p0 = m_min;
	Vec3 p1 = m_max;
	Vec3 p2 = Vec3(m_max.x, 0, m_min.z);
	Vec3 p2_2 = Vec3(m_min.x, 0, m_max.z);

	D3DXVec3TransformCoord(&p0, &p0, &m_world);
	D3DXVec3TransformCoord(&p1, &p1, &m_world);
	D3DXVec3TransformCoord(&p2, &p2, &m_world);
	D3DXVec3TransformCoord(&p2_2, &p2_2, &m_world);

	return D3DXIntersectTri(&p0, &p1, &p2, point, &Vec3(0, -1, 0), 0, 0, 0) || D3DXIntersectTri(&p0, &p1, &p2_2, point, &Vec3(0, -1, 0), 0, 0, 0);*/
	Vec3 checkPoint = *point;
	checkPoint = (*point) - m_position;
	Mat temp;
	temp = m_world;
	D3DXQUATERNION quat;
	D3DXQuaternionRotationMatrix(&quat, &temp);
	D3DXQuaternionNormalize(&quat, &quat);
	D3DXQuaternionInverse(&quat, &quat);
	temp._41 = 0;
	temp._42 = 0;
	temp._43 = 0;
	
	D3DXMatrixRotationQuaternion(&temp, &quat);
	D3DXVec3TransformCoord(&checkPoint, &checkPoint, &temp);
	if(checkPoint.x > m_min.x && checkPoint.x < m_max.x &&
		//checkPoint.y > m_min.y && checkPoint.y < m_max.y &&
		checkPoint.z > m_min.z && checkPoint.z < m_max.z)
		return true;

	return false;
}

BoundingBox* ActionBox::getBoundingBox()
{
	static BoundingBox *retVal = NULL;
	
	Vec3 bbmin = Vec3(9999999999, 99999999999, 99999999999);
	Vec3 bbmax = Vec3(-9999999999, -9999999999, -9999999999);
	Vec3 min1 = m_min;
	Vec3 max1 = m_max;
	Vec3 min2 = m_min;
	Vec3 max2 = m_max;
	min2.z = max1.z;
	min2.y = max1.y;
	max2.z = min1.z;
	max2.y = min1.y;

	Mat tempMat = m_world;
	/*tempMat._41 = 0;
	tempMat._42 = 0;
	tempMat._43 = 0;*/
	D3DXVec3TransformCoord(&min1, &min1, &tempMat);
	D3DXVec3TransformCoord(&min2, &min2, &tempMat);
	D3DXVec3TransformCoord(&max1, &max1, &tempMat);
	D3DXVec3TransformCoord(&max2, &max2, &tempMat);
	/*min1 += m_position;
	min2 += m_position;
	max1 += m_position;
	max2 += m_position;*/
	D3DXVec3Minimize(&bbmin, &bbmin, &min1);
	D3DXVec3Minimize(&bbmin, &bbmin, &min2);
	D3DXVec3Minimize(&bbmin, &bbmin, &max1);
	D3DXVec3Minimize(&bbmin, &bbmin, &max2);

	D3DXVec3Maximize(&bbmax, &bbmax, &min1);
	D3DXVec3Maximize(&bbmax, &bbmax, &min2);
	D3DXVec3Maximize(&bbmax, &bbmax, &max1);
	D3DXVec3Maximize(&bbmax, &bbmax, &max2);
	/*if(bbmin.x > min1.x)
		bbmin.x = min1.x;
	if(bbmin.x > min2.x)
		bbmin.x = min2.x;

	if(bbmin.y > min1.y)
		bbmin.y = min1.y;
	if(bbmin.y > min2.y)
		bbmin.y = min2.y;

	if(bbmin.z > min1.z)
		bbmin.z = min1.z;
	if(bbmin.z > min2.z)
		bbmin.z = min2.z;



	if(bbmax.x < max1.x)
		bbmax.x = max1.x;
	if(bbmax.x < max2.x)
		bbmax.x = max2.x;

	if(bbmax.y < max1.y)
		bbmax.y = max1.y;
	if(bbmax.y < max2.y)
		bbmax.y = max2.y;

	if(bbmax.z < max1.z)
		bbmax.z = max1.z;
	if(bbmax.z < max2.z)
		bbmax.z = max2.z;*/

	/*
	Vec3 p0 = m_min;
	Vec3 p1 = m_max;
	Vec3 p2 = Vec3(m_max.x, 0, m_min.z);
	Vec3 p2_2 = Vec3(m_min.x, 0, m_max.z);
	Vec3 min1, max1;
	min1 = m_min;
	max1 = m_max;
	min1.y = m_max.y;

	D3DXVec3TransformCoord(&p0, &p0, &m_world);
	D3DXVec3TransformCoord(&p1, &p1, &m_world);
	D3DXVec3TransformCoord(&p2, &p2, &m_world);
	D3DXVec3TransformCoord(&p2_2, &p2_2, &m_world);

	Vec3 tempMin = p0;
	Vec3 tempMax = p0;

	if(tempMin.x > p1.x)
		tempMin.x = p1.x;
	if(tempMin.x > p2.x)
		tempMin.x = p2.x;
	if(tempMin.x > p2_2.x)
		tempMin.x = p2_2.x;

	if(tempMin.z > p1.z)
		tempMin.z = p1.z;
	if(tempMin.z > p2.z)
		tempMin.z = p2.z;
	if(tempMin.z > p2_2.z)
		tempMin.z = p2_2.z;




	if(tempMax.x < p1.x)
		tempMax.x = p1.x;
	if(tempMax.x < p2.x)
		tempMax.x = p2.x;
	if(tempMax.x < p2_2.x)
		tempMax.x = p2_2.x;

	if(tempMax.z < p1.z)
		tempMax.z = p1.z;
	if(tempMax.z < p2.z)
		tempMax.z = p2.z;
	if(tempMax.z < p2_2.z)
		tempMax.z = p2_2.z;

*/
	m_boundingBox->reset(bbmin, bbmax);
	return m_boundingBox;
}