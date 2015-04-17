/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "GUI.h"

//BEGIN COLORS GREEN
//#define GUI_LABEL_COLOR 0xDD566230
//#define GUI_LABEL_TEXTCOLOR 0xDDFFFFFF
//#define GUI_LABEL_FONT_SIZE 12 //#
//
//#define GUI_WINDOW_CAPTION_ACTIVE 0xDD3d5100
//#define GUI_WINDOW_CAPTION_INACTIVE 0x993d5100
//#define GUI_WINDOW_CAPTION_TEXT_ACTIVE 0xFFFFFFFF
//#define GUI_WINDOW_CAPTION_TEXT_INACTIVE 0xDDFFFFFF
//#define GUI_WINDOW_CAPTION_TEXT_FONT_SIZE 12 //#
//#define GUI_WINDOW_RECT_ACTIVE 0xDD777777
//#define GUI_WINDOW_RECT_INACTIVE 0x99777777
//#define GUI_WINDOW_ACTIVE 0xDD1e1e1e
//#define GUI_WINDOW_INACTIVE 0x991e1e1e
//
//#define GUI_POPUPWINDOW_ACTIVE 0xDD405011
//#define GUI_POPUPWINDOW_INACTIVE 0x99405011
//
//#define GUI_BUTTON_PRESSED_ACTIVE 0xDD7da308
//#define GUI_BUTTON_PRESSED_INACTIVE 0x997da308
//#define GUI_BUTTON_NORMAL_ACTIVE 0xDD394418
//#define GUI_BUTTON_NORMAL_INACTIVE 0x99394418
//#define GUI_BUTTON_TEXT_ACTIVE 0xFFFFFFFF
//#define GUI_BUTTON_TEXT_INACTIVE 0xDDFFFFFF
//#define GUI_BUTTON_TEXT_FONT_SIZE 12 //#
//
//#define GUI_TEXTBOX_ACTIVE 0xDD000000
//#define GUI_TEXTBOX_INACTIVE 0x99000000
//#define GUI_TEXTBOX_TEXT_ACTIVE 0xFFFFFFFF
//#define GUI_TEXTBOX_TEXT_INACTIVE 0xDDFFFFFF
//#define GUI_TEXTBOX_TEXT_FONT_SIZE 12 //#
//#define GUI_TEXTBOX_CARET_ACTIVE 0xFFFFFFFF
//#define GUI_TEXTBOX_CARET_INACTIVE 0xDDFFFFFF
//END COLORS

#define GUI_FONTNAME_DEFAULT "Arial"
#define GUI_FONTSIZE_DEFAULT 13
#define GUI_FONTDESC_DEFAULT "'Arial' 13"

#define GUI_LABEL_COLOR 0xDDbbbbff
#define GUI_LABEL_TEXTCOLOR 0xDDe1e1e1
#define GUI_LABEL_FONT_SIZE 10 //#

#define GUI_WINDOW_CAPTION_ACTIVE 0xEE4343ab
#define GUI_WINDOW_CAPTION_INACTIVE 0xAA5b5b9f
#define GUI_WINDOW_CAPTION_TEXT_ACTIVE 0xFFbbbbff
#define GUI_WINDOW_CAPTION_TEXT_INACTIVE 0xDDbbbbff
#define GUI_WINDOW_CAPTION_TEXT_FONT_SIZE 10 //#
#define GUI_WINDOW_RECT_ACTIVE 0xDDc2c2cb
#define GUI_WINDOW_RECT_INACTIVE 0x99c2c2cb
#define GUI_WINDOW_ACTIVE 0xDD1e1e1e
#define GUI_WINDOW_INACTIVE 0x991e1e1e

#define GUI_POPUPWINDOW_ACTIVE 0xDD1e1e1e
#define GUI_POPUPWINDOW_INACTIVE 0x991e1e1e

#define GUI_BUTTON_PRESSED_ACTIVE 0xDD5d5dff
#define GUI_BUTTON_PRESSED_INACTIVE 0x995d5dff
#define GUI_BUTTON_NORMAL_ACTIVE 0xDD8080b7
#define GUI_BUTTON_NORMAL_INACTIVE 0x998080b7
#define GUI_BUTTON_TEXT_ACTIVE 0xFFc9c9c9
#define GUI_BUTTON_TEXT_INACTIVE 0xDDc9c9c9
#define GUI_BUTTON_TEXT_FONT_SIZE 10 //#

#define GUI_TEXTBOX_ACTIVE 0xDD000000
#define GUI_TEXTBOX_INACTIVE 0x99000000
#define GUI_TEXTBOX_TEXT_ACTIVE 0xFFFFFFFF
#define GUI_TEXTBOX_TEXT_INACTIVE 0xDDFFFFFF
#define GUI_TEXTBOX_TEXT_FONT_SIZE 10 //#
#define GUI_TEXTBOX_CARET_ACTIVE 0xFFFFFFFF
#define GUI_TEXTBOX_CARET_INACTIVE 0xDDFFFFFF

BaseControl* BaseControl::addChildControl(BaseControl* Control)
{
	Control->setParentControl(this);
	if(!m_ChildControls)
		m_ChildControls = Control;
	else
	{
		BaseControl* Temp = this->getFirstChild();
		while(Temp->getNextSibling())
			Temp = Temp->getNextSibling();
		Temp->setNextSibling(Control);
		Control->setPreviousSibling(Temp);
	}
	return Control;
}

void BaseControl::removeAllChildren()
{
	BaseControl* Temp = this->getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		delete Temp;
		Temp = Next;
	}
}

BaseControl* BaseControl::removeChildControl(BaseControl* Control)
{
	BaseControl* Next = Control->getNextSibling();
	BaseControl* Previous = Control->getPreviousSibling();
	delete Control;
	Next->setPreviousSibling(Previous);
	Control = Next;
	return Control;
}

int BaseControl::getChildCount()
{
	int Count = 0;
	BaseControl* Temp = this->getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		Count++;
		Temp = Next;
	}
	return Count;
}

void BaseControl::getAbsolutePosition(Vector2* Position)
{
	Position->x += this->getXPos();
	Position->y += this->getYPos();
	if(this->m_Parent)
		this->m_Parent->getAbsolutePosition(Position);
}

bool BaseControl::cursorIntersect(FLOAT X, FLOAT Y)
{
	Vector2 ControlAbsolutePos;
	ControlAbsolutePos.x = 0;
	ControlAbsolutePos.y = 0;
	getAbsolutePosition(&ControlAbsolutePos);
	if((X >= ControlAbsolutePos.x) && (X <= ControlAbsolutePos.x + getWidth()))
	 if((Y >= ControlAbsolutePos.y) && (Y <= ControlAbsolutePos.y + getHeight()))
	  return true;
	return false;
}

bool BaseControl::cursorIntersectL(FLOAT X, FLOAT Y)
{
	BaseControl* Temp = getParentControl()->getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		if(Temp->cursorIntersect(X,Y))return (Temp == this);
		Temp = Next;
	}
	return false;
}

BaseControl* BaseControl::postToAll(UINT msg, WPARAM wParam, LPARAM lParam, void* Data)
{
	BaseControl* Temp = getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		if(Temp->postMessage(msg, wParam, lParam, Data))
			return Temp;
		Temp = Next;
	}
	return NULL;
}

bool BaseControl::postMessage(UINT msg, WPARAM wParam, LPARAM lParam, void* Data)
{
	if(!getVisible())return false;
	if(getEnable())
	{
		switch(msg)
		{
			case WM_LBUTTONUP:
				onMouseUp(1,wParam, lParam);
			case WM_MOUSEMOVE:
				onMouseMove(wParam, lParam);
				postToAll(msg, wParam, lParam, Data);
				break;
			case WM_LBUTTONDOWN:				
				if(cursorIntersect((f32)wParam, (f32)lParam))
				{
					BaseControl* Control = postToAll(msg, wParam, lParam, Data);
					if(!Control)
					{
						onMouseDown(1,wParam, lParam);

						if(this->getParentControl())
							getParentControl()->moveToFront(this);
						setFocus(this);
					}					 
					return true;
				}
				else
					return false;
				break;
			case WM_KEYUP:
			case WM_KEYDOWN:
			case WM_CHAR:
				if(getFocus() != NULL)
				{
					if(msg == WM_KEYUP)
						getFocus()->onKeyUp(wParam, lParam);
					if(msg == WM_KEYDOWN)
						getFocus()->onKeyDown(wParam, lParam);
					if(msg == WM_CHAR)
						getFocus()->onChar(wParam, lParam);
				}
				return true;
				break;
			case WM_PAINT:
			{
				{
					Vector2 Abs = Vector2(0,0);
					getAbsolutePosition(&Abs);

					//shadow
					if(m_drawShadow)
						gEngine.renderer->r2d->drawQuad(Abs.x+5, Abs.y+5, Abs.x + getWidth() + 5, Abs.y + getHeight() + 5, 0x55000000, 0x55000000, 0x55000000, 0x55000000);
					
					onRender();
					
					if(m_Focus && m_drawFrameIfFocus)
					{								
						Vector2 rect[8] = 
						{ Vector2(Abs.x-1, Abs.y-1), Vector2(Abs.x + getWidth()+1, Abs.y-1),
						  Vector2(Abs.x + getWidth()+1, Abs.y-1), Vector2(Abs.x + getWidth() + 1, Abs.y + getHeight() + 1),
						  Vector2(Abs.x + getWidth() + 1, Abs.y + getHeight() + 1), Vector2(Abs.x-1, Abs.y + getHeight()+1),
						  Vector2(Abs.x-1, Abs.y + getHeight()+1), Vector2(Abs.x-1, Abs.y-1) };
						gEngine.renderer->r2d->drawLine(rect, 8, 0xEEFFFFFF);
					}

					if(m_ChildControls)
						postToAllReverse(m_ChildControls, msg, wParam, lParam, Data);
				}
				return true;
				break;
			}
			default:
				return false;
		}
	}
	else if(msg == WM_PAINT)
	{
		{
			Vector2 Abs = Vector2(0,0);
			getAbsolutePosition(&Abs);

			//shadow
			if(m_drawShadow)
				gEngine.renderer->r2d->drawQuad(Abs.x+5, Abs.y+5, Abs.x + getWidth() + 5, Abs.y + getHeight() + 5, 0x55000000, 0x55000000, 0x55000000, 0x55000000);
			
			onRender();
			
			if(m_Focus && m_drawFrameIfFocus)
			{								
				Vector2 rect[8] = 
				{ Vector2(Abs.x-1, Abs.y-1), Vector2(Abs.x + getWidth()+1, Abs.y-1),
				  Vector2(Abs.x + getWidth()+1, Abs.y-1), Vector2(Abs.x + getWidth() + 1, Abs.y + getHeight() + 1),
				  Vector2(Abs.x + getWidth() + 1, Abs.y + getHeight() + 1), Vector2(Abs.x-1, Abs.y + getHeight()+1),
				  Vector2(Abs.x-1, Abs.y + getHeight()+1), Vector2(Abs.x-1, Abs.y-1) };
				gEngine.renderer->r2d->drawLine(rect, 8, 0xEEFFFFFF);
			}

			if(m_ChildControls)
				postToAllReverse(m_ChildControls, msg, wParam, lParam, Data);
		}
		return true;
	}
	return false;
}

