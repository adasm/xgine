/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "Device.h"

string		wndCaption;
u32			cl_wndActive = 0;
POINT		wnd_proc_cursor_pos;
WindowPopup				*console_wnd;
TextBoxConsoleInput		*textConsoleIn;
TextConsoleOutput		*textConsoleOut;
float					scr_fading = 0.0f;
float					con_pos_y =  -220.0f;
bool					con_enabled = false;
unsigned int			con_current_history = 0;

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
		case WM_ACTIVATE:
			if(LOWORD(wParam) == WA_ACTIVE || LOWORD(wParam) == WA_CLICKACTIVE)
			{ cl_wndActive = 1; SetWindowText(hwnd, wndCaption.c_str()); }
			else
			{ cl_wndActive = 0; SetWindowText(hwnd, (wndCaption + " [PAUSED]").c_str()); }
			break;
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MOUSEMOVE:	
			{
				if(gEngine.desktop)
				{
					gEngine.desktop->postMessage(uMsg, LOWORD(lParam), HIWORD(lParam), NULL);
				}
			}break;
		case WM_KEYDOWN:
			{
				if(gEngine.desktop)
				{
					switch(wParam)
					{
						case VK_OEM_3: {con_enabled = !con_enabled;} console_wnd->setEnable(con_enabled); textConsoleIn->setText(L"");  return DefWindowProc( hwnd, uMsg, wParam, lParam );
						case VK_UP: 
							if(con_enabled)
							{ 
								con_current_history = gEngine.kernel->con->cmdBuff.size()-1;
								if(con_current_history < gEngine.kernel->con->cmdBuff.size() && con_current_history >= 0)
									textConsoleIn->setText(convstr(gEngine.kernel->con->cmdBuff[con_current_history].c_str()));
							} return DefWindowProc( hwnd, uMsg, wParam, lParam );
						case VK_DOWN:
							if(con_enabled)
							{ 
								con_current_history--;
								if(con_current_history < 0)con_current_history = 0;
								if(con_current_history < gEngine.kernel->con->cmdBuff.size() && con_current_history >= 0)
									textConsoleIn->setText(convstr(gEngine.kernel->con->cmdBuff[con_current_history]));
							} return DefWindowProc( hwnd, uMsg, wParam, lParam );
						default: break;
					}
					
					gEngine.desktop->postMessage(uMsg,  wParam, lParam, NULL);
				}
			}break;
		case WM_CHAR:
			{
				if(gEngine.desktop)
				{
					gEngine.desktop->postMessage(uMsg,  wParam, lParam, NULL);
				}
			}break;
		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;
	}

	return DefWindowProc( hwnd, uMsg, wParam, lParam );
}

void Device::enableConsole(u32 enabled)
{
	con_enabled = enabled;
	console_wnd->setEnable(con_enabled);
}

u32 Device::createRendWnd(HINSTANCE hInstance, RenderWindowInfo *rend_wnd_info)
{
	if(!rend_wnd_info)return 0;

	memset(rend_wnd_info, NULL, sizeof(RenderWindowInfo));
	
	memset(&rend_wnd_info->wndClass, NULL, sizeof(WNDCLASS));
	rend_wnd_info->wndClass.style				= 0;
	rend_wnd_info->wndClass.hInstance			= hInstance;
	rend_wnd_info->wndClass.lpszClassName		= "XGineWNDCLASS";
	rend_wnd_info->wndClass.lpfnWndProc			= WndProc;
	rend_wnd_info->wndClass.hIcon				= NULL;
	rend_wnd_info->wndClass.hCursor				= NULL;
	rend_wnd_info->wndClass.lpszMenuName		= NULL;
	rend_wnd_info->wndClass.cbClsExtra			= 0;
	rend_wnd_info->wndClass.cbWndExtra			= 0;
	rend_wnd_info->wndClass.hbrBackground		= (HBRUSH)GetStockObject(BLACK_BRUSH);
	if(FAILED(RegisterClass(&rend_wnd_info->wndClass)))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't register window class");
		return 0;
	}

	unsigned long ulWndStyle = r_windowed ? (WS_CAPTION | WS_SYSMENU | WS_BORDER) : WS_POPUP;
	unsigned long ulExWndStyle = r_windowed ? (WS_EX_TOOLWINDOW | WS_EX_APPWINDOW) : WS_EX_TOPMOST;
	RECT WndRect = {0, 0, r_width, r_height};
	AdjustWindowRectEx(&WndRect, ulWndStyle, false, ulExWndStyle);

	wndCaption = XGINE_VER;
	HWND hWindow = CreateWindowEx(ulExWndStyle, "XGineWNDCLASS", wndCaption.c_str(), ulWndStyle, 0, 0,
								  WndRect.right-WndRect.left, WndRect.bottom-WndRect.top, NULL, NULL, 
								  rend_wnd_info->wndClass.hInstance, NULL);

	if(hWindow==NULL)
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't create window: width %u, height %u, windowed %u", r_width, r_height, r_windowed);
		return 0;
	}

	ShowWindow(hWindow, SW_SHOW);

	rend_wnd_info->hInstance = hInstance;
	rend_wnd_info->hMainWnd = hWindow;
	rend_wnd_info->hRendWnd = hWindow;
	rend_wnd_info->width = r_width;
	rend_wnd_info->height = r_height;
	rend_wnd_info->windowed = r_windowed;

	//CreateThread(NULL,NULL,DeviceWndProc,0,NULL,NULL);

	gEngine.kernel->log->prnEx(LT_SUCCESS, "Device", "Created render window: width %u, height %u, windowed %u", r_width, r_height, r_windowed);
	return 1;
}

