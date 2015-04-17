#include "FileSystem.h"

//PACK FORMAT
typedef struct PACK_HEADER
{
	c8		signature[4]; // "PACK"
	i32		dirOffset;
	i32		dirLength;
}*PACK_HEADER_ptr;

typedef struct PACK_FILE
{
	c8		fileName[56];
	i32		position;
	i32		length;
}*PACK_FILE_ptr;

u32	FSArchivePACK::open(string fileName)
{
	PACK_HEADER	pakHeader;
	PACK_FILE	pakFile;
	FSFileEntryPACK fileEntry;
	std::ifstream file;

	prepFileName(&fileName[0]);

	file.open(fileName.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Couldn't open file '%s'.", fileName.c_str());
		return 0;
	}

	file.seekg(0,std::ios::beg);
	file.read((c8*)&pakHeader, sizeof(PACK_HEADER));

	if(memcmp(pakHeader.signature, "PACK", 4) != 0)
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Wrong signature of archive PACK '%s'.", fileName.c_str());
		return 0;
	}

	if(pakHeader.dirLength%64 != 0)
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Wrong length of directory in archive PACK '%s'.", fileName.c_str());
		return 0;
	}
		           
	file.seekg(pakHeader.dirOffset,std::ios::beg);
		           		
	u32 count = pakHeader.dirLength/64;
	u32 TotalDataSize = pakHeader.dirOffset - sizeof(PACK_HEADER);

	for(u32 i = 0; i < count; i++)
	{				
		file.read((c8*)&pakFile, sizeof(PACK_FILE));

		prepFileName(pakFile.fileName);
		fileEntry.archName = fileName;
		fileEntry.offset = pakFile.position;
		fileEntry.size = pakFile.length;
		files.insert(std::make_pair(pakFile.fileName, fileEntry));

		plugEngine->kernel->log->prnEx(LT_INFO, "FSArchivePACK", "Archive '%s' contains file '%s', %u bytes", fileName.c_str(), pakFile.fileName, pakFile.length);
	}   
	
	file.close();

	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchivePACK", "Added archive PACK '%s' (%u files, %u bytes).", fileName.c_str(), count, TotalDataSize);

	return 1;
}

u32 FSArchivePACK::exists(string fileName)
{
	return files.find(fileName) != files.end();
}

u32 FSArchivePACK::load(string fileName, Buffer &buffer)
{
	FSFilePACKMapIt it = files.find(fileName);
	if(it == files.end())return 0;
	FSFileEntryPACK fileEntry = it->second;

	plugEngine->kernel->mem->freeBuff(buffer);
	std::ifstream file;
	file.open(fileEntry.archName.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Couldn't open archive PACK '%s' with file '%s'", fileEntry.archName.c_str(), fileName.c_str());
		return 0;
	}

	if(!plugEngine->kernel->mem->allocBuff(buffer, fileEntry.size, FILELINE))
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Couldn't load file '%s'. Not enough memory.", fileName.c_str());
		return 0;
	}

	file.seekg(fileEntry.offset);
	file.read(buffer.data, buffer.size);
	file.close();
	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchivePACK", "Loaded buffer '%s' from archive PACK '%s'", fileName.c_str(), fileEntry.archName.c_str());
	return 1;
}