void BaseControl::setFocus(BaseControl* Control)
{
	if(getParentControl())
		getParentControl()->setFocus(Control);
	else
	{
		if(getFocus())
			getFocus()->m_Focus= false;
		m_FocusControl = Control;
		Control->m_Focus = true;
	}
}

void BaseControl::onLostFocus()
{
	m_Focus = false;
	BaseControl* Temp = getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		Temp->onLostFocus();
		Temp = Next;
	}
}

BaseControl* BaseControl::postToAllReverse(BaseControl* Control, UINT msg,
			WPARAM wParam, LPARAM lParam, void* Data)
{
	BaseControl* Next = Control->getNextSibling();
	if(Next)
		Next->postToAllReverse(Next, msg, wParam, lParam, Data);
	Control->postMessage(msg, wParam, lParam, Data);
	return NULL;
}

void BaseControl::moveToFront(BaseControl* Control)
{
	if(Control)
	{
		if(this->getParentControl())
			getParentControl()->moveToFront(this);

		BaseControl* Next = Control->getNextSibling(); //Next
		BaseControl* Previous = Control->getPreviousSibling(); //Previous
		if(Previous) //Not the front
		{
			Previous->setNextSibling(Next);
			if(Next)
				Next->setPreviousSibling(Previous);
		}
		else
			return;
		Control->setNextSibling(m_ChildControls);
		m_ChildControls->setPreviousSibling(Control);
		Control->setPreviousSibling(NULL);
		m_ChildControls = Control;
	}
}

Label::Label(std::string fontDesc)
{
	m_drawShadow = false;
	drawRect = false;
	m_drawFrameIfFocus = false;
	m_Font = gEngine.resMgr->load<Font>(fontDesc);
	m_Caption = L"";
	m_Color = GUI_LABEL_COLOR;
	m_TextColor = GUI_LABEL_TEXTCOLOR;
	m_Format = DT_LEFT | DT_VCENTER;
}

Label::~Label()
{
	gEngine.resMgr->release(m_Font);
}

bool Label::onRender(void)
{
	if(m_Font && m_Font->lpFont)
	{
		RECT rect;
		Vector2 Vec2;
		Vec2.x = 0;
		Vec2.y = 0;
		getAbsolutePosition(&Vec2);
		rect.left = (u32)Vec2.x;
		rect.top = (u32)Vec2.y;
		rect.right = (u32)Vec2.x + getWidth();
		rect.bottom = (u32)Vec2.y + getHeight();
		if(drawRect)gEngine.renderer->r2d->drawQuad((f32)rect.left, (f32)rect.top, (f32)rect.right, (f32)rect.bottom,m_Color,m_Color,m_Color,m_Color);
		rect.left += 2; rect.right -= 2;
		m_Font->lpFont->DrawTextW(NULL, m_Caption.c_str(), -1, &rect, m_Format, m_TextColor);
	}
	return true;
}

RECT Label::getRenderedRect()
{
	RECT String_Info;
	ZeroMemory(&String_Info, sizeof(RECT));
	if(m_Font && m_Font->lpFont)
		m_Font->lpFont->DrawTextW(NULL, m_Caption.c_str(), m_Caption.length(),&String_Info, DT_CALCRECT, D3DCOLOR_XRGB(0,0,0));
	return String_Info;
}

Desktop::Desktop()
{
	m_drawFrameIfFocus = false;
	m_drawShadow = false;
}

Desktop::~Desktop()
{
}

void Desktop::init()
{
	m_Position = Vector2(0,0);
	m_Width = gEngine.device->getWidth();
	m_Height = gEngine.device->getHeight();
	m_Visible = true;
}

void Desktop::minimized(BaseControl * Control)
{
	std::list< BaseControl*>::iterator it;
	minimizedmap.push_front(Control);
	int lasty = 50;
	for(it = minimizedmap.begin(); it != minimizedmap.end(); it++)
	{
		(*it)->setXYPos(0.0f, (f32)lasty);
		lasty += (*it)->getHeight()+2;
	}
}

void Desktop::maximized(BaseControl* Control)
{
	std::list< BaseControl*>::iterator it;
	int lasty = 50;
	for(it = minimizedmap.begin(); it != minimizedmap.end(); it++)
	{
		if((*it) == Control){minimizedmap.erase(it); break;}
	}
	for(it = minimizedmap.begin(); it != minimizedmap.end(); it++)
	{
		(*it)->setXYPos(0, lasty);
		lasty += (*it)->getHeight()+2;
	}
}

bool Desktop::onRender() 
{ 
	return true; 
}

Window::Window()
{ 
	m_drawFrameIfFocus = true;
	m_Caption = NULL;
	m_Caption = new Label(GUI_FONTDESC_DEFAULT); 
	m_Caption->setdrawRect(true);
	m_Depressed = true;
	m_Flashed = false;
	m_Minimized = false;
	m_drawShadow = true;
	m_Moveable = true;

	m_ColorCaptionActive = GUI_WINDOW_CAPTION_ACTIVE;
	m_ColorCaptionInactive = GUI_WINDOW_CAPTION_INACTIVE;
	m_ColorCaptionTextActive = GUI_WINDOW_CAPTION_TEXT_ACTIVE;
	m_ColorCaptionTextInactive = GUI_WINDOW_CAPTION_TEXT_INACTIVE;
	m_ColorWindowActive = GUI_WINDOW_ACTIVE;
	m_ColorWindowActiveInactive = GUI_WINDOW_INACTIVE;
	m_ColorWindowRectActive = GUI_WINDOW_RECT_ACTIVE;
	m_ColorWindowRectInactive = GUI_WINDOW_RECT_INACTIVE;
}

Window::~Window()
{
	if(m_Caption)
		delete(m_Caption);
}

void Window::setCaption(const std::wstring&  Caption)
{ 
	m_Caption->setCaption(Caption); 
}