u32	Device::init(RenderWindowInfo *rend_wnd_info)
{
	if(initialized)return 1;
	if(!rend_wnd_info)return 0;

	hInstance = rend_wnd_info->hInstance;
	windowed = rend_wnd_info->windowed;
	width = rend_wnd_info->width;
	height = rend_wnd_info->height;
	hMainWnd = rend_wnd_info->hMainWnd;
	hRendWnd = rend_wnd_info->hRendWnd;
	memcpy(&wndClass, &rend_wnd_info->wndClass, sizeof(WNDCLASS));	
	cl_wndActive = 1;

	SetCursor(LoadCursor(NULL, IDC_ARROW));

	if((NULL==(pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't create Direct3D9");
		return 0;
	} 
	if(FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &D3DCaps)))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't get device caps");
		return 0;
	}
	if( FAILED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &D3Ddm ) ) )
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't get adapter display mode");
		return 0;
	}
	ZeroMemory(&D3Dpp,sizeof(D3Dpp)); 
	D3Dpp.BackBufferWidth = width; 
	D3Dpp.BackBufferHeight = height;
	D3Dpp.BackBufferCount = 2;
	D3Dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	D3Dpp.BackBufferFormat = D3Ddm.Format;
	D3Dpp.EnableAutoDepthStencil = TRUE;
	D3Dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	D3Dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if(r_multisample > 16)	r_multisample = 16;
	u32 multiSample = 0;
	for(u32 i = 0; i <= r_multisample; i++)
		if(SUCCEEDED(pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,D3Ddm.Format,0,(D3DMULTISAMPLE_TYPE)i,0)))
			multiSample = i;
	D3Dpp.MultiSampleType = (D3DMULTISAMPLE_TYPE)(multiSample);
	gEngine.kernel->log->prnEx(LT_INFO, "Device", "Device Mutlisample type %u", (multiSample));

	if(windowed)
	{
		D3Dpp.Windowed = true;
		D3Dpp.hDeviceWindow = hRendWnd;
	}
	else
	{
		D3Dpp.Windowed = false;
		D3Dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	}

	D3DDEVTYPE devType = D3DDEVTYPE_HAL;
	DWORD behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE;
	/*if(D3DCaps.VertexProcessingCaps = 0)
	{
		behaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		gEngine.kernel->log->prnEx("Using software vertex processing.");
		behaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}*/

	/*if(_strcmpi(g_vertexprocessing.c_str(), "software")==0)behaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	else if(_strcmpi(g_vertexprocessing.c_str(), "mixed")==0)behaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
	else if(_strcmpi(g_vertexprocessing.c_str(), "hardware")==0)behaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	if(_strcmpi(g_devtype.c_str(), "hal")==0)devType = D3DDEVTYPE_HAL;
	else if(_strcmpi(g_devtype.c_str(), "ref")==0)devType = D3DDEVTYPE_REF;*/

	m_hdr = (D3D_OK == pD3D->CheckDeviceFormat(D3DADAPTER_DEFAULT, devType, D3Ddm.Format, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING | D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE, D3DFMT_A16B16G16R16F));
	
	if( FAILED( pD3D->CreateDevice(D3DADAPTER_DEFAULT, devType, hRendWnd, behaviorFlags, &D3Dpp, &pD3DDevice) ))
	{
		gEngine.kernel->log->prnEx(LT_FATAL, "Device", "Couldn't create device");
		return 0;
	}

	//ShowWindow(hRendWnd, SW_SHOW);
	initialized = 1;

	/**************************************************************************/
	/*Desktop*/
	gEngine.desktop->init();
	/*Console*/
	console_wnd = new WindowPopup();
	console_wnd->setXYPos(0,-220.0f);
	console_wnd->setWidthHeight(this->getWidth(),215);
	console_wnd->setEnable(false);
	textConsoleIn = new TextBoxConsoleInput();
	textConsoleIn->setXYPos(0,200);
	textConsoleIn->setWidthHeight(this->getWidth(),15);
	textConsoleIn->setHighlight(false);
	textConsoleIn->setDrawFrameIfFocus(true);
	textConsoleIn->setEnable(false);
	textConsoleIn->setColorTextActive(0xFFFFEEEE);
	textConsoleIn->setColorTextInactive(0xFFFFEEEE);
	textConsoleOut = new TextConsoleOutput();
	textConsoleOut->setXYPos(0,0);
	textConsoleOut->setWidthHeight(this->getWidth(),200);
	textConsoleOut->setEnable(false);
	console_wnd->addChildControl(textConsoleOut);
	console_wnd->addChildControl(textConsoleIn);
	gEngine.desktop->addChildControl(console_wnd);
	return 1;
}