u32 FSArchMgrPACK::create(string archiveName, vector<FSPakFileInfo> &files)
{
	if(files.size() < 1)
		return 1;

	PACK_HEADER			pakHeader;
	vector<PACK_FILE>	pakFiles;
	ofstream			archive;
	u32	i, dirpos;

	prepFileName(&archiveName[0]);
	for(i=0; i<files.size();i++)
		prepFileName(&files[i].fileName[0]);

	archive.open(archiveName.c_str(), std::ios::binary | std::ios::trunc);
	if(!archive.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Couldn't open file '%s' to write. Archive PACK has not been created.", archiveName.c_str());
		return 0;
	}

	pakFiles.resize(files.size());

	archive.seekp(sizeof(PACK_HEADER), std::ios::beg);
	dirpos = sizeof(PACK_HEADER);

	//FILES DATA
	Buffer buffer;
	for(i = 0; i < files.size(); i++)
	{
		if(!gEngine.kernel->fs->load(files[i].fileName, buffer))
		{
			archive.close();
			plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Archive PACK '%s' has not been created.", archiveName.c_str());
			return 0;
		}

		strcpy_s(pakFiles[i].fileName, 56, files[i].packedFileName.c_str());
		pakFiles[i].position = archive.tellp();
		pakFiles[i].length = buffer.size;

		archive.write(buffer.data, buffer.size);
		dirpos += buffer.size;
		plugEngine->kernel->mem->freeBuff(buffer);
	}

	if(dirpos != archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Error writing files data to archive PACK '%s'. Archive PACK has not been created.", archiveName.c_str());
		return 0;
	}

	//DIRECTORY
	for(i = 0; i < files.size(); i++)
	{
		archive.write((c8*)&pakFiles[i], sizeof(PACK_FILE));
	}

	if((dirpos + (files.size() * sizeof(PACK_FILE)))!= archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchivePACK", "Error writing directory to archive PACK '%s'. Archive PACK has not been created.", archiveName.c_str());
		return 0;
	}

	//HEADER
	memcpy(pakHeader.signature, "PACK", 4);
	pakHeader.dirOffset = dirpos;
	pakHeader.dirLength = (files.size() * sizeof(PACK_FILE));
	archive.seekp(0, std::ios::beg);
	archive.write((c8*)&pakHeader, sizeof(PACK_HEADER));
	archive.close();

	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchivePACK", "Archive PACK '%s' with %u files has been created.", archiveName.c_str(), files.size());
	return 1;	
}

u32 FSArchMgrPACK::canLoad(string fileName)
{
	PACK_HEADER	pakHeader;
	std::ifstream file;

	prepFileName(&fileName[0]);

	file.open(fileName.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		file.close();
		return 0;
	}

	file.seekg(0,std::ios::beg);
	file.read((c8*)&pakHeader, sizeof(PACK_HEADER));

	if(memcmp(pakHeader.signature, "PACK", 4) != 0)
	{
		file.close();
		return 0;
	}

	if(pakHeader.dirLength%64 != 0)
	{
		file.close();
		return 0;
	}
	
	file.close();
	return 1;
}

IFSArchive* FSArchMgrPACK::load(string archiveName)
{
	FSArchivePACK *arch = new FSArchivePACK();
	if(arch->open(archiveName))
	{
		return arch;
	}
	delete(arch);
	return 0;
}

string FSArchMgrPACK::getType()
{
	return "PACK";
}

//XPAK FORMAT

#define XPAK_VERSION 0x00002000

typedef struct XPAK_HEADER
{
	c8		signature[4]; // "XPAK"
	u32		version; //Must be XPAK_VERSION
	u32		dirOffset;
	u32		dirLength;
}*XPAK_HEADER_ptr;

typedef struct XPAK_FILE
{
	c8		fileName[139];
	u32		position;
	u32		size;
	u32		uncompSize;
	u8		compType;
	u64		mdate;
}*XPAK_FILE_ptr;


FSArchiveXPAK::~FSArchiveXPAK() 
{ 
	if(mustUpdate)update(); 

	for(FSFileXPAKMapIt it = files.begin(); it != files.end(); it++)
	{
		plugEngine->kernel->mem->freeBuff(it->second.buffer);
	}
	files.clear();
}

