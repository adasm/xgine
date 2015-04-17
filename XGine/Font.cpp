#include "Font.h"

Font::Font()
{
	lpFont = 0;
}

Font::~Font()
{
	release();
}


void Font::release()
{
	if(lpFont)lpFont->Release();
	lpFont = 0;
}

u32 Font::load()
{
	release();

	string	name;
	string	sizeStr;
	u32		size;
	Tokenizer tokenizer(strFilename);
	tokenizer.nextToken(&name);
	tokenizer.nextToken(&sizeStr);

	std::stringstream conv; 
	conv.unsetf(std::ios::skipws);
	conv << sizeStr;
	conv >> size;

	if(FAILED(D3DXCreateFont(gEngine.device->getDev(),size,0,FW_NORMAL,0,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLEARTYPE_QUALITY,DEFAULT_PITCH | FF_DONTCARE,name.c_str(),&lpFont)))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Font", "Couldn't create font '%s'", strFilename.c_str());
		return 0;
	}

	gEngine.kernel->log->prnEx(LT_SUCCESS, "Font", "Created font '%s'.", strFilename.c_str());
	return 1;
}