bool Window::onRender()
{
	Vector2 abs;
	abs.x = 0;
	abs.y = 0;
	getAbsolutePosition(&abs);

	bool active = (getParentControl() && getParentControl()->getFirstChild() == this && getParentControl()->getFocus() != getParentControl()) || m_Flashed;

	if(m_Minimized)
	{
		m_Caption->setXYPos(abs.x+2, abs.y+2);
		m_Caption->setWidthHeight(getWidth()-4, 18);
		m_Caption->setColor((active)?m_ColorCaptionActive:m_ColorCaptionInactive);
		m_Caption->setTextColor((active)?m_ColorCaptionTextActive:m_ColorCaptionTextInactive);
		m_Caption->onRender();
		Vector2 cross[8] = { Vector2(abs.x+m_Width-14, abs.y+5), Vector2(abs.x+m_Width-4, abs.y+5),
							 Vector2(abs.x+m_Width-4, abs.y+5),  Vector2(abs.x+m_Width-4, abs.y+17),
							 Vector2(abs.x+m_Width-4, abs.y+17), Vector2(abs.x+m_Width-14, abs.y+17),
							 Vector2(abs.x+m_Width-14, abs.y+17), Vector2(abs.x+m_Width-14, abs.y+5)};
		gEngine.renderer->r2d->drawLine(cross, 8, (active)?m_ColorWindowRectActive:m_ColorWindowRectInactive);
	}
	else
	{
		gEngine.renderer->r2d->drawQuad(abs.x, abs.y, abs.x+m_Width, abs.y+m_Height,(active)?m_ColorWindowActive:m_ColorWindowActiveInactive,(active)?m_ColorWindowActive:m_ColorWindowActiveInactive,(active)?m_ColorWindowActive:m_ColorWindowActiveInactive,(active)?m_ColorWindowActive:m_ColorWindowActiveInactive);
		gEngine.renderer->r2d->drawRect(abs.x + 1, abs.y + 1, abs.x+m_Width - 1, abs.y+m_Height - 1,(active)?m_ColorWindowRectActive:m_ColorWindowRectInactive);
		Vector2 p[2] = { Vector2(abs.x+1, abs.y+21), Vector2(abs.x+m_Width - 1, abs.y+21) };
		gEngine.renderer->r2d->drawLine(p,2, (active)?m_ColorWindowRectActive:m_ColorWindowRectInactive);

		m_Caption->setXYPos(abs.x+2, abs.y+2);
		m_Caption->setWidthHeight(getWidth()-4, 18);
		m_Caption->setColor((active)?m_ColorCaptionActive:m_ColorCaptionInactive);
		m_Caption->setTextColor((active)?m_ColorCaptionTextActive:m_ColorCaptionTextInactive);
		m_Caption->onRender();
		Vector2 cross[8] = { Vector2(abs.x+m_Width-14, abs.y+5), Vector2(abs.x+m_Width-4, abs.y+5),
							 Vector2(abs.x+m_Width-4, abs.y+5),  Vector2(abs.x+m_Width-4, abs.y+17),
							 Vector2(abs.x+m_Width-4, abs.y+17), Vector2(abs.x+m_Width-14, abs.y+17),
							 Vector2(abs.x+m_Width-14, abs.y+17), Vector2(abs.x+m_Width-14, abs.y+5)};
		gEngine.renderer->r2d->drawLine(cross,8, (active)?m_ColorWindowRectActive:m_ColorWindowRectInactive);
		Vector2 lin[2] = {Vector2(abs.x+m_Width-6, abs.y+15), Vector2(abs.x+m_Width-13, abs.y+15)};
		gEngine.renderer->r2d->drawLine(lin, 2, (active)?m_ColorWindowRectActive:m_ColorWindowRectInactive);
	}
	return true;
}

void Window::onMouseDown(int Button, int X, int Y)
{
	Vector2 Abs;
	Abs.x = 0;
	Abs.y = 0;
	getAbsolutePosition(&Abs);

	if(m_Moveable)
	{
		m_X = X - Abs.x;
		m_Y = Y - Abs.y;
		m_Depressed = false;
	}
	
	if(X - Abs.x >= (m_Width - 14) && X - Abs.x <= (m_Width - 4))
	 if(Y - Abs.y >= 5 && Y - Abs.y <= 17)
		 (m_Minimized)?restore():minimize();
	
}

void Window::onMouseMove(int X, int Y)
{
	if(!m_Moveable)return;

	if(!m_Depressed)
	{
		Vector2 Abs;
		Abs.x = 0;
		Abs.y = 0;
		getAbsolutePosition(&Abs);
		setXPos(getXPos() + ((X - Abs.x) - m_X));
		setYPos(getYPos() + ((Y - Abs.y) - m_Y));
	}

	m_Flashed = (cursorIntersectL(X,Y))?true:false;					
}

void Window::onMouseUp(int Button, int X, int Y)
{
	if(!m_Moveable)return;

	m_Depressed = true;
}

void Window::minimize()
{
	BaseControl* Temp = this->getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		Temp->setVisible(false);
		Temp = Next;
	}
	m_LastNormalPosX = (int)getXPos();
	m_LastNormalPosY = (int)getYPos();
	m_LastNormalHeight = this->getHeight();
	m_LastNormalWidth = this->getWidth();
	this->setWidthHeight(100, 20);
	if(this->getParentControl())
		this->getParentControl()->minimized(this);
	
	m_Depressed = true;
	m_Minimized = true;
	m_Moveable = false;
}

void Window::restore(void)
{
	if(this->getParentControl())
		this->getParentControl()->maximized(this);

	BaseControl* Temp = this->getFirstChild();
	while(Temp)
	{
		BaseControl* Next = Temp->getNextSibling();
		Temp->setVisible(true);
		Temp = Next;
	}
	this->setWidthHeight(m_LastNormalWidth, m_LastNormalHeight);
	this->setXYPos(m_LastNormalPosX, m_LastNormalPosY);
	
	m_Minimized = false;
	m_Moveable = true;
}

WindowPopup::WindowPopup()
{ 
	m_Moveable = false;
	m_drawFrameIfFocus = false;
	m_drawShadow = false;
	m_Flashed = false;
	m_ColorWindowActive = GUI_POPUPWINDOW_ACTIVE;
	m_ColorWindowInactive = GUI_POPUPWINDOW_INACTIVE;

	m_Sliding = true;
	startTime = 0;
}

WindowPopup::~WindowPopup()
{
}

bool WindowPopup::onRender()
{
	if(!m_Moveable && m_Flashed)
	{
		Vector2 Abs;
		Abs.x = 0;
		Abs.y = 0;
		getAbsolutePosition(&Abs);

		const f32 range = 40.0f;

		int X = lastX, Y = lastY;

		if(m_Height >  gEngine.device->getHeight())
		{
			if(X > Abs.x && X < Abs.x + m_Width)
			{
				if(Y < range)
				{
					f32 speed = 0.2f;

					if(Abs.y < 0)
						setYPos(getYPos() + gEngine.kernel->tmr->get() * speed);
					if(getYPos() > 0)setYPos(0);
				}
				else if(Y < gEngine.device->getHeight() && Y > gEngine.device->getHeight() - range)
				{
					f32 speed = 0.2f;

					if(Abs.y + m_Height > gEngine.device->getHeight())
						setYPos(getYPos() - gEngine.kernel->tmr->get() * speed);
					else setYPos((f32)gEngine.device->getHeight()-(f32)m_Height);
				}
			}
		}
	}

	Vector2 abs;
	abs.x = 0;
	abs.y = 0;
	getAbsolutePosition(&abs);
	//bool active = (getParentControl() && getParentControl()->getFirstChild() == this && getParentControl()->getFocus() != getParentControl()) || m_Flashed;
	//gEngine.renderer->r2d->drawQuad(abs.x, abs.y, abs.x+m_Width, abs.y+m_Height,(active)?m_ColorWindowActive:m_ColorWindowInactive,(active)?m_ColorWindowActive:m_ColorWindowInactive,(active)?m_ColorWindowActive:m_ColorWindowInactive,(active)?m_ColorWindowActive:m_ColorWindowInactive);
	gEngine.renderer->beginGuiShader();
	gEngine.renderer->r2d->drawQuadTexMatch(abs.x, abs.y, abs.x+m_Width, abs.y+m_Height);
	gEngine.renderer->endGuiShader();

	D3DXCOLOR c1 = D3DXCOLOR(0,0,0,0);
	D3DXCOLOR c2 = D3DXCOLOR(0,0,0,1);

	//f32 size = 200.0f;

	gEngine.renderer->r2d->begin();
	/*gEngine.renderer->r2d->drawQuad(abs.x, 0,abs.x + getWidth(),size,c2, c2, c1, c1);
	gEngine.renderer->r2d->drawQuad(abs.x, gEngine.device->getHeight() - size,abs.x + getWidth(),gEngine.device->getHeight(),c1, c1, c2, c2);*/

	if(m_Height >  gEngine.device->getHeight())
	{
		f32 side = 10;	
		f32 time = timeGetTime();
		f32 alpha = pow(sin(time*0.003f),1) - 0.6;
		if(alpha < 0)alpha = -alpha;

		alpha *= 0.5f;

		if(abs.y < 0)
		{
			Vec2 lines[6];
			lines[0] = Vec2(abs.x + getWidth()/2, side*3);
			lines[4] = lines[2] = lines[1] = Vec2(abs.x + getWidth()/2, 2);
			lines[3] = Vec2(abs.x + getWidth()/2-side, side*2);
			lines[5] = Vec2(abs.x + getWidth()/2+side, side*2);

			Vec2 linesEx[6];
			for(i32 x = -2; x <= 2; x++)
				for(i32 y = 0; y <= 5; y++)
				{
					for(u32 i = 0; i < 6; i++)linesEx[i] = lines[i] + Vec2(x,y);
					gEngine.renderer->r2d->drawLine(linesEx, 6, D3DXCOLOR(0.3,0.3,0.3,alpha));
				}
		}
		else
		{
			Vec2 lines[6];
			lines[0] = Vec2(abs.x + getWidth()/2, gEngine.device->getHeight() - side*3);
			lines[4] = lines[2] = lines[1] = Vec2(abs.x + getWidth()/2, gEngine.device->getHeight() - 2);
			lines[3] = Vec2(abs.x + getWidth()/2-side, gEngine.device->getHeight() - side*2);
			lines[5] = Vec2(abs.x + getWidth()/2+side, gEngine.device->getHeight() - side*2);

			

			Vec2 linesEx[6];
			for(i32 x = -2; x <= 2; x++)
				for(i32 y = 0; y >= -5; y--)
				{
					for(u32 i = 0; i < 6; i++)linesEx[i] = lines[i] + Vec2(x,y);
					gEngine.renderer->r2d->drawLine(linesEx, 6, D3DXCOLOR(0.3,0.3,0.3,alpha));
				}
		}
	}
	gEngine.renderer->r2d->end();
	return true;
}

void WindowPopup::onMouseDown(int Button, int X, int Y)
{	
	Vector2 Abs;
	Abs.x = 0;
	Abs.y = 0;
	getAbsolutePosition(&Abs);

	if(m_Moveable)
	{
		m_X = X - Abs.x;
		m_Y = Y - Abs.y;
		m_Depressed = false;
	}
}