u32	FSArchiveXPAK::open(string fileName)
{
	XPAK_HEADER			pakHeader;
	XPAK_FILE			pakFile;
	FSFileEntryXPAK		fileEntry;

	prepFileName(&fileName[0]);

	archName = fileName;

	if(!plugEngine->kernel->fs->exists(fileName, ST_DISK))
	{
		std::ofstream fout;
		fout.open(fileName.c_str(), std::ios::binary | std::ios::trunc);
		if(!fout.is_open())return 0;
		fout.close();
		DeleteFile(fileName.c_str());
		return 1;
	}

	std::ifstream file;
	file.open(fileName.c_str(), std::ios::binary);
	if(!file.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't open archive '%s'.", fileName.c_str());
		return 0;
	}

	file.seekg(0,std::ios::beg);
	file.read((c8*)&pakHeader, sizeof(XPAK_HEADER));

	if(memcmp(pakHeader.signature, "XPAK", 4) != 0)
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Wrong signature of archive XPAK '%s'.", fileName.c_str());
		return 0;
	}

	if(pakHeader.version != XPAK_VERSION)
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Wrong version of archive XPAK '%s'.", fileName.c_str());
		return 0;
	}

	if(pakHeader.dirLength%sizeof(XPAK_FILE) != 0)
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Wrong length of directory in archive XPAK '%s'.", fileName.c_str());
		return 0;
	}
		           
	file.seekg(pakHeader.dirOffset,std::ios::beg);
		           		
	u32 count = pakHeader.dirLength/sizeof(XPAK_FILE);
	u32 TotalDataSize = pakHeader.dirOffset - sizeof(XPAK_HEADER);

	for(u32 i = 0; i < count; i++)
	{				
		file.read((c8*)&pakFile, sizeof(XPAK_FILE));

		prepFileName(pakFile.fileName);

		fileEntry.fileName = pakFile.fileName;
		fileEntry.packedFileName = pakFile.fileName;
		fileEntry.offset = pakFile.position;
		fileEntry.size = pakFile.size;
		fileEntry.uncompSize = pakFile.uncompSize;
		fileEntry.compType = pakFile.compType;
		fileEntry.mdate = pakFile.mdate;
		fileEntry.canLoad = 1;
		files.insert(std::make_pair(pakFile.fileName, fileEntry));

		plugEngine->kernel->log->prnEx(LT_INFO, "FSArchiveXPAK", "Archive '%s' contains file '%s',  compressed: %u bytes, uncompressed: %u, method: %s ", fileName.c_str(), pakFile.fileName, pakFile.size, pakFile.uncompSize, Compression::getName(pakFile.compType));
	}   
	
	file.close();

	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Added archive XPAK '%s' (%u files, %u bytes).", fileName.c_str(), count, TotalDataSize);

	return 1;
}

u32 FSArchiveXPAK::exists(string fileName)
{
	prepFileName(fileName);
	FSFileXPAKMapIt it = files.find(fileName);
	if(it != files.end())
		if(it->second.canLoad || (it->second.buffer.data != 0 && it->second.buffer.size != 0))return 1;
	return 0;
}

