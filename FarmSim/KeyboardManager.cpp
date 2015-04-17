#include "KeyboardManager.h"

KeyboardManager::KeyboardManager()
{
	m_forward			= false;
	m_backward			= false;
	m_left				= false;
	m_right				= false;

	m_enterVehicle		= false;
	m_attachToVehicle	= false;
	m_liftDevice		= false;

	m_action1			= false;
	m_action2			= false;
	m_changeCamera		= false;


	m_forwardKeycode			= DIK_W;
	m_backwardKeycode			= DIK_S;
	m_leftKeycode				= DIK_A;
	m_rightKeycode				= DIK_D;

	m_enterVehicleKeycode		= DIK_RETURN;
	m_attachToVehicleKeycode	= DIK_Q;
	m_liftDeviceKeycode			= DIK_E;

	m_action1Keycode			= DIK_C;
	m_action2Keycode			= DIK_V;
	
	m_changeCameraKeycode		= DIK_O;

	m_keyNum = 10;

	ifstream fin;
	fin.open("keyconfig", ios::binary);
	if(fin)
	{
		DWORD *buff;
		buff = new DWORD[m_keyNum];
		fin.read((char*)buff, m_keyNum * sizeof(DWORD));
		fin.close();

		m_forwardKeycode			= buff[0];
		m_backwardKeycode			= buff[1];
		m_leftKeycode				= buff[2];
		m_rightKeycode				= buff[3];

		m_enterVehicleKeycode		= buff[4];
		m_attachToVehicleKeycode	= buff[5];
		m_liftDeviceKeycode			= buff[6];

		m_action1Keycode			= buff[7];
		m_action2Keycode			= buff[8];

		m_changeCameraKeycode		= buff[9];
	}
}

KeyboardManager::~KeyboardManager()
{
	ofstream fout;
	fout.open("keyconfig", ios::binary | ios::trunc);
	if(fout)
	{
		DWORD *buff;
		buff = new DWORD[m_keyNum];

		buff[0] = m_forwardKeycode;
		buff[1] = m_backwardKeycode;
		buff[2] = m_leftKeycode;
		buff[3] = m_rightKeycode;

		buff[4] = m_enterVehicleKeycode;
		buff[5] = m_attachToVehicleKeycode;
		buff[6] = m_liftDeviceKeycode;

		buff[7] = m_action1Keycode;
		buff[8] = m_action2Keycode;
		buff[9] = m_changeCameraKeycode;
		fout.write((char*)buff, m_keyNum * sizeof(DWORD));
		fout.close();
	}
}

void KeyboardManager::update()
{
	static bool switchEnterVehicle = false;
	static bool switchAttachToVehicle = false;
	static bool switchLiftDevice = false;
	static bool switchAction1 = false;
	static bool switchAction2 = false;
	static bool switchChangeCamera = false;

	m_forward			= false;
	m_backward			= false;
	m_left				= false;
	m_right				= false;

	m_enterVehicle		= false;
	m_attachToVehicle	= false;
	m_liftDevice		= false;

	m_action1			= false;
	m_action2			= false;
	m_changeCamera		= false;

	if(gEngine.input->isKeyDown(m_forwardKeycode))
		m_forward = true;
	else
		m_forward = false;


	if(gEngine.input->isKeyDown(m_backwardKeycode))
		m_backward = true;
	else
		m_backward = false;


	if(gEngine.input->isKeyDown(m_leftKeycode))
		m_left = true;
	else
		m_left = false;


	if(gEngine.input->isKeyDown(m_rightKeycode))
		m_right = true;
	else
		m_right = false;

	if(gEngine.input->isKeyDown(DIK_RETURN) && !switchEnterVehicle)
	{
		m_enterVehicle = true;
		switchEnterVehicle = true;
	}
	if(gEngine.input->isKeyUp(DIK_RETURN))
		switchEnterVehicle = false;

	if(gEngine.input->isKeyDown(m_attachToVehicleKeycode) && !switchAttachToVehicle)
	{
		m_attachToVehicle = true;
		switchAttachToVehicle = true;
	}
	if(gEngine.input->isKeyUp(m_attachToVehicleKeycode))
		switchAttachToVehicle = false;

	if(gEngine.input->isKeyDown(m_liftDeviceKeycode) && !switchLiftDevice)
	{
		m_liftDevice = true;
		switchLiftDevice = true;
	}
	if(gEngine.input->isKeyUp(m_liftDeviceKeycode))
		switchLiftDevice = false;






	if(gEngine.input->isKeyDown(m_action1Keycode) && !switchAction1)
	{
		m_action1 = true;
		switchAction1 = true;
	}
	if(gEngine.input->isKeyUp(m_action1Keycode))
		switchAction1 = false;

	if(gEngine.input->isKeyDown(m_action2Keycode) && !switchAction2)
	{
		m_action2 = true;
		switchAction2 = true;
	}
	if(gEngine.input->isKeyUp(m_action2Keycode))
		switchAction2 = false;

	if(gEngine.input->isKeyDown(m_changeCameraKeycode) && !switchChangeCamera)
	{
		m_changeCamera = true;
		switchChangeCamera = true;
	}
	if(gEngine.input->isKeyUp(m_changeCameraKeycode))
		switchChangeCamera = false;
}