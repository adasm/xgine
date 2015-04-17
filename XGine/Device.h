/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

typedef struct XGINE_API RenderWindowInfo
{
	HINSTANCE	hInstance;
	HWND		hMainWnd;
	HWND		hRendWnd;
	u32			width;
	u32			height;
	u32			windowed;
	WNDCLASS	wndClass;
}*RenderWindowInfo_ptr;

extern XGINE_API u32 cl_wndActive;

typedef struct XGINE_API Device
{
private:
	LPDIRECT3D9				pD3D;
	D3DDISPLAYMODE			D3Ddm;
	D3DPRESENT_PARAMETERS	D3Dpp;
	LPDIRECT3DDEVICE9		pD3DDevice;
	D3DCAPS9				D3DCaps;

	HWND					hMainWnd;
	HWND					hRendWnd;
	HINSTANCE				hInstance;
	u32						width;
	u32						height;
	u32						windowed;
	WNDCLASS				wndClass;
	HANDLE					m_wndprocthread;
	u32						m_hdr;
	u32						initialized;

public:
	Device() : pD3D(0), pD3DDevice(0), initialized(0) { }

	static u32	createRendWnd(HINSTANCE hInstance, RenderWindowInfo *rend_wnd_info);
	u32			init(RenderWindowInfo *rend_wnd_info);
	void		close();
	u32			restore();
	u32			loop();
	u32			afterLoop();

	inline LPDIRECT3DDEVICE9	getDev()		{ return pD3DDevice; }
	inline u32					getWidth()		{ return width; }
	inline u32					getHeight()		{ return height; }
	inline u32					isWnd()			{ return windowed; }
	inline HINSTANCE			getHINSTANCE()	{ return hInstance; }
	inline HWND					getMainHWND()	{ return hMainWnd; }
	inline HWND					getRendHWND()	{ return hRendWnd; }
	inline u32					isWndActive()	{ return cl_wndActive; }
	inline u32					isHDREnabled()	{ return m_hdr; }
	inline void					enableConsole(u32 enabled);
}*Device_ptr;