u32 FSArchiveXPAK::load(string fileName, Buffer &buffer)
{
	FSFileXPAKMapIt it = files.find(fileName);
	if(it == files.end())return 0;
	FSFileEntryXPAK fileEntry = it->second;

	plugEngine->kernel->mem->freeBuff(buffer);

	if(fileEntry.canLoad)
	{
		std::ifstream file;
		file.open(archName.c_str(), std::ios::binary);
		if(!file.is_open())
		{
			plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't open archive XPAK '%s' with file '%s'.", archName.c_str(), fileName.c_str());
			return 0;
		}
		
		if(fileEntry.compType == COMP_NONE)
		{
			if(!plugEngine->kernel->mem->allocBuff(buffer, fileEntry.size, FILELINE))
			{
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't load file '%s'. Not enough memory.", fileName.c_str());
				return 0;
			}
			file.seekg(fileEntry.offset);
			file.read(buffer.data, buffer.size);
			file.close();
		}
		else
		{
			Buffer compressed;
			if(!plugEngine->kernel->mem->allocBuff(compressed, fileEntry.size, FILELINE))
			{
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't load file '%s'. Not enough memory.", fileName.c_str());
				return 0;
			}
			file.seekg(fileEntry.offset);
			file.read(compressed.data, compressed.size);
			file.close();

			if(!Compression::decompBuff(fileEntry.compType, compressed, buffer, fileEntry.uncompSize))
			{
				plugEngine->kernel->mem->freeBuff(compressed);
				plugEngine->kernel->mem->freeBuff(buffer);
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't load file '%s'. Decompression failed.", fileName.c_str());
				return 0;
			}
			plugEngine->kernel->mem->freeBuff(compressed);
		}
	
		plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Loaded buffer '%s' from archive XPAK '%s'.", fileName.c_str(), archName.c_str());
		return 1;
	}
	else if(fileEntry.buffer.data != 0 && fileEntry.buffer.size)
	{
		plugEngine->kernel->mem->allocBuff(buffer, fileEntry.buffer.size, FILELINE);
		memcpy(buffer.data, fileEntry.buffer.data, fileEntry.buffer.size);

		plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Loaded buffer '%s' from archive XPAK '%s'.", fileName.c_str(), archName.c_str());
		return 1;
	}
	else
	{
		plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Couldn't load buffer '%s' from archive XPAK '%s'.", fileName.c_str(), archName.c_str());
		return 0;
	}
}
u32 FSArchiveXPAK::add(FSPakFileInfo &file)
{
	prepFileName(file.fileName);
	prepFileName(file.packedFileName);

	FSFileXPAKMapIt it = files.find(file.fileName);
	if(it != files.end())
	{
		if(it->second.mdate >= file.mdate)
		{
			plugEngine->kernel->log->prnEx(LT_DEBUG, "FSArchiveXPAK", "Update file '%s' in archive '%s' rejected.", file.fileName.c_str(), archName.c_str());
			return 1;
		}

		mustUpdate = 1;
		it->second.fileName = file.fileName;
		it->second.packedFileName = file.packedFileName;
		it->second.offset = 0;
		it->second.size = 0;
		it->second.uncompSize = 0;
		it->second.compType = file.compType;
		it->second.mdate = file.mdate;
		if(file.buffer)
		{
			plugEngine->kernel->mem->freeBuff(it->second.buffer);
			plugEngine->kernel->mem->allocBuff(it->second.buffer, file.buffer->size, FILELINE);
			memcpy(it->second.buffer.data, file.buffer->data, it->second.buffer.size);
		}
		else
		{
			plugEngine->kernel->mem->freeBuff(it->second.buffer);
		}

		plugEngine->kernel->log->prnEx(LT_DEBUG, "FSArchiveXPAK", "Updated file '%s' in archive '%s'.", file.fileName.c_str(), archName.c_str());
	}
	else
	{
		mustUpdate = 1;

		FSFileEntryXPAK entry;
		entry.fileName = file.fileName;
		entry.packedFileName = file.packedFileName;
		entry.offset = 0;
		entry.size = 0;
		entry.uncompSize = 0;
		entry.compType = file.compType;
		entry.mdate = file.mdate;
		if(file.buffer)
		{
			plugEngine->kernel->mem->allocBuff(entry.buffer, file.buffer->size, FILELINE);
			memcpy(entry.buffer.data, file.buffer->data, entry.buffer.size);
		}
		else
		{
			plugEngine->kernel->mem->freeBuff(entry.buffer);
		}

		files.insert( make_pair( file.fileName, entry ) );

		plugEngine->kernel->log->prnEx(LT_DEBUG, "FSArchiveXPAK", "Added file '%s' to archive '%s'.", file.fileName.c_str(), archName.c_str());
	}
	
	return 1;
}

u64	FSArchiveXPAK::getModDate(string fileName)
{
	prepFileName(fileName);
	FSFileXPAKMapIt it = files.find(fileName);
	if(it == files.end())return 0;
	return it->second.mdate;
}

