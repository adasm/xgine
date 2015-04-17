/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

class XGINE_API BaseControl
{		
private:
protected:
	Vector2 m_Position;
	DWORD m_Width;
	DWORD m_Height;
	bool m_Enable;
	bool m_Visible;
	BaseControl* m_ChildControls;
	BaseControl* m_NextSibling;
	BaseControl* m_PreviousSibling;
	BaseControl* m_Parent;

	bool m_Focus;
	bool m_Flashed;
	bool m_drawFrameIfFocus;
	bool m_drawShadow;
	BaseControl* m_FocusControl;
public:
	BaseControl() { m_ChildControls = m_NextSibling = m_PreviousSibling = m_Parent = NULL; m_FocusControl=NULL; m_Focus=false; m_Enable=true; m_Visible=true;}

	bool			getVisible() {return m_Visible;}
	void			setVisible(bool Visible) {m_Visible = Visible;}

	bool			getEnable() {return m_Enable;}
	void			setEnable(bool Enable) {m_Enable = Enable;}

	DWORD			getWidth() {return m_Width;}
	DWORD			getHeight() {return m_Height;}
	void			setWidth(DWORD Width) {m_Width = Width;}
	void			setHeight(DWORD Height) {m_Height = Height;}
	void			setWidthHeight(DWORD Width, DWORD Height) { m_Width = Width; m_Height = Height; }

	Vector2*	getPosition() {return &m_Position;}
	void			getAbsolutePosition(Vector2* Position);

	float			getXPos() {return m_Position.x;}
	float			getYPos() {return m_Position.y;}

	void setXPos(FLOAT X) 
	{
		//if(X<0)X=0;
		//BaseControl *pa = getParentControl();
		//if(pa&&X+this->getWidth()>pa->getWidth())X=(float)pa->getWidth()-this->getWidth();
		m_Position.x = X;
	}
	void setYPos(FLOAT Y)
	{
		//if(Y<0)Y=0;
		//BaseControl *pa = getParentControl();
		//if(pa&&Y+this->getHeight()>pa->getHeight())Y=(float)pa->getHeight()-this->getHeight();
		m_Position.y = Y;
	}

	void			setXYPos(FLOAT X, FLOAT Y) { setXPos(X); setYPos(Y);}


	BaseControl*	getParentControl() {return m_Parent;}
	void			setParentControl(BaseControl* Control) {m_Parent = Control;}
	BaseControl*	getNextSibling() {return m_NextSibling;}
	void			setNextSibling(BaseControl* Control) {m_NextSibling = Control;}
	BaseControl*	getPreviousSibling() {return m_PreviousSibling;}
	void			setPreviousSibling(BaseControl* Control) {m_PreviousSibling = Control;}
	BaseControl*	getFirstChild() {return m_ChildControls;}
	void			setFirstChild(BaseControl* Control) {m_ChildControls = Control;}

	BaseControl*	addChildControl(BaseControl* Control);
	BaseControl*	removeChildControl(BaseControl* Control);
	void			removeAllChildren();
	int				getChildCount();
	
	BaseControl*	getFocus() {return m_FocusControl;}
	void			setFocus(BaseControl* Control);

	virtual void	onMouseDown(int Button, int X, int Y) = NULL;
	virtual void	onMouseMove(int X, int Y) = NULL;
	virtual void	onMouseUp(int Button, int X, int Y) = NULL;
	virtual void	onKeyDown(WPARAM Key, LPARAM Extended) = NULL;
	virtual void	onKeyUp(WPARAM Key, LPARAM Extended) = NULL;
	virtual void	onChar(WPARAM Key, LPARAM Extended) = NULL;
	virtual bool	onRender(void) = NULL;

	void			onLostFocus();

	bool			isFocused() { return m_Focus; }
	
	bool			cursorIntersect(FLOAT X, FLOAT Y);
	bool			cursorIntersectL(FLOAT X, FLOAT Y);

	bool			postMessage(UINT msg, WPARAM wParam, LPARAM lParam, void* Data);
	BaseControl*	postToAll(UINT msg, WPARAM wParam, LPARAM lParam, void* Data);
	BaseControl*	postToAllReverse(BaseControl* Control, UINT msg, WPARAM wParam, LPARAM lParam, void* Data);

	void			moveToFront(BaseControl* Control);

	void			setFlashed(bool b) { m_Flashed = b; }

	virtual	void	minimized(BaseControl* Control) { }
	virtual	void	maximized(BaseControl* Control) { }

	void setDrawFrameIfFocus(bool b) { m_drawFrameIfFocus = b; }
	void setDrawShadow(bool b) { m_drawShadow = b; }
};

class XGINE_API Label : public BaseControl
{
private:
protected:
	std::wstring m_Caption;
	Font* m_Font;
	DWORD m_Color;
	DWORD m_TextColor;
	DWORD m_Format;
	bool drawRect;
public:
	Label(std::string fontDesc);
	~Label();

