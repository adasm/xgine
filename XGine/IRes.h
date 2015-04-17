#pragma once

struct ResMgr;
struct Buffer;

struct XGINE_API IRes
{
protected:
	friend	ResMgr;


	u32		iRef;
	string	strFilename;
	
	IRes() : buffer(0) {  }
	virtual ~IRes() { }
	virtual u32 load() { return false; }

	//background loading
	virtual string getFolder() { return ""; }
	virtual string getCache() { return "data/cache.xpak"; }
	virtual u32	   canBeLoadBackground() = 0;
	virtual	u32    mustLoadBuffer() = 0;
	Buffer	*buffer;

public:

	const string& getResFilename() { return strFilename; }
};