#pragma once
#include "Headers.h"

class Game
{
public:
	void				init();
	void				close();

	void				update();
	void				proceed();

	BoundingBox*		getCameraBB();
	FreeCamera*			getCamera() { return m_camera; }
	World*				getWorld() { return m_world; }
	KeyboardManager*	getKeyboardManager() { return m_keyboardManager; }

protected:
	GUIMenu				m_menu;
	FreeCamera*			m_camera;
	World*				m_world;
	KeyboardManager*	m_keyboardManager;
};