#pragma once
#include "Headers.h"

class KeyboardManager
{
public:
	KeyboardManager();
	~KeyboardManager();

	void	update();


	bool	isForward()				{ return m_forward; };
	bool	isBackward()			{ return m_backward; };
	bool	isLeft()				{ return m_left; };
	bool	isRight()				{ return m_right; };

	bool	isEnterVehicle()		{ return m_enterVehicle; };
	bool	isAttachToVehicle()		{ return m_attachToVehicle; };
	bool	isLiftDevice()			{ return m_liftDevice; };

	bool	isAction1()				{ return m_action1; };
	bool	isAction2()				{ return m_action2; };

	bool	isChangeCamera()		{ return m_changeCamera; };
protected:

	bool	m_forward;
	bool	m_backward;
	bool	m_left;
	bool	m_right;

	bool	m_enterVehicle;
	bool	m_attachToVehicle;
	bool	m_liftDevice;

	bool	m_action1;
	bool	m_action2;
	bool	m_changeCamera;



	DWORD	m_forwardKeycode;
	DWORD	m_backwardKeycode;
	DWORD	m_leftKeycode;
	DWORD	m_rightKeycode;

	DWORD	m_enterVehicleKeycode;
	DWORD	m_attachToVehicleKeycode;
	DWORD	m_liftDeviceKeycode;

	DWORD	m_action1Keycode;
	DWORD	m_action2Keycode;

	DWORD	m_changeCameraKeycode;

	int		m_keyNum;
};