	bool onRender();
	void onMouseDown(int Button, int X, int Y){	};
	void onMouseMove(int X, int Y){	};
	void onMouseUp(int Button, int X, int Y){	};
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended){	};
	void onKeyUp(WPARAM Key, LPARAM Extended){	};
	void onChar(WPARAM Key, LPARAM Extended) { };
	void setCaption(const std::wstring & Caption) { m_Caption = Caption; }
	std::wstring getCaption() const {return m_Caption;}
	void	setColor(DWORD Color) {m_Color = Color;}
	void	setTextColor(DWORD Color) {m_TextColor = Color;}
	void	setFormat(DWORD Format) {m_Format = Format;}
	void	setdrawRect(bool b){ drawRect = b; }
	RECT	getRenderedRect();
};

class XGINE_API Desktop : public BaseControl
{
protected:
	std::list< BaseControl*> minimizedmap;

public:
	Desktop();
	~Desktop();

	void init();
	bool m_Visible;
	bool onRender();
	void onMouseDown(int Button, int X, int Y){ }
	void onMouseMove(int X, int Y){ }
	void onMouseUp(int Button, int X, int Y){ }
	void onSetFocus(){ }
	void onLostFocus(){ }
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	void minimized(BaseControl* Control);
	void maximized(BaseControl* Control);
};

class XGINE_API Window : public BaseControl
{
protected:
	bool IsParentWindow; //Does this class represent a child or parent window?
	bool m_IsMouseDown;
	int m_LastNormalPosX;
	int m_LastNormalPosY;
	int m_LastNormalWidth;
	int m_LastNormalHeight;
	bool m_Minimized;
	float m_X, m_Y;
	bool m_Depressed;
	Label *m_Caption;
	bool m_Moveable;

	DWORD m_ColorCaptionActive;
	DWORD m_ColorCaptionInactive;
	DWORD m_ColorCaptionTextActive;
	DWORD m_ColorCaptionTextInactive;
	DWORD m_ColorWindowActive;
	DWORD m_ColorWindowActiveInactive;
	DWORD m_ColorWindowRectActive;
	DWORD m_ColorWindowRectInactive;
public:
	Window();
	~Window();

	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y);
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	void setCaption(const std::wstring&  Caption);
	void minimize();
	void restore(void);

	void setColorCaptionActive(DWORD Color) {m_ColorCaptionActive = Color;}
	void setColorCaptionInactive(DWORD Color) {m_ColorCaptionInactive = Color;}
	void setColorCaptionTextActive(DWORD Color) {m_ColorCaptionTextActive = Color;}
	void setColorCaptionTextInactive(DWORD Color) {m_ColorCaptionTextInactive = Color;}
	void setColorWindowActive(DWORD Color) {m_ColorWindowActive = Color;}
	void setColorWindowActiveInactive(DWORD Color) {m_ColorWindowActiveInactive = Color;}
	void setColorWindowRectActive(DWORD Color) {m_ColorWindowRectActive = Color;}
	void setColorWindowRectInactive(DWORD Color) {m_ColorWindowRectInactive = Color;}
};

class XGINE_API WindowPopup : public BaseControl
{
protected:
	DWORD m_ColorWindowActive;
	DWORD m_ColorWindowInactive;
	float m_X, m_Y;
	bool m_Depressed;
	bool m_Moveable;
	bool m_Sliding;

	u32	 startTime;
	int lastX, lastY;
public:
	WindowPopup();
	~WindowPopup();

	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y);
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };

	void setColorWindowActive(DWORD Color) {m_ColorWindowActive = Color;}
	void setColorWindowInactive(DWORD Color) {m_ColorWindowInactive = Color;}
};

class XGINE_API Button : public BaseControl
{
protected:
	Label* m_Caption;
	bool m_Pressed;
	DWORD m_ColorPressedActive;
	DWORD m_ColorPressedInactive;
	DWORD m_ColorNormalActive;
	DWORD m_ColorNormalInactive;
	DWORD m_ColorTextActive;
	DWORD m_ColorTextInactive;
public:
	Button(std::string fontDesc);
	~Button();

	bool isPressed() { return m_Pressed; }
	void setPressed(bool b) { m_Pressed = b; }
	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y) { };
	void onSetFocus() { };
	void onLostFocus() { };
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	void setCaption(const std::wstring& Caption);

	void setColorPressedActive(DWORD Color) {m_ColorPressedActive = Color;}
	void setColorPressedInactive(DWORD Color) {m_ColorPressedInactive = Color;}
	void setColorNormalActive(DWORD Color) {m_ColorNormalActive = Color;}
	void setColorNormalInactive(DWORD Color) {m_ColorNormalInactive = Color;}
	void setColorTextActive(DWORD Color) {m_ColorTextActive = Color;}
	void setColorTextInactive(DWORD Color) {m_ColorTextInactive = Color;}
};

class XGINE_API TextBox : public BaseControl
{
protected:
	std::wstring m_Text; //Actual text of the text box
	Font* m_Font; //Interface to draw text
	DWORD m_Format;
	Vector2 m_CaretVector[2]; //Top and bottom points of the line
	bool m_CaretVisible; //Is line visible?
	long CaretCharPos; //Index of caret
	FLOAT m_TextWidth;
	bool accepted;
	bool highlight;

