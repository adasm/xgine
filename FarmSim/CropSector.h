#pragma once
#include "Headers.h"


class CropSector
{
public:
	CropSector(int cropID, Vec3 WorldPos, int posX, int posY, GrassMesh* youngSector, GrassMesh* adultSector, GrassMesh* oldSector, GrassMesh* deadSector);

	void				harvest(ActionBox* abox);

	void				generatePositions();
	void				update(bool young = true, bool adult = true, bool old = true, bool dead = true); //reassigns all positions in meshes
	void				render();

	void				deleteMesh();
	void				recreateMesh();

	void				newTurn();
protected:
	int					m_cropID;
	int					m_posInMapX;
	int					m_posInMapY;

	Vec3				m_worldPos;

	Vec3*				m_youngPos;
	Vec3*				m_adultPos;
	Vec3*				m_oldPos;
	Vec3*				m_deadPos;

	int					m_youngNum;
	int					m_adultNum;
	int					m_oldNum;
	int					m_deadNum;

	GrassMesh*			m_young;
	GrassMesh*			m_adult;
	GrassMesh*			m_old;
	GrassMesh*			m_dead;

	CropType*			m_cropTypeInfo;

	bool				m_toUpdate;
};