u32 FSArchiveXPAK::update()
{
	if(files.size() < 1 || !mustUpdate)
		return 1;
	
	for(FSFileXPAKMapIt it = files.begin(); it != files.end(); it++)
	{
		if(!it->second.buffer.data || !it->second.buffer.size)
		{
			if(exists(it->second.fileName))
			{
				if(!load(it->second.fileName, it->second.buffer))
				{
					plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't pre load file '%s' from archive '%s'. Archive XPAK has not been created.", it->second.fileName.c_str(), archName.c_str());
					return 0;
				}
			}
		}
	}

	mustUpdate = 0;
	XPAK_HEADER			pakHeader;
	vector<XPAK_FILE>	pakFiles;
	ofstream			archive;
	ofstream			fileList;
	u32	i, dirpos;

	for(FSFileXPAKMapIt it = files.begin(); it != files.end(); it++)
		prepFileName(&it->second.fileName[0]);

	archive.open(archName.c_str(), std::ios::binary | std::ios::trunc);
	if(!archive.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't open file '%s' to write. Archive XPAK has not been created.", archName.c_str());
		return 0;
	}

	pakFiles.resize(files.size());

	archive.seekp(sizeof(XPAK_HEADER), std::ios::beg);
	dirpos = sizeof(XPAK_HEADER);

	//FILES DATA
	Buffer uncompressed, compressed;
	i = 0;
	for(FSFileXPAKMapIt it = files.begin(); it != files.end(); it++, i++)
	{
		if(it->second.buffer.data && it->second.buffer.size)
		{
			uncompressed.data = it->second.buffer.data; it->second.buffer.data = 0;
			uncompressed.size = it->second.buffer.size; it->second.buffer.size = 0;
			pakFiles[i].mdate = it->second.mdate;
		}
		else 
		{
			if(!plugEngine->kernel->fs->load(it->second.fileName, uncompressed, ST_DISK))
			{
				archive.close();
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't load file buffer %s", archName.c_str(), it->second.fileName.c_str());
				return 0;
			}

			pakFiles[i].mdate = plugEngine->kernel->fs->getModDate(it->second.fileName, ST_DISK);
		}

		strcpy_s(pakFiles[i].fileName, 115, it->second.packedFileName.c_str());
		pakFiles[i].position = archive.tellp();
		
		if(it->second.compType == COMP_NONE)
		{
			pakFiles[i].size = uncompressed.size;
			pakFiles[i].uncompSize = uncompressed.size;
			pakFiles[i].compType = COMP_NONE;
			archive.write(uncompressed.data, uncompressed.size);
		}
		else if(it->second.compType == COMP_BESTBCL)
		{
			u32 bestSize = uncompressed.size;
			it->second.compType = COMP_NONE;

			for(u8 type = 0x01; type < 0xB; type++)
			{
				if(!Compression::compBuff(type, uncompressed, compressed))
				{
					archive.close();
					plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archName.c_str());
					return 0;
				}
				if(compressed.size < bestSize)
				{ 
					bestSize = compressed.size; 
					it->second.compType = type; 
				}
				plugEngine->kernel->mem->freeBuff(compressed);
			}

			if(it->second.compType != COMP_NONE)
			{
				if(!Compression::compBuff(it->second.compType, uncompressed, compressed))
				{
					archive.close();
					plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archName.c_str());
					return 0;
				}

				pakFiles[i].size = compressed.size;
				pakFiles[i].uncompSize = uncompressed.size;
				pakFiles[i].compType = it->second.compType;
				archive.write(compressed.data, compressed.size);
			}
			else
			{
				pakFiles[i].size = uncompressed.size;
				pakFiles[i].uncompSize = uncompressed.size;
				pakFiles[i].compType = COMP_NONE;
				archive.write(uncompressed.data, uncompressed.size);
			}
		}
		else
		{
			if(!Compression::compBuff(it->second.compType, uncompressed, compressed))
			{
				archive.close();
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archName.c_str());
				return 0;
			}

			pakFiles[i].size = compressed.size;
			pakFiles[i].uncompSize = uncompressed.size;
			pakFiles[i].compType = it->second.compType;
			archive.write(compressed.data, compressed.size);
		}

		dirpos += pakFiles[i].size;	
		plugEngine->kernel->mem->freeBuff(uncompressed);
		plugEngine->kernel->mem->freeBuff(compressed);
	}
	fileList.close();

	if(dirpos != archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Error writing files data to archive XPAK '%s'. Archive XPAK has not been created.", archName.c_str());
		return 0;
	}

	//DIRECTORY
	for(i = 0; i < files.size(); i++)
	{
		archive.write((c8*)&pakFiles[i], sizeof(XPAK_FILE));
	}

	if((dirpos + (files.size() * sizeof(XPAK_FILE)))!= archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Error writing directory to archive XPAK '%s'. Archive XPAK has not been created.", archName.c_str());
		return false;
	}

	//HEADER
	memcpy(pakHeader.signature, "XPAK", 4);
	pakHeader.version = XPAK_VERSION;
	pakHeader.dirOffset = dirpos;
	pakHeader.dirLength = (files.size() * sizeof(XPAK_FILE));
	archive.seekp(0, std::ios::beg);
	archive.write((c8*)&pakHeader, sizeof(XPAK_HEADER));
	archive.close();

	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Archive XPAK '%s' with %u files has been created.", archName.c_str(), files.size());
	return 1;
}

