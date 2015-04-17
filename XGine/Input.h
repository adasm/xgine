/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API IInput
{
	virtual u32		init() = 0;
	virtual void	close() = 0;
	virtual void	clear() = 0;
	virtual void	update() = 0;
	virtual u32		isKeyDown(u32 key) = 0;
	virtual u32		isKeyUp(u32 key) = 0;
	virtual u32		isKeyPressed(u32 key) = 0;
	virtual u32		isMousePressed(u32 buttonNumber) = 0;
	virtual Vec2*	getMouseAbs() = 0;
	virtual Vec2*	getMouseRel() = 0;
	virtual f32		getMouseZRel() = 0;
};

typedef struct XGINE_API Input : IInput
{
private:
	LPDIRECTINPUT8 			lpdi;
	LPDIRECTINPUTDEVICE8	lpdikeyboard; 
	LPDIRECTINPUTDEVICE8	lpdimouse;
	HANDLE					hmouseevent;
	DIMOUSESTATE			mouse_state;
	u8						keystate[256];
	u8						oldKeystate[256];
	Vec2					MousePositionRel;
	Vec2					MousePositionAbs;
	f32						MousePosZRel;
	u32						MouseButton[4];
	u32						MouseButtonOld[4];
public:
	Input() : lpdi(0), lpdikeyboard(0), lpdimouse(0) { }
	u32		init();
	void	close();
	void	clear();
	void	update();
	u32		isKeyDown(u32 key);
	u32		isKeyUp(u32 key);
	u32		isKeyPressed(u32 key);
	u32		isMousePressed(u32 buttonNumber);
	Vec2*	getMouseAbs();
	Vec2*	getMouseRel();
	f32		getMouseZRel();
}*Input_ptr;