void WindowPopup::onMouseMove(int X, int Y)
{
	if(!m_Depressed)
	{
		Vector2 Abs;
		Abs.x = 0;
		Abs.y = 0;
		getAbsolutePosition(&Abs);
		setXPos(getXPos() + ((X - Abs.x) - m_X));
		setYPos(getYPos() + ((Y - Abs.y) - m_Y));
	}

	m_Flashed = (cursorIntersectL(X,Y))?true:false;		

	if(m_Flashed)
	{
		lastX = X;
		lastY = Y;
	}
}

void WindowPopup::onMouseUp(int Button, int X, int Y)
{
	if(!m_Moveable)return;

	m_Depressed = true;
}

Button::Button(std::string fontDesc)
{
	this->m_ChildControls = 0;
	m_drawShadow = false;
	m_drawFrameIfFocus = true;
	m_Pressed = false;
	m_Flashed = false;
	m_Caption = NULL;
	m_Caption = new Label(fontDesc);
	m_Caption->setFormat( DT_LEFT | DT_VCENTER | DT_CENTER );

	m_ColorPressedActive = GUI_BUTTON_PRESSED_ACTIVE;
	m_ColorPressedInactive = GUI_BUTTON_PRESSED_INACTIVE;
	m_ColorNormalActive = GUI_BUTTON_NORMAL_ACTIVE;
	m_ColorNormalInactive = GUI_BUTTON_NORMAL_INACTIVE;
	m_ColorTextActive = GUI_BUTTON_TEXT_ACTIVE;
	m_ColorTextInactive = GUI_BUTTON_TEXT_INACTIVE;

}

void Button::setCaption(const std::wstring& Caption)
{			
	m_Caption->setCaption(Caption);
}	

bool Button::onRender()
{
	RECT rect;
	Vector2 Abs;
	Abs.x = 0;
	Abs.y = 0;
	getAbsolutePosition(&Abs);
	rect.left = Abs.x;
	rect.top = Abs.y;
	rect.right = Abs.x + getWidth();
	rect.bottom =	Abs.y + getHeight();		

	bool active = (getParentControl() && getParentControl()->getFirstChild() == this && getParentControl()->getFocus() != getParentControl()) || m_Flashed;
	
	if(m_Pressed)
		gEngine.renderer->r2d->drawQuad(rect.left, rect.top, rect.right, rect.bottom, (active)?m_ColorPressedActive:m_ColorPressedInactive, (active)?m_ColorPressedActive:m_ColorPressedInactive, (active)?m_ColorPressedActive:m_ColorPressedInactive, (active)?m_ColorPressedActive:m_ColorPressedInactive);
	else 
		gEngine.renderer->r2d->drawQuad(rect.left, rect.top, rect.right, rect.bottom, (active)?m_ColorNormalActive:m_ColorNormalInactive, (active)?m_ColorNormalActive:m_ColorNormalInactive, (active)?m_ColorNormalActive:m_ColorNormalInactive, (active)?m_ColorNormalActive:m_ColorNormalInactive);
		
	if(m_Caption)
	{
		Abs.x = 0;
		Abs.y = 0;
		getAbsolutePosition(&Abs);
		m_Caption->setXYPos(Abs.x, Abs.y);
		m_Caption->setWidthHeight(getWidth(),getHeight());
		m_Caption->setTextColor((active)?m_ColorTextActive:m_ColorTextInactive);
		m_Caption->onRender();
	}
	return true;
}

void Button::onMouseMove(int X, int Y)
{
	//m_Flashed = (cursorIntersectL(X,Y) && getParentControl()->getParentControl()->getFirstChild() == getParentControl())?true:false;	
}

void Button::onMouseDown(int Button, int X, int Y)
{
	m_Pressed = !m_Pressed;
}

Button::~Button()
{
	if(m_Caption)
		delete(m_Caption);
}

TextBox::TextBox()
{		
	m_drawShadow = false;
	m_drawFrameIfFocus = true;
	m_Font = gEngine.resMgr->load<Font>("'Lucida Console' 12");
	ZeroMemory(m_CaretVector, sizeof(Vector2) * 2);
	m_TextWidth = 0.0f;
	m_Text = L"";
	m_CaretVisible = false;
	m_Format = DT_LEFT | DT_VCENTER;
	CaretCharPos = 0;
	accepted = false;
	highlight = true;

	m_ColorActive = GUI_TEXTBOX_ACTIVE;
	m_ColorInactive = GUI_TEXTBOX_INACTIVE;
	m_ColorTextActive = GUI_TEXTBOX_TEXT_ACTIVE;
	m_ColorTextInactive = GUI_TEXTBOX_TEXT_INACTIVE;
	m_ColorCaretActive = GUI_TEXTBOX_CARET_ACTIVE;
	m_ColorCaretInactive = GUI_TEXTBOX_CARET_INACTIVE;
}


TextBox::~TextBox()
{
	gEngine.resMgr->release(m_Font);
}

FLOAT TextBox::getStringWidth(std::wstring String)
{

	for(unsigned int i = 0; i<String.size(); i++)if(String[i] == L' ')String[i] = '.';
	RECT String_Info;
	ZeroMemory(&String_Info, sizeof(RECT));
	if(m_Font && m_Font->lpFont)m_Font->lpFont->DrawTextW(NULL, String.c_str(), String.length(),&String_Info, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_XRGB(0,0,0));
	return String_Info.right;
}

FLOAT TextBox::getStringHeight(std::wstring String)
{
	/*RECT String_Info;
	ZeroMemory(&String_Info, sizeof(RECT));
	if(m_Font)m_Font->drawText(NULL, String.c_str(), String.length(),&String_Info, DT_CALCRECT | DT_NOCLIP, D3DCOLOR_XRGB(0,0,0));*/
	return 12;//String_Info.bottom;
}

void TextBox::setText(const std::wstring& Text)
{
	int Width = getStringWidth(m_Text);
	if(Width <= getWidth())
	{
		m_Text = Text;
		m_TextWidth = Width;
		setCaretPos(m_Text.length());
	}
}

void TextBox::setCaretPos(long Pos)
{
	if((Pos >= 0) && (Pos <= m_Text.length()))
	{
		std::wstring tmpstr = m_Text.substr(0, Pos);
		//Top of line
		m_CaretVector[0].x = getStringWidth(tmpstr); //Set X Pos
		//Bottom of line
		m_CaretVector[1].x = m_CaretVector[0].x;
		CaretCharPos = Pos;
	}
}

bool TextBox::insertText(const std::wstring& Text)
{
	std::wstring TmpStr = Text;
	if((m_TextWidth + getStringWidth(TmpStr)) <= getWidth())
	{
		m_Text.insert(CaretCharPos, Text);
		m_TextWidth = getStringWidth(m_Text);
		setCaretPos(getCaretPos()+Text.length());
		return true;
	}
	return false;
}

long TextBox::removeText(long Quantity)
{
	setCaretPos(getCaretPos()-Quantity);
	m_Text.erase(CaretCharPos, Quantity);
	m_TextWidth = getStringWidth(m_Text);
	return m_TextWidth;
}

void TextBox::onKeyDown(WPARAM Key, LPARAM Extended)
{
	if(!getEnable())return;
		switch (Key)
		{
			case VK_BACK:
			case VK_DELETE:
				{
					removeText(1);
				}break;
			case VK_LEFT:
				{
					setCaretPos(getCaretPos()-1);
				}break;
			case VK_RIGHT:
				{
					setCaretPos(getCaretPos()+1);
				}break;
			case VK_RETURN:
				{
					accepted = true;
				}break;
		}
}

void TextBox::onChar(WPARAM Key, LPARAM Extended)
{
	if(!getEnable())return;
	if(Key >= 32 && Key <= 126)
		insertText((wchar_t*) &Key);
}

bool TextBox::cursorIntersectChar(int X, int Y)
{
	Vector2 Pos;
	this->getAbsolutePosition(&Pos);
	if((X >= Pos.x) && (X <= Pos.x + m_TextWidth))
		if((Y >= Pos.y) && (Y <= Pos.y + getHeight()))
			return true;
	return false;
}

long TextBox::getCharAtPos(int X, int Y)
{
	long Pos = 0;
	long Left = 0;
	long Right= 0;
	Vector2 Abs;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	Left = Abs.x;
	while(Pos < m_Text.length())
	{
		std::wstring TmpStr = m_Text.substr(Pos, 1);
		Right = getStringWidth(TmpStr);
		if((X >= Left) && (X <= Left + Right))
		break;
		Pos++;
		Left += Right;
	}
	return Pos;
}

void TextBox::onMouseDown(int Button, int X, int Y)
{
	if(m_Focus)
		setCaretPos(getCharAtPos(X, Y));
}

void TextBox::onMouseMove(int X, int Y)
{
	if(getParentControl() && getParentControl()->getParentControl())
		m_Flashed = (cursorIntersectL(X,Y) && getParentControl()->getParentControl()->getFirstChild() == getParentControl())?true:false;
	else if(getParentControl())
		m_Flashed = (cursorIntersectL(X,Y) && getParentControl()->getFirstChild() == getParentControl())?true:false;
}	