	DWORD m_ColorActive;
	DWORD m_ColorInactive;
	DWORD m_ColorTextActive;
	DWORD m_ColorTextInactive;
	DWORD m_ColorCaretActive;
	DWORD m_ColorCaretInactive;

public:
	TextBox();
	~TextBox();

	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y) { };
	void onSetFocus();
	void onLostFocus();
	virtual void onKeyDown(WPARAM Key, LPARAM Extended);
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended);
	std::wstring getText() const {return m_Text;}
	void setText(const std::wstring& Text);
	long getCharAtPos(int X, int Y);
	bool cursorIntersectChar(int X, int Y);
	f32 getStringWidth(std::wstring String);
	f32 getStringHeight(std::wstring String);
	long getCaretPos() const {return CaretCharPos;}
	void setCaretPos(long Pos);
	bool insertText(const std::wstring& Text);
	long removeText(long Quantity);
	bool isAccepted() { return accepted; }
	void setAccepted(bool a) { accepted = a; }
	void setHighlight(bool g) { highlight = g; }
	
	void setColorActive(DWORD Color) {m_ColorActive = Color;}
	void setColorInactive(DWORD Color) {m_ColorInactive = Color;}
	void setColorTextActive(DWORD Color) {m_ColorTextActive = Color;}
	void setColorTextInactive(DWORD Color) {m_ColorTextInactive = Color;}
	void setColorCaretActive(DWORD Color) {m_ColorCaretActive = Color;}
	void setColorCaretInactive(DWORD Color) {m_ColorCaretInactive = Color;}
};

class XGINE_API TextBoxConsoleInput : public TextBox
{
public:
	TextBoxConsoleInput() { }
	~TextBoxConsoleInput() { }

	void onKeyDown(WPARAM Key, LPARAM Extended);
};

class XGINE_API TextConsoleOutput : public BaseControl
{
private:
protected:
	Font* m_Font;
	float lookup_y;
	float max_lookup_y;
	unsigned int last_size;
public:
	TextConsoleOutput();
	~TextConsoleOutput();

	bool onRender();
	void onMouseDown(int Button, int X, int Y) { };
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y) { };
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	void addToLookUpY(float y) { lookup_y += y; lookup_y = (lookup_y<=0)?0:lookup_y; if(lookup_y>max_lookup_y)lookup_y=max_lookup_y;}

	float	m_stampFade;
};

struct XGINE_API GUIMenu
{
	GUIMenu() : menuWnd(0), settWnd(0), menuWndSettingsButton(0), menuWndConsoleButton(0), settWndCloseButton(0) { }
	void init();
	void update();

protected:
	WindowPopup *menuWnd;
	WindowPopup *settWnd;
	Button *menuWndSettingsButton, *menuWndConsoleButton, *settWndCloseButton;
};



/*
class XGINE_API ProfilerOutput : public BaseControl
{
private:
protected:
	LPD3DXFONT m_Font;
public:
	ProfilerOutput();
	~ProfilerOutput();

	bool onRender();
	void onMouseDown(int Button, int X, int Y) { };
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y) { };
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
};
*/

class XGINE_API ScrollBar : public BaseControl
{
private:
protected:
	int m_Min;
	int m_Max;
	int m_ThumbPosition;
	int m_Change;
	FLOAT m_BackgroundHeight;
	bool m_DragMode;
	Vector2 m_ThumbPos;
	Vector2 m_ThumbScale;
	FLOAT m_ThumbHeight;
	FLOAT autoSizeThumb();
	FLOAT autoSizeBackground();
	void updateScaling();
	f32	*m_var;
	string varName;
	f32 varMul;
public:
	ScrollBar(u32 width, u32 height, i32 Min, i32 Max, f32 Mul, f32 *var = 0, string name = "");
	~ScrollBar();

	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y);
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	void setValue(int Value);
	int  getValue() {return m_ThumbPosition;}
	int  getPosValue(FLOAT Pos);
	void setMinMax(int Min, int Max);
	void setChange(int Change) {m_Change = Change;}
};


class XGINE_API CheckBox : public BaseControl
{
protected:
	bool m_Checked;
	Button* m_TickBox;
	Label* m_Label;
	u32 *m_variable;
	wstring m_strEnabled, m_strDisabled;

public:
	CheckBox(u32 *var, const std::wstring& strEnabled, const std::wstring& strDisabled);
	~CheckBox();

	bool getCheckedState() const {return m_Checked;}
	void setCheckedState(bool State) {m_Checked = State;}
	void setCaption(const wstring& Text);
	bool onRender();
	void onMouseDown(int Button, int X, int Y);
	void onMouseMove(int X, int Y);
	void onMouseUp(int Button, int X, int Y) { };
	void onSetFocus();
	void onLostFocus();
	void onKeyDown(WPARAM Key, LPARAM Extended) { };
	void onKeyUp(WPARAM Key, LPARAM Extended) { };
	void onChar(WPARAM Key, LPARAM Extended) { };
	Button* GetTickBoxControl() { return m_TickBox; }
	Label* GetLabelControl() { return m_Label; }
};