u32 FSArchMgrXPAK::create(string archiveName, vector<FSPakFileInfo> &files)
{
	if(files.size() < 1)
		return 1;

	XPAK_HEADER			pakHeader;
	vector<XPAK_FILE>	pakFiles;
	ofstream			archive;
	ofstream			fileList;
	u32	i, dirpos;

	prepFileName(&archiveName[0]);
	for(i=0; i<files.size();i++)
		prepFileName(&files[i].fileName[0]);

	archive.open(archiveName.c_str(), std::ios::binary | std::ios::trunc);
	if(!archive.is_open())
	{
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't open file '%s' to write. Archive XPAK has not been created.", archiveName.c_str());
		return 0;
	}

	pakFiles.resize(files.size());

	archive.seekp(sizeof(XPAK_HEADER), std::ios::beg);
	dirpos = sizeof(XPAK_HEADER);

	//FILES DATA
	fileList.open((archiveName+"_.txt").c_str(), ios::trunc);
	Buffer uncompressed, compressed;
	for(i = 0; i < files.size(); i++)
	{
		if(!plugEngine->kernel->fs->load(files[i].fileName, uncompressed, ST_DISK))
		{
			archive.close();
			plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't load file buffer %s", archiveName.c_str(), files[i].fileName.c_str());
			return 0;
		}
		
		pakFiles[i].mdate = plugEngine->kernel->fs->getModDate(files[i].fileName, ST_DISK);

		strcpy_s(pakFiles[i].fileName, 115, files[i].packedFileName.c_str());
		pakFiles[i].position = archive.tellp();
		
		if(files[i].compType == COMP_NONE)
		{
			pakFiles[i].size = uncompressed.size;
			pakFiles[i].uncompSize = uncompressed.size;
			pakFiles[i].compType = COMP_NONE;
			archive.write(uncompressed.data, uncompressed.size);
		}
		else if(files[i].compType == COMP_BESTBCL)
		{
			u32 bestSize = uncompressed.size;
			files[i].compType = COMP_NONE;

			for(u8 type = 0x01; type < 0xB; type++)
			{
				if(!Compression::compBuff(type, uncompressed, compressed))
				{
					archive.close();
					plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archiveName.c_str());
					return 0;
				}
				if(compressed.size < bestSize)
				{ 
					bestSize = compressed.size; 
					files[i].compType = type; 
				}
				plugEngine->kernel->mem->freeBuff(compressed);
			}

			if(files[i].compType != COMP_NONE)
			{
				if(!Compression::compBuff(files[i].compType, uncompressed, compressed))
				{
					archive.close();
					plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archiveName.c_str());
					return 0;
				}

				pakFiles[i].size = compressed.size;
				pakFiles[i].uncompSize = uncompressed.size;
				pakFiles[i].compType = files[i].compType;
				archive.write(compressed.data, compressed.size);
			}
			else
			{
				pakFiles[i].size = uncompressed.size;
				pakFiles[i].uncompSize = uncompressed.size;
				pakFiles[i].compType = COMP_NONE;
				archive.write(uncompressed.data, uncompressed.size);
			}
		}
		else
		{
			if(!Compression::compBuff(files[i].compType, uncompressed, compressed))
			{
				archive.close();
				plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Archive XPAK '%s' has not been created. Couldn't compress buffer.", archiveName.c_str());
				return 0;
			}

			pakFiles[i].size = compressed.size;
			pakFiles[i].uncompSize = uncompressed.size;
			pakFiles[i].compType = files[i].compType;
			archive.write(compressed.data, compressed.size);
		}

		dirpos += pakFiles[i].size;	
		plugEngine->kernel->mem->freeBuff(uncompressed);
		plugEngine->kernel->mem->freeBuff(compressed);

		fileList << files[i].fileName << endl;
	}
	fileList.close();

	if(dirpos != archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Error writing files data to archive XPAK '%s'. Archive XPAK has not been created.", archiveName.c_str());
		return 0;
	}

	//DIRECTORY
	for(i = 0; i < files.size(); i++)
	{
		archive.write((c8*)&pakFiles[i], sizeof(XPAK_FILE));
	}

	if((dirpos + (files.size() * sizeof(XPAK_FILE)))!= archive.tellp())
	{
		archive.close();
		plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Error writing directory to archive XPAK '%s'. Archive XPAK has not been created.", archiveName.c_str());
		return false;
	}

	//HEADER
	memcpy(pakHeader.signature, "XPAK", 4);
	pakHeader.version = XPAK_VERSION;
	pakHeader.dirOffset = dirpos;
	pakHeader.dirLength = (files.size() * sizeof(XPAK_FILE));
	archive.seekp(0, std::ios::beg);
	archive.write((c8*)&pakHeader, sizeof(XPAK_HEADER));
	archive.close();

	plugEngine->kernel->log->prnEx(LT_SUCCESS, "FSArchiveXPAK", "Archive XPAK '%s' with %u files has been created.", archiveName.c_str(), files.size());
	return 1;	
}