bool TextBox::onRender()
{
	Vector2 Abs;
	RECT rect;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	rect.left = Abs.x;
	rect.top = Abs.y;
	rect.right = Abs.x + getWidth();
	rect.bottom = Abs.y + getHeight();

	static float carret_vis = 0.0f;
	carret_vis += gEngine.kernel->tmr->get();
	if(carret_vis > 1000){ carret_vis = 0.0f; m_CaretVisible = !m_CaretVisible; }

	bool active = ((getParentControl() && getParentControl()->getFirstChild() == this && getParentControl()->getFocus() != getParentControl()) || m_Flashed ) && highlight;

	//
	gEngine.renderer->beginGuiShader();
	gEngine.renderer->r2d->drawQuadTexMatch(rect.left, rect.top, rect.right, rect.bottom);
	gEngine.renderer->endGuiShader();
	//gEngine.renderer->r2d->drawQuad(rect.left, rect.top, rect.right, rect.bottom, (active)?m_ColorActive:m_ColorInactive, (active)?m_ColorActive:m_ColorInactive, (active)?m_ColorActive:m_ColorInactive, (active)?m_ColorActive:m_ColorInactive);
	
	if(m_Font && m_Font->lpFont)
		m_Font->lpFont->DrawTextW(NULL, m_Text.c_str(), m_Text.length(), &rect, m_Format, (active)?m_ColorTextActive:m_ColorTextInactive);

	if(m_Focus)
		if(m_CaretVisible)
		{
			Vector2 Absolute[2];
			ZeroMemory(Absolute, sizeof(Vector2) * 2);
			Absolute[0].x = Abs.x + m_CaretVector[0].x;
			Absolute[0].y = Abs.y + m_CaretVector[0].y;
			Absolute[1].x = Abs.x + m_CaretVector[0].x;
			Absolute[1].y = Abs.y + m_CaretVector[0].y + getHeight();
			gEngine.renderer->r2d->drawLine(Absolute, 2, (active)?m_ColorCaretActive:m_ColorCaretInactive);

		}
	return true;
}

void TextBoxConsoleInput::onKeyDown(WPARAM Key, LPARAM Extended)
{
	if(!getEnable())return;
		switch (Key)
		{
			case VK_BACK:
			case VK_DELETE:
				{
					removeText(1);
				}break;
			case VK_LEFT:
				{
					setCaretPos(getCaretPos()-1);
				}break;
			case VK_RIGHT:
				{
					setCaretPos(getCaretPos()+1);
				}break;
			case VK_RETURN:
				{
					gEngine.kernel->con->exec(convstr(getText()));
					setText(L"");
				}break;
		}
}



TextConsoleOutput::TextConsoleOutput()
{
	m_drawFrameIfFocus = false;
	m_drawShadow = false;
	m_Flashed = false;
	lookup_y = 0.0f;
	max_lookup_y = 0.0f;
	last_size = 0;
	m_stampFade = 1.0f;
	m_Font = gEngine.resMgr->load<Font>("'Lucida Console' 10");
}
TextConsoleOutput::~TextConsoleOutput()
{
}

bool TextConsoleOutput::onRender()
{
	Vector2 Abs;
	RECT rect;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	rect.left = Abs.x;
	rect.top = Abs.y;
	rect.right = Abs.x + getWidth();
	rect.bottom = Abs.y + getHeight();
	unsigned int pos_yy = rect.top;

	/*gEngine.renderer->beginGuiShader();
	gEngine.renderer->r2d->drawQuadTexMatch(rect.left, rect.top, rect.right, rect.bottom);
	gEngine.renderer->endGuiShader();*/
	
	unsigned int s = gEngine.kernel->con->txtBuff.size();
	unsigned int maxLines = getHeight()/10;
	if(s != last_size)
	{
		last_size = s;
		lookup_y = 0.0f;
		max_lookup_y = s-maxLines+2;
	}
	if(s > 0)
	{
		int i = s-1-lookup_y;
		i = (i<0)?0:i;
		for(int c = 1; c <= maxLines && i >= 0; i--, c++)
		{
			rect.top = pos_yy + getHeight() - c*10;
			if(m_Font && m_Font->lpFont)
				m_Font->lpFont->DrawText(NULL,  gEngine.kernel->con->txtBuff[i].c_str(),  gEngine.kernel->con->txtBuff[i].length(), &rect, DT_LEFT | DT_SINGLELINE,D3DXCOLOR(1.0f,1.0f,1.0f, (float)(((float)maxLines-c+1.0f)/maxLines) + 0.05f));
		}
	}

	rect.top = 0;
	if(m_Font && m_Font->lpFont && m_stampFade > 0)
		m_Font->lpFont->DrawText(NULL, XGINE_VER, string(XGINE_VER).length(), &rect, DT_RIGHT | DT_SINGLELINE,D3DXCOLOR(1,0.0f,0, 1.0f) * m_stampFade);
	return true;
}

void TextConsoleOutput::onMouseMove(int X, int Y)
{
		//m_Flashed = (cursorIntersectL(X,Y))?true:false;
}

/*
ProfilerOutput::ProfilerOutput()
{
	m_drawFrameIfFocus = false;
	m_drawShadow = false;
	m_Flashed = false;
	D3DXCreateFont(gEngine.device->getDev(),10,0,FW_NORMAL,0,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH | FF_DONTCARE,"Lucida Console",&m_Font);
}
ProfilerOutput::~ProfilerOutput()
{
}

bool ProfilerOutput::onRender()
{
	Vector2 Abs;
	RECT rect;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	rect.left = Abs.x;
	rect.top = Abs.y;
	rect.right = Abs.x + getWidth();
	rect.bottom = Abs.y + getHeight();

	gEngine.renderer->r2d->drawQuad(rect.left, rect.top, rect.right, rect.bottom,0xEE000000,0xEE000000,0x99000000,0x99000000);
	gEngine.renderer->r2d->drawRect(rect.left, rect.top, rect.right, rect.bottom, 0xEE000000);

	char record[512]; 
	rect.right++;
	rect.top++;
	sprintf_s(record, 512, "%-25s %s  %s  %s", "Block name:", "Avg CT [ms]", "Sum CT [ms]", "Calls");
	m_Font->drawText(NULL, record, strlen(record), &rect, DT_LEFT | DT_SINGLELINE, 0xFF00FF00);
	rect.top += 12;

	if(gProfiler.counters.size() > 0)
		for(_cProfiler::tContainerIterator it = gProfiler.counters.begin(); it != gProfiler.counters.end() && rect.top <= rect.bottom; it++, rect.top += 10)
		{
			sprintf_s(record, 512, "%-25s %011.6f  %011u  %-18u", (it->first).substr(0,20).c_str(), it->second.avg, it->second.sum,  it->second.calls);
			m_Font->drawText(NULL, record, strlen(record), &rect, DT_LEFT | DT_SINGLELINE, 0xFFFFFFFF);
		}

	return true;
}

void ProfilerOutput::onMouseMove(int X, int Y)
{
		//m_Flashed = (cursorIntersectL(X,Y))?true:false;
}
*/

ScrollBar::ScrollBar(u32 width, u32 height, i32 Min, i32 Max, f32 Mul, f32 *var, string name)
{
	setWidthHeight(width, height);
	m_drawShadow = false;
	m_drawFrameIfFocus = true;
	m_Flashed = false;
	m_ThumbPosition = 0;
	m_ThumbHeight = 0;
	m_Change = 1;
	m_var = var;
	varMul = Mul;
	varName = name;
	ZeroMemory(&m_ThumbPos, sizeof(Vector2));
	setMinMax(Min,Max+1);
	setValue((m_var)?(*m_var * 100 / Mul):(0));
	m_DragMode = false;
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::setMinMax(int Min, int Max)
{
	m_Min = Min;
	m_Max = Max;
	if(m_ThumbPosition > m_Max)
		m_ThumbPosition = m_Max;
	if(m_ThumbPosition < Min)
		m_ThumbPosition = Min;
}

int ScrollBar::getPosValue(FLOAT Pos)
{
	if(Pos >= 20)
	{
		if(Pos < (getWidth() - 20))
		{
			FLOAT Range = getWidth() - 40;
			FLOAT Increment = Range / m_Max;
			for(int counter = m_Min; counter < m_Max; counter++)
			{
				FLOAT Item = 20 + (Increment * counter);
				if(((Pos >= Item)) && (Pos <= (Item + Increment)))
					return counter;
			}
		}
		
		return m_Max-1;
	}
	else return 0;
}

FLOAT ScrollBar::autoSizeThumb()
{
	FLOAT Range = (float)getWidth() - 40;
	FLOAT Increment = Range / m_Max;
	return (Increment / 10);
}

void ScrollBar::setValue(int Value)
{
	int NewValue = Value;
	if(Value < m_Min)
	NewValue = m_Min;
	else if (Value >= m_Max)
	NewValue = m_Max - 1;
	FLOAT Range = this->getWidth() - 40;
	FLOAT Increment = Range / m_Max;
	m_ThumbPos.x = 20 + (Increment * NewValue);
	m_ThumbPosition = NewValue;
	if(m_var)*m_var = ((f32)NewValue/(f32)(m_Max-1)) * (f32)varMul;
}

void ScrollBar::updateScaling()
{
	if((getWidth()-40)<0)
		setWidth(50);
	m_ThumbPos.y = (getWidth()-40)/(m_Max - m_Min);
	if(m_ThumbPos.x<20)m_ThumbPos.x=20;
	m_ThumbHeight = getHeight();
}

void ScrollBar::onMouseDown(int Button, int X, int Y)
{
	Vector2 Abs;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	if ((X >= Abs.x) && (X <= Abs.x + 20))
	{
		setValue(m_ThumbPosition - m_Change);
		return;
	}
	if ((X >= (Abs.x + getWidth()) - 20))
	if (X <= Abs.x + getWidth())
	{
		setValue(m_ThumbPosition + m_Change);
		return;
	}
	FLOAT Thumb = Abs.x + m_ThumbPos.x;
	if((X >= Thumb) && (X <= Thumb + m_ThumbPos.y))
	{
		m_DragMode = true;
		//return;
	}
	FLOAT Val = 20 + (X -(Abs.x + 20));
	setValue(getPosValue(Val));
}

void ScrollBar::onMouseMove(int X, int Y)
{
	Vector2 Abs;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);
	//FLOAT Thumb = (Abs.x + 20) + m_ThumbPos.x;
	if(m_DragMode)
	{
		FLOAT Val = 20 + (X -(Abs.x + 20));
		setValue(getPosValue(Val));
		return;
	}

	m_Flashed = (cursorIntersectL(X,Y) && getParentControl()->getParentControl()->getFirstChild() == getParentControl())?true:false;
}

