#pragma once
#include "Plugin.h"

//PACK FORMAT
typedef struct FSFileEntryPACK
{
	FSFileEntryPACK() : offset(0), size(0) { }
	string	archName;
	u32		offset;
	u32		size;
}*FSFileEntryPACK_ptr;

typedef map<string, FSFileEntryPACK>	FSFilePACKMap;
typedef FSFilePACKMap::iterator			FSFilePACKMapIt;

typedef struct FSArchivePACK : IFSArchive
{
	u32 open(string fileName);
	u32 exists(string fileName);
	u32 load(string fileName, Buffer &buffer);

protected:
	FSFilePACKMap files;
}*FSArchivePACK_ptr;

typedef struct FSArchMgrPACK : IFSArchiveMgr
{
	u32			create(string archiveName, vector<FSPakFileInfo> &files);
	u32			canLoad(string archiveName);
	IFSArchive* load(string archiveName);
	string		getType();
}*FSArchMgrPACK_ptr;

//XPAK FORMAT
typedef struct FSFileEntryXPAK
{
	FSFileEntryXPAK() : offset(0), size(0), uncompSize(0), compType(0), canLoad(0) { }
	string	fileName;
	string	packedFileName;
	u32		offset;
	u32		size;
	u32		uncompSize;
	u8		compType;
	u64		mdate;
	Buffer	buffer;
	u32		canLoad;
}*FSFileEntryXPAK_ptr;

typedef map<string, FSFileEntryXPAK>	FSFileXPAKMap;
typedef FSFileXPAKMap::iterator			FSFileXPAKMapIt;

typedef struct FSArchiveXPAK : IFSArchive
{
	FSArchiveXPAK() : mustUpdate(0) { }
	~FSArchiveXPAK();

	u32 open(string fileName);
	u32 exists(string fileName);
	u32 load(string fileName, Buffer &buffer);
	u32 add(FSPakFileInfo &file);
	u64	getModDate(string fileName);
	u32 update();
	u32	canUpdate() { return 1; }

protected:
	string			archName;
	u32				mustUpdate;
	FSFileXPAKMap	files;
}*FSArchiveXPAK_ptr;

typedef struct FSArchMgrXPAK : IFSArchiveMgr
{
	u32			create(string archiveName, vector<FSPakFileInfo> &files);
	u32			canLoad(string archiveName);
	IFSArchive* load(string archiveName);
	string		getType();
	u32			canUpdate() { return 1; }
}*FSArchMgrXPAK_ptr;

//////////////////////////////////////////////////////////////////////////

static u32 createPACK(string archiveName, vector<FSPakFileInfo> &files)
{
	FSArchMgrPACK pack;
	return pack.create(archiveName, files);
}

static u32 createXPAK(string archiveName, vector<FSPakFileInfo> &files)
{
	FSArchMgrXPAK xpak;
	return xpak.create(archiveName, files);
}