u32 FSArchMgrXPAK::canLoad(string fileName)
{
	XPAK_HEADER	pakHeader;
	XPAK_FILE	pakFile;
	prepFileName(&fileName[0]);

	if(plugEngine->kernel->fs->exists(fileName))
	{
		std::ifstream file;
		file.open(fileName.c_str(), std::ios::binary);
		if(!file.is_open())
		{
			plugEngine->kernel->log->prnEx(LT_ERROR, "FSArchiveXPAK", "Couldn't open file '%s'.", fileName.c_str());
			return 0;
		}

		file.seekg(0,std::ios::beg);
		file.read((c8*)&pakHeader, sizeof(XPAK_HEADER));

		if(memcmp(pakHeader.signature, "XPAK", 4) != 0)
		{
			file.close();
			return 0;
		}

		if(pakHeader.version != XPAK_VERSION)
		{
			file.close();
			return 0;
		}

		if(pakHeader.dirLength%sizeof(XPAK_FILE) != 0)
		{
			file.close();
			return 0;
		}
		
		file.close();
		return 1;
	}
	else
	{
		return 1;
	}
}

IFSArchive* FSArchMgrXPAK::load(string archiveName)
{
	FSArchiveXPAK *arch = new FSArchiveXPAK();
	if(arch->open(archiveName))
	{
		return arch;
	}
	delete(arch);
	return 0;
}

string FSArchMgrXPAK::getType()
{
	return "XPAK";
}

//////////////////////////////////////////////////////////////////////////////////////////////////