void Device::close()
{
	if(pD3DDevice != NULL )
		pD3DDevice->Release();
	if(pD3D != NULL )
		pD3D->Release();
	UnregisterClass(wndClass.lpszClassName, wndClass.hInstance);
}

u32 Device::restore()
{
	HRESULT result;
	u32 reset = 0;

	result = pD3DDevice->TestCooperativeLevel();
	while(result == D3DERR_DEVICELOST)
	{
		while(result != D3DERR_DEVICENOTRESET)
		{
			Sleep(200);
			MSG msg;
			PeekMessage(&msg, 0, 0, 0, PM_REMOVE);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			result = pD3DDevice->TestCooperativeLevel();
		}

		reset = 1;
		if(FAILED(pD3DDevice->Reset(&D3Dpp)))
			result = 0;
	}

	return reset;
}

u32 Device::loop()
{
	if(!initialized)return 0;

	static MSG msg;
	do
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))  
		{
			TranslateMessage(&msg);  
			DispatchMessage(&msg);
			if(msg.message == WM_QUIT) { return 0; }
		}
	}
	while(!cl_wndActive);

	return 1;
}

u32 Device::afterLoop()
{
	if(!initialized)return 0;

	if(cl_wndActive && gEngine.device->isWnd())
	{
		std::stringstream conv; 
		string str;
		conv.unsetf(std::ios::skipws);
		conv << gEngine.renderer->getFps();
		conv >> str;
		SetWindowText(getMainHWND(), (wndCaption + " FPS: " + str).c_str()); 
	}

	{
		if(con_enabled)
		{
			gEngine.desktop->moveToFront(console_wnd);
			console_wnd->setVisible(true);
			console_wnd->setFocus(textConsoleIn);

			if(con_pos_y < 0.0f)
			{
				con_pos_y += gEngine.kernel->tmr->get() * 0.5f;
				if(con_pos_y > 0.0f){ con_pos_y = 0.0f; textConsoleIn->setEnable(true); }
				console_wnd->setXYPos(0, con_pos_y);
			}
			else con_pos_y = 0.0f;
			
			if(gEngine.input->isKeyDown(DIK_PGUP))
				textConsoleOut->addToLookUpY(gEngine.kernel->tmr->get()*0.05);
			else if(gEngine.input->isKeyDown(DIK_PGDN))
				textConsoleOut->addToLookUpY(-gEngine.kernel->tmr->get()*0.05);

			if(gEngine.input->isKeyDown(DIK_LCONTROL))
				textConsoleIn->setText(convstr(gEngine.kernel->con->getItemName(convstr(textConsoleIn->getText()))));

			textConsoleOut->m_stampFade += gEngine.kernel->tmr->get() * 0.001f;
			if(textConsoleOut->m_stampFade > 1.0f)textConsoleOut->m_stampFade = 1.0f;
		}
		else
		{
			textConsoleIn->setEnable(false);
			if(con_pos_y > -220.0f)
			{
				con_pos_y -= gEngine.kernel->tmr->get() * 0.5f;
				if(con_pos_y < -220.0f){ con_pos_y = -220.0f;gEngine.desktop->setFocus(gEngine.desktop); }
				console_wnd->setXYPos(0, con_pos_y);
			}
			else console_wnd->setVisible(false);

			textConsoleOut->m_stampFade -= gEngine.kernel->tmr->get() * 0.001f;
			if(textConsoleOut->m_stampFade < 0.0f)textConsoleOut->m_stampFade = 0.0f;
		}
	}
	return 1;
}