void ScrollBar::onMouseUp(int Button, int X, int Y)
{
	m_DragMode = false;
}

FLOAT ScrollBar::autoSizeBackground()
{
	FLOAT Range = getWidth() - 40;
	FLOAT Increment = Range / m_Max;
	return (Increment / m_BackgroundHeight);
}

bool ScrollBar::onRender()
{
	Vector2 Abs = Vector2(0,0);
	getAbsolutePosition(&Abs);
	DWORD arrowColor = (m_Flashed)?0xEE333333:0xAA333333;
	//ArrowUp
	gEngine.renderer->r2d->drawQuad(Abs.x, Abs.y, Abs.x + 20, Abs.y + getHeight(), arrowColor, arrowColor, arrowColor, arrowColor);
	//Vector2 arrowup[4] = { Vector2(Abs.x, Abs.y + 20), Vector2(Abs.x+10, Abs.y),
	//						   Vector2(Abs.x+10, Abs.y), Vector2(Abs.x+20, Abs.y + 20) };
	//gEngine.renderer->r2d->drawLine(arrowup, 4, 0xEE000000);
	//ArrowDown
	gEngine.renderer->r2d->drawQuad(Abs.x + (getWidth()-20), Abs.y, Abs.x + getWidth(), Abs.y + getHeight(), arrowColor, arrowColor, arrowColor, arrowColor);
	//Vector2 arrowdown[4] = { Vector2(Abs.x, Abs.y + getHeight() - 20), Vector2(Abs.x+10, Abs.y + getHeight()),
	//							 Vector2(Abs.x+10, Abs.y + getHeight()), Vector2(Abs.x+20, Abs.y + getHeight() - 20) };
	//gEngine.renderer->r2d->drawLine(arrowdown, 4, 0xEE000000);
	//Thumb
	updateScaling();
	gEngine.renderer->r2d->drawQuad(Abs.x + m_ThumbPos.x, Abs.y, Abs.x + m_ThumbPos.x + m_ThumbPos.y, Abs.y + getHeight(), 0xEFff0000, 0xEFff0000, 0xEFff0000, 0xEFff0000);
	if(m_var)
		gEngine.renderer->r2d->drawTextSmall(Abs.x + 20, Abs.y, 0xBBffffff, "%s = %.2f", varName.c_str(), *m_var);
	return true;
}

CheckBox::CheckBox(u32 *var, const std::wstring& strEnabled, const std::wstring& strDisabled)
{
	m_strEnabled = strEnabled;
	m_strDisabled = strDisabled;
	m_variable = var;
	m_drawShadow = false;
	m_drawFrameIfFocus = true;
	m_Checked = (m_variable)?(*m_variable):(true);
	if(strEnabled != strDisabled)
	{
		m_TickBox = new Button(GUI_FONTDESC_DEFAULT);
		m_TickBox->setPressed(m_Checked);	
	}
	else 
	{
		m_TickBox = 0;
	}
	m_Label = new Label(GUI_FONTDESC_DEFAULT);
	m_Label->setdrawRect(false);
	m_Label->setCaption( m_Checked? ( m_strEnabled ) : ( m_strDisabled ) );
}

CheckBox::~CheckBox()
{
	if(m_TickBox)
		delete m_TickBox;
	if(m_Label)
		delete m_Label;
}

void CheckBox::setCaption(const wstring& Text)
{
	m_Label->setCaption(Text);
}

void CheckBox::onMouseDown(int Button, int X, int Y)
{
	*m_variable = !(*m_variable);
}

void CheckBox::onMouseMove(int X, int Y)
{
	m_Flashed = (cursorIntersectL(X,Y) && getParentControl()->getParentControl()->getFirstChild() == getParentControl())?true:false;	
}

bool CheckBox::onRender()
{
	Vector2 Abs;
	ZeroMemory(&Abs, sizeof(Vector2));
	getAbsolutePosition(&Abs);	

	m_Checked = (*m_variable)?(1):(0);
	if(m_TickBox)
	{
		m_Label->setCaption( m_Checked? ( m_strEnabled ) : ( m_strDisabled ) );
		m_TickBox->setPressed(m_Checked);

		m_TickBox->setXYPos(Abs.x,Abs.y);
		m_TickBox->setWidthHeight(getHeight(),getHeight());
		m_TickBox->setFlashed(m_Flashed);
		m_Label->setXYPos(Abs.x + getHeight(),Abs.y);
		m_Label->setWidthHeight(getWidth()-getHeight(),getHeight());

		m_TickBox->onRender();
		m_Label->onRender();
	}
	else
	{
		m_Label->setXYPos(Abs.x,Abs.y);
		m_Label->setWidthHeight(getWidth(),getHeight());
		m_Label->onRender();
	}
	return true;
}

u32 guiMenu_shadows_disabled = 0;
u32 guiMenu_shadows_sv = 0;
u32 guiMenu_shadows_vsm = 0;
u32 guiMenu_shadows_vsm_blur = 0;
u32 guiMenu_shadows_vsm_boxfiler = 0;
u32 guiMenu_shadows_vsm_blur_boxfilter = 0;
u32 guiMenu_shadows_esm = 0;
u32 guiMenu_shadows_sm_g = 0;
u32 guiMenu_shadows_sm_g_pcf3x3 = 0;
u32 guiMenu_shadows_sm_g_pcf5x5 = 0;
u32 guiMenu_shadows_sm_g_pcf7x7 = 0;
u32 guiMenu_shadows_sm_g_blur = 0;
u32 guiMenu_shadows_sm_g_blur_pcf3x3 = 0;
u32 guiMenu_shadows_sm_g_blur_pcf5x5 = 0;
u32 guiMenu_shadows_sm_g_blur_pcf7x7 = 0;
u32 guiMenu_shadows_sm = 0;
u32 guiMenu_shadows_sm_pcf3x3 = 0;
u32 guiMenu_shadows_sm_pcf5x5 = 0;
u32 guiMenu_shadows_sm_pcf7x7 = 0;
u32 guiMenu_shadows_sm_blur = 0;
u32 guiMenu_shadows_sm_blur_pcf3x3 = 0;
u32 guiMenu_shadows_sm_blur_pcf5x5 = 0;
u32 guiMenu_shadows_sm_blur_pcf7x7 = 0;
u32 guiMenu_shadows_sm_d;
u32 guiMenu_shadows_sm_d_pcf3x3 = 0;
u32 guiMenu_shadows_sm_d_pcf5x5 = 0;
u32 guiMenu_shadows_sm_d_pcf7x7 = 0;

void GUIMenu::init()
{
	settWnd = new WindowPopup();
	WindowPopup *wnd = settWnd;
	wnd->setEnable(true);
	wnd->setVisible(true);
	u32 sep = 3;
	u32 pos = 5 + 10;
	u32 height = 16;
	u32 width = 210;
	Label * t;
	CheckBox *cb;
	ScrollBar *sb;

	string subtitleDesc = "'Lucida Console' 15";


	t = new Label(subtitleDesc); t->setCaption(L"[XGine Settings]");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"RENDERER");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_enabledDeferred , L"Deferred: Enabled", L"Deferred: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledLightPrePass , L"Light Pre-Pass: Enabled", L"Light Pre-Pass: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_forceForward , L"Force Forward: Enabled", L"Force Forward: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledZPrePass , L"Z Pre-Pass: Enabled", L"Z Pre-Pass: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_hidestats , L"Hide stats: Enabled", L"Hide stats: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_optimizeRSChanges , L"Optimize RS Changes: Enabled", L"Optimize RS Changes: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	

	t = new Label(subtitleDesc); t->setCaption(L"HDR RENDERING");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_hdrRendering , L"HDR Rendering: Enabled", L"HDR Rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_hdrToneMap , L"Tone Mapping: Enabled", L"Tone Mapping: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_hdrToneMapScale, "Tone Map Scale");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_hdrEyeAdaptation , L"HDR Dynamic Eye Adaptation: Enabled", L"HDR Dynamic Eye Adaptation: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_hdrEyeAdaptationTimeScale, "Eye Adaptation Time Scale");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_hdrBloom , L"HDR Bloom: Enabled", L"HDR Bloom: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_hdrBrightPass , L"Bloom Bright Pass: Enabled", L"Bloom Bright Pass: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_hdrBloomAmount, "Bloom Amount");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_hdrColorGradeEnabled , L"HDR ColorGrade: Enabled", L"HDR ColorGrade: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_hdrColorGrade.x, "HDR ColorGrade -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_hdrColorGrade.y, "HDR ColorGrade -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_hdrColorGrade.z, "HDR ColorGrade -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;

	t = new Label(subtitleDesc); t->setCaption(L"VOLUMETRIC LIGHT SCATTERING");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_enabledVLS, L"VLS: Enabled", L"VLS: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledHDRVLS, L"HDR VLS: Enabled", L"HDR VLS: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"WATER RENDERING");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_waterRenderingEnabled , L"Water Rendering: Enabled", L"Water Rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_highQualityWaterEnabled , L"High Quality Water: Enabled", L"High Quality Water: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_waterOcean , L"Ocean Waves: Enabled", L"Ocean Waves Water: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterColor.x, "Water Color -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterColor.y, "Water Color -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterColor.z, "Water Color -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterH0coef, "H0 Coefficient");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterH1coef, "H1 Coefficient");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1,  &r_waterH2coef, "H2 Coefficient");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterH3coef, "H3 Coefficient");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterDetailNormalMapCoef, "Detail NormalMap Coefficient");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 20, &r_waterDetailNormalMapScale, "Detail NormalMap Scale");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 100, &r_waterAmplitude, "Waves Amplitude");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterFresnelR0, "Fresnel R0 Constant");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_waterRefractionStrength, "Refraction Strength");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 400, &r_waterSpecularExp, "Specular Exponent");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 3, &r_refExp, "Refraction Exponent");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;

	t = new Label(subtitleDesc); t->setCaption(L"ATMOSPHERE");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	sb = new ScrollBar(width, height/2, 0, 100, 9000, &r_fogParams.x, "Fog Start");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 9000, &r_fogParams.y, "Fog End");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_fogParams.z, "Fog Min Value");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_fogParams.w, "Fog Max Value");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_fogColor.x, "Fog Color -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_fogColor.y, "Fog Color -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_fogColor.z, "Fog Color -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;

	t = new Label(subtitleDesc); t->setCaption(L"LIGHTING & MATERIALS");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	
	cb = new CheckBox(&r_enabledNormalMap, L"NormalMap: Enabled", L"NormalMap: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledParallaxMap, L"ParallaxMap: Enabled", L"ParallaxMap: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSpecular, L"Specular: Enabled", L"Specular: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledAlbedo, L"Albedo: Enabled", L"Albedo: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledCubeMap, L"CubeMap: Enabled", L"CubeMap: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSubsurfaceScattering, L"Subsurface Scattering: Enabled", L"Subsurface Scattering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledVegetationAnimation, L"Vegetation Animation: Enabled", L"Vegetation Animation: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"POSTPROCESS");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_dofEnabled , L"Depth Of Field: Enabled", L"Depth Of Field: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_dofEyeAdaptation , L"DOF Dynamic Eye Adaptation: Enabled", L"DOF Dynamic Eye Adaptation: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_dofBlurExponent, "DOF Blur Exponent");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_heatHazeEnabled , L"Heat & Haze: Enabled", L"Heat & Haze: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;	
	sb = new ScrollBar(width, height/2, 0, 100, 5, &r_heatHazeStrength, "Heat & Haze Strength");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 2, &r_heatHazeTimeScale, "Heat & Haze Time Scale");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_bloomEnabled , L"Bloom: Enabled", L"Bloom: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSSAO , L"SSAO: Enabled", L"SSAO: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSoftParticles , L"Soft Particles: Enabled", L"Soft Particles: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"WATER REFLECTION");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_enabledSkyReflection , L"Reflect Sky: Enabled", L"Reflect Sky: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledTerrainReflection , L"Reflect Terrain: Enabled", L"Reflect Terrain: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSurfacesReflection , L"Reflect Surfaces: Enabled", L"Reflect Surfaces: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledEntitiesReflection , L"Reflect Entities: Enabled", L"Reflect Entities: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledReflectionBlur , L"Reflection blur: Enabled", L"Reflection blur: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"CULLING");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;

	cb = new CheckBox(&r_deferredStencilOptimize, L"Deferred Stencil Optimize: Enabled", L"Deferred Stencil Optimize: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_deferredScissorOptimize, L"Deferred Scissor Optimize: Enabled", L"Deferred Scissor Optimize: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_deferredDebugComplexity, L"Deferred Debug Complexity: Enabled", L"Deferred Debug Complexity: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;	
	cb = new CheckBox(&r_cullingFrustum, L"Frustum Culling: Enabled", L"Frustum Culling: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_cullingOcclusion, L"Occlusion Culling: Enabled", L"Occlusion Culling: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_cullingLights, L"Culling lights: Enabled", L"Culling lights: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_renderSky, L"Sky rendering: Enabled", L"Sky rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_renderTerrain, L"Terrain rendering: Enabled", L"Terrain rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_renderSurfaces, L"Surfaces rendering: Enabled", L"Surfaces rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_renderEntities, L"Entities rendering: Enabled", L"Entities rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enableLODs, L"Surfaces Level of Detail: Enabled", L"Surfaces Level of Detail: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&g_terrainLodEnabled, L"Terrain Level of Detail: Enabled", L"Terrain Level of Detail: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"MISC.");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	cb = new CheckBox(&r_wireframe, L"Wireframe: Enabled", L"Wireframe: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&g_terrainDebugBB , L"Terrain Debug BB: Enabled", L"Terrain Debug BB: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&g_rendObjDebugBB , L"RendObj Debug BB: Enabled", L"RendObj Debug BB: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&g_rendOctreeDebugBB , L"Octree Debug BB: Enabled", L"Octree Debug BB: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"INSTANCING");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	cb = new CheckBox(&r_geometryInstancing , L"HW Geometry Instancing: Enabled", L"HW Geometry Instancing: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_packInstanceDataToFloat16 , L"Instance data: packed", L"Instance data: normal");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"SHADOWS [PREDEFINED]");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_disabled , L" Disable Shadows", L" Disable Shadows");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_sv , L" Shadow Volume", L" Shadow Volume");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_sm_d , L" SM D", L" SM D");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_sm_d_pcf3x3 , L" SM D + PCF3x3", L" SM D + PCF3x3");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_sm_d_pcf5x5 , L" SM D + PCF5x5", L" SM D + PCF5x5");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;
	cb = new CheckBox(&guiMenu_shadows_sm_d_pcf7x7 , L" SM D + PCF7x7", L" SM D + PCF7x7");
	cb->setXYPos(sep,pos);cb->setWidthHeight(width, height);wnd->addChildControl(cb);pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"SHADOWS ADVANCED");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	cb = new CheckBox(&r_enabledShadows , L"Shadows: Enabled", L"Shadows: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowVolume  , L"Shadow Volume: Enabled", L"Shadow Volume: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMapping , L"Shadow Mapping: Enabled", L"Shadow Mapping: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledVarianceShadowMapping , L"Variance Shadow Mapping: Enabled", L"Variance Shadow Mapping: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledBlurVarianceShadowMaps , L"Blur Variance Shadow Maps: Enabled", L"Blur Variance Shadow Maps: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledExponentialShadowMapping , L"Exponential Shadow Mapping: Enabled", L"Exponential Shadow Mapping: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledBoxFilterVarianceShadowMaps , L"Box Filter Variance Shadow Maps: Enabled", L"Box Filter Variance Shadow Maps: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMappingPCF , L"Shadow Mapping PCF: Enabled", L"Shadow Mapping PCF: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMappingPCFMed , L"Shadow Mapping PCF Med (5x5): Enabled", L"Shadow Mapping PCF Med (5x5): Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMappingPCFHigh , L"Shadow Mapping PCF High (7x7): Enabled", L"Shadow Mapping PCF High (7x7): Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMappingDeferred , L"Shadow Mapping Deferred: Enabled", L"Shadow Mapping Deferred: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledShadowMappingGeoRendering , L"Shadow Mapping Geometry Rendering: Enabled", L"Shadow Mapping Geometry Rendering: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	cb = new CheckBox(&r_enabledSMScreenSpaceBlur  , L"SM ScreenSpace Blur: Enabled", L"SM ScreenSpace Blur: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;

	t = new Label(subtitleDesc); t->setCaption(L"SUN & SKY");
	t->setXYPos(sep,pos); t->setWidthHeight(width, height); wnd->addChildControl(t); pos += height+sep;
	cb = new CheckBox(&r_sunColorEnabled , L"Sun Color Change: Enabled", L"Sun Color Change: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_sunColor.x, "Sun Color -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_sunColor.y, "Sun Color -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_sunColor.z, "Sun Color -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 10, &r_sunSizeExp, "r_sunSizeExp");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	cb = new CheckBox(&r_skyColorEnabled , L"Sky Color Change: Enabled", L"Sky Color Change: Disabled");
	cb->setXYPos(sep,pos); cb->setWidthHeight(width, height); wnd->addChildControl(cb); pos += height+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorFront.x, "Sky Color Front -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorFront.y, "Sky Color Front -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorFront.z, "Sky Color Front -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorBack.x, "Sky Color Back -> RED");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorBack.y, "Sky Color Back -> GREEN");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;
	sb = new ScrollBar(width, height/2, 0, 100, 1, &r_skyColorBack.z, "Sky Color Back -> BLUE");
	sb->setXYPos(sep,pos); wnd->addChildControl(sb); pos += height/2+sep;

	pos += height+sep;


	//u32 pos2 = 5;
	//height = 12;

	/*settWndCloseButton = new Button("Arial Black", 15);
	settWndCloseButton->setXYPos(width + 2*sep - 16,1);
	settWndCloseButton->setWidthHeight(15, 15);
	settWndCloseButton->setCaption(L"X");
	settWndCloseButton->setEnable(true);
	settWndCloseButton->setVisible(true);
	wnd->addChildControl(settWndCloseButton);*/

	wnd->setXYPos(gEngine.device->getWidth() - (width + 2*sep),0);
	wnd->setWidthHeight(width + 2*sep, pos + 10);
	gEngine.desktop->addChildControl(wnd);


	menuWnd = new WindowPopup();
	wnd = menuWnd;
	wnd->setEnable(false);
	wnd->setVisible(false);
	sep = 3;
	pos = 5;
	height = 15;
	width = 210;
	t = new Label("'Lucida Console' 14");
	t->setCaption(L"[ Main Menu ]");
	t->setXYPos(sep,pos);
	t->setWidthHeight(width, height);
	wnd->addChildControl(t);
	pos += height+sep;

	menuWndSettingsButton = new Button("'Arial Black' 15");
	menuWndSettingsButton->setXYPos(sep,pos);
	menuWndSettingsButton->setWidthHeight(width, height);
	menuWndSettingsButton->setCaption(L"Rendering Settings");
	menuWndSettingsButton->setEnable(false);
	menuWndSettingsButton->setVisible(false);
	menuWndSettingsButton->setPressed(true);
	wnd->addChildControl(menuWndSettingsButton);
	pos += height+sep;

	/*menuWndConsoleButton = new Button("Arial Black", 15);
	menuWndConsoleButton->setXYPos(sep,pos);
	menuWndConsoleButton->setWidthHeight(width, height);
	menuWndConsoleButton->setCaption(L"Settings");
	menuWndConsoleButton->setEnable(true);
	menuWndConsoleButton->setVisible(true);
	wnd->addChildControl(menuWndConsoleButton);
	pos += height+sep;*/

	wnd->setXYPos(0, gEngine.device->getHeight() - pos);
	wnd->setWidthHeight(width + 2*sep,pos);
	gEngine.desktop->addChildControl(wnd);

	
}

void clearallsh()
{
	guiMenu_shadows_disabled = 0;
	guiMenu_shadows_sv = 0;
	guiMenu_shadows_vsm = 0;
	guiMenu_shadows_vsm_blur = 0;
	guiMenu_shadows_vsm_boxfiler = 0;
	guiMenu_shadows_vsm_blur_boxfilter = 0;
	guiMenu_shadows_esm = 0;
	guiMenu_shadows_sm_g = 0;
	guiMenu_shadows_sm_g_pcf3x3 = 0;
	guiMenu_shadows_sm_g_pcf5x5 = 0;
	guiMenu_shadows_sm_g_pcf7x7 = 0;
	guiMenu_shadows_sm_g_blur = 0;
	guiMenu_shadows_sm_g_blur_pcf3x3 = 0;
	guiMenu_shadows_sm_g_blur_pcf5x5 = 0;
	guiMenu_shadows_sm_g_blur_pcf7x7 = 0;
	guiMenu_shadows_sm = 0;
	guiMenu_shadows_sm_pcf3x3 = 0;
	guiMenu_shadows_sm_pcf5x5 = 0;
	guiMenu_shadows_sm_pcf7x7 = 0;
	guiMenu_shadows_sm_blur = 0;
	guiMenu_shadows_sm_blur_pcf3x3 = 0;
	guiMenu_shadows_sm_blur_pcf5x5 = 0;
	guiMenu_shadows_sm_blur_pcf7x7 = 0;
	guiMenu_shadows_sm_d = 0;
	guiMenu_shadows_sm_d_pcf3x3 = 0;
	guiMenu_shadows_sm_d_pcf5x5 = 0;
	guiMenu_shadows_sm_d_pcf7x7 = 0;

	r_enabledVarianceShadowMapping = 0;
	r_enabledBlurVarianceShadowMaps = 0;
	r_enabledBoxFilterVarianceShadowMaps = 0;
	r_enabledExponentialShadowMapping = 0;
	r_enabledSMScreenSpaceBlur = 0;
	r_enabledShadowMapping = 0;
	r_enabledShadowMappingDeferred = 0;
	r_enabledShadowMappingGeoRendering = 0;
	r_enabledShadowMappingPCF = 0;
	r_enabledShadowMappingPCFHigh = 0;
	r_enabledShadowMappingPCFMed = 0;
	r_enabledShadowVolume = 0;
	r_enabledShadows = 0;
}

void GUIMenu::update()
{
	if(!settWnd)return;
	/*if(menuWndConsoleButton->isPressed())
	{
		menuWndConsoleButton->setPressed(false);
		gEngine.device->enableConsole(true);
	}*/

	if(r_hidestats && r_hidestats != 2)
	{
		settWnd->setEnable( false );
		settWnd->setVisible( false );
	}
	else
	{
		settWnd->setEnable( true );
		settWnd->setVisible( true );
	}

	/*if(menuWndSettingsButton->isPressed())
	{
		settWnd->setEnable( true );
		settWnd->setVisible( true );
	}
	else
	{
		settWnd->setEnable( false );
		settWnd->setVisible( false );
	}*/

	if(settWnd->getEnable())
	{
		if(guiMenu_shadows_disabled)
		{
			gEngine.renderer->addTxtF("Shadows disabled");
			clearallsh();
		}
		if(guiMenu_shadows_sv)
		{
			gEngine.renderer->addTxtF("Shadow Volumes enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowVolume = 1;
		}
		if(guiMenu_shadows_vsm)
		{
			gEngine.renderer->addTxtF("Variance Shadow Mapping enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledVarianceShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
		}
		if(guiMenu_shadows_vsm_blur)
		{
			gEngine.renderer->addTxtF("Variance Shadow Mapping with SM blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledVarianceShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
			r_enabledBlurVarianceShadowMaps = 1;
		}
		if(guiMenu_shadows_vsm_boxfiler)
		{
			gEngine.renderer->addTxtF("Variance Shadow Mapping with box filter enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledVarianceShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
			r_enabledBoxFilterVarianceShadowMaps = 1;
		}
		if(guiMenu_shadows_vsm_blur_boxfilter)
		{
			gEngine.renderer->addTxtF("Variance Shadow Mapping with SM blur and box filter enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledVarianceShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
			r_enabledBlurVarianceShadowMaps = 1;
			r_enabledBoxFilterVarianceShadowMaps = 1;
		}
		if(guiMenu_shadows_esm)
		{
			gEngine.renderer->addTxtF("Exponential Shadow Mapping enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledExponentialShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
		}
		if(guiMenu_shadows_sm_g)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;

		}
		if(guiMenu_shadows_sm_g_pcf3x3)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 3x3 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
		}
		if(guiMenu_shadows_sm_g_pcf5x5)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 5x5 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFMed = 1;
		}
		if(guiMenu_shadows_sm_g_pcf7x7)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 7x7 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFHigh = 1;
		}
		if(guiMenu_shadows_sm_g_blur)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_g_blur_pcf3x3)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 3x3 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_g_blur_pcf5x5)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 5x5 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFMed = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_g_blur_pcf7x7)
		{
			gEngine.renderer->addTxtF("Shadow Mapping G with PCF 7x7 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingGeoRendering = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFHigh = 1;
			r_enabledSMScreenSpaceBlur = 1;		
		}
		if(guiMenu_shadows_sm)
		{
			gEngine.renderer->addTxtF("Shadow Mapping enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
		}
		if(guiMenu_shadows_sm_pcf3x3)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 3x3 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
		}
		if(guiMenu_shadows_sm_pcf5x5)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 5x5 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFMed = 1;
		}
		if(guiMenu_shadows_sm_pcf7x7)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 7x7 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFHigh = 1;
		}
		if(guiMenu_shadows_sm_blur)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_blur_pcf3x3)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 3x3 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_blur_pcf5x5)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 5x5 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFMed = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_blur_pcf7x7)
		{
			gEngine.renderer->addTxtF("Shadow Mapping with PCF 7x7 and SS blur enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFHigh = 1;
			r_enabledSMScreenSpaceBlur = 1;
		}
		if(guiMenu_shadows_sm_d)
		{
			gEngine.renderer->addTxtF("Shadow Mapping D enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingDeferred = 1;
		}
		if(guiMenu_shadows_sm_d_pcf3x3)
		{
			gEngine.renderer->addTxtF("Shadow Mapping D with PCF 3x3 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingDeferred = 1;	
		}
		if(guiMenu_shadows_sm_d_pcf5x5)
		{
			gEngine.renderer->addTxtF("Shadow Mapping D with PCF 5x5 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFMed = 1;
			r_enabledShadowMappingDeferred = 1;
		}
		if(guiMenu_shadows_sm_d_pcf7x7)
		{
			gEngine.renderer->addTxtF("Shadow Mapping D with PCF 7x7 enabled");
			clearallsh();
			r_enabledShadows = 1;
			r_enabledShadowMapping = 1;
			r_enabledShadowMappingPCF = 1;
			r_enabledShadowMappingPCFHigh = 1;
			r_enabledShadowMappingDeferred = 1;
		}

		/*settWndCloseButton->setEnable(true);
		settWndCloseButton->setVisible(true);

		if(settWndCloseButton->isPressed())
		{
			settWndCloseButton->setPressed(false);
			menuWndSettingsButton->setPressed(false);
			settWnd->setEnable( false );
			settWnd->setVisible( false );
		}*/
	}
}
