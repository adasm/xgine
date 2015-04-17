/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "Kernel.h"

//_stat
#include <sys/types.h>
#include <sys/stat.h>

//COMPRESSION
#include "bcl/rle.h"
#include "bcl/shannonfano.h"
#include "bcl/huffman.h"
#include "bcl/rice.h"
#include "bcl/lz.h"
#include "zlib/include/zconf.h"
#include "zlib/include/zlib.h"
#pragma comment(lib, "zlib/lib/zdll.lib")

/*** Common Functions ***/
c8* getDate()
{
	static c8 date[10];
	_strdate_s(date, 10);
	return date;
}

c8* getTime()
{
	static c8 time[15];
	_strtime_s(time, 15);
	return time;
}

void prepFileName(c8* fileName)
{
	c8 *c = fileName - 1;
	while(*(++c))
		if((*c) == '\\')(*c) = '/';
		else (*c) = (c8)tolower((i32)(*c));
}

void prepFileName(string &fileName)
{
	string temp = fileName;
	fileName = "";
	for(u32 i = 0; i < temp.size(); i++)
		if(temp[i] == '\\')fileName+='/';
		else fileName += tolower((i32)temp[i]);
}

std::string getExt(const std::string& fileName)
{
	return fileName.substr(fileName.find_last_of("."));
}

std::string	getWithoutExt(const std::string& fileName)
{
	return fileName.substr(0, fileName.find_last_of("."));
}

std::string convstr(const std::wstring& str)
{
	static char _str[1024];
	WideCharToMultiByte(CP_ACP, 0, str.c_str(), -1, _str, 1024, 0, 0);
	return std::string(_str);
}

std::wstring convstr(const std::string& str)
{
	static wchar_t _str[1024];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, _str, 1024);
	return std::wstring(_str);
}

/*** Logger ***/
u32 Logger::init(const c8 *fileName)
{ 
	file.open(fileName, std::ios::trunc);
	if(!file.is_open())return 0;
	file << "<?xml version=\"1.0\" encoding=\"utf-8\" standalone=\"yes\"?> <!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\"> <html><head> <title>XGine log file</title> <meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"/> <style type=\"text/css\"> body { background: #1a242a; color: #b4c8d2; margin-right: 20px; margin-left: 20px; font-size: 14px; font-family: Arial, sans-serif, sans; } a { text-decoration: none; } a:link { color: #b4c8d2; } a:active { color: #ff9900; } a:visited { color: #b4c8d2; } a:hover { color: #ff9900; } h1 { text-align: center; } h2 { color: #ffffff; } .message, .success .warning, .error, .debug, .fatal, .message { background-color: #080c10; color: #b4c8d2; padding: 3px; } .success { background-color: #080c10; color: #339933; padding: 3px; } .warning { background-color: #839ca7; color: #1a242a; padding: 3px; } .error { background-color: #ff9933; color: #1a242a; padding: 3px; } .debug { background-color: #080c10; color: #2255dd; padding: 3px; } .fatal { background-color: #ffffff; color: #ff5522; padding: 3px; }</style></head><body>" << endl;
	file << "<h1>XGine - Next-gen 3D Graphics Engine log file</h1>" << endl;
	file << "<h3 align=\"center\">" << XGINE_VER << "</h4>";
	file << "<h4 align=\"center\">App name: " << g_appName.c_str() << "</h4>";
	prnEx(LT_SUCCESS, "Kernel", "Log file opened at %s %s", getTime(), getDate());
	return 1;
}

void Logger::close()
{
	prnEx(LT_SUCCESS, "Kernel", "Log file closed at %s", getTime());
	file<<"</body"<<endl;
	if(file.is_open())
		file.close();
}

void Logger::prn(LogType type, c8* module, c8 *str)
{
	if(file.is_open())
	{
		switch(type)
		{
			case LT_INFO:{
				file << "\n<div class=\"message\">" ;
				break;}
			case LT_SUCCESS:{
				file << "\n<div class=\"success\">" ;
				break;}
			case LT_WARNING:{
				file << "\n<div class=\"warning\">" ;
				break;}
			case LT_ERROR:{
				file << "\n<div class=\"error\">" ;
				break;}
			case LT_DEBUG:{
				file << "\n<div class=\"debug\">" ;
				break;}
			case LT_FATAL:{
				file << "\n<div class=\"fatal\">" ;
				break;}
			default:{
				file << "\n<div class=\"message\">" ;
				break;}
		}

		file << "<b>["<< getTime() << "]</b> <b>[" << module << "]</b> " << str;
		file << "</div>"<< std::flush;
	}
	gEngine.kernel->con->prn(str);
}

void Logger::prnEx(LogType type, c8* module, c8 *str, ...)
{
	static c8 buffer[4096];

	va_list args; 
	va_start(args,str);
	vsprintf_s(buffer,4096,str,args);

	if(file.is_open())
	{
		switch(type)
		{
			case LT_INFO:{
				file << "\n<div class=\"message\">" ;
				break;}
			case LT_SUCCESS:{
				file << "\n<div class=\"success\">" ;
				break;}
			case LT_WARNING:{
				file << "\n<div class=\"warning\">" ;
				break;}
			case LT_ERROR:{
				file << "\n<div class=\"error\">" ;
				break;}
			case LT_DEBUG:{
				file << "\n<div class=\"debug\">" ;
				break;}
			case LT_FATAL:{
				file << "\n<div class=\"fatal\">" ;
				break;}
			default:{
				file << "\n<div class=\"message\">" ;
				break;}
		}

		file << "<b>["<< getTime() << "]</b> <b>[" << module << "]</b> " << buffer;
		file << "</div>"<< std::flush;
	}

	gEngine.kernel->con->prn(buffer);
}

/*** FileSystem ***/

void fsRemoveCurrentDirPath(string &fileName)
{
	static char cd[512];
	GetCurrentDirectory(511, cd);
	string currentDir = cd;
	if(_strcmpi(fileName.substr(0, currentDir.size()).c_str(), currentDir.c_str()) == 0)
		fileName = fileName.substr(((currentDir.size())+1));
}

void createPath(string path)
{
	for(u32 i=0; i<path.size(); i++)
		if(path[i] == '/' || path[i] == '\\')
			if(CreateDirectory(path.substr(0,i).c_str(),NULL) != TRUE)return;                    
}

void confunc_fs(const vector<string> &arg)
{
	if(arg.size() < 3)
	{
		gEngine.kernel->con->prn("Example usage: fs uf/x archName/x");
		return;
	}

	if(arg[1] == "uf")
	{
		gEngine.kernel->fs->dumpUsedFilesToPack(arg[2], "XGine XPAK");
	}
	else
	{
		gEngine.kernel->con->prn("Example usage: fs func args");
		return;
	}
	
}

u32 FileSystem::init()
{
	return 1;
}

void FileSystem::close()
{
	for(FSArchiveMapIt it = archives.begin(); it != archives.end(); it++)
	{
		delete(it->second);
	}
	archives.clear();

	for(u32 i = 0; i < archMgrs.size(); i++)
	{
		delete(archMgrs[i]);
		archMgrs[i] = 0;
	}
	archMgrs.clear();
}

u32 FileSystem::createArchive(string archiveName, string archiveType, vector<FSPakFileInfo> &files)
{
	for(u32 i = 0; i < archMgrs.size(); i++)
	{
		if(archMgrs[i]->getType() == archiveType)
			return archMgrs[i]->create(archiveName, files);
	}
	return 0;
}

u32	FileSystem::addSrc(string fileName)
{
	FSArchiveMapIt it = archives.find(fileName);
	if(it != archives.end()) return 1;

	for(u32 i = 0; i < archMgrs.size(); i++)
	{
		if(archMgrs[i]->canLoad(fileName))
		{
			IFSArchive *arch = archMgrs[i]->load(fileName);
			if(arch)
			{
				archives.insert( make_pair(fileName, arch) );
				if(g_logvfs)
					gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Added archive '%s' (%s)", fileName.c_str(), archMgrs[i]->getType().c_str());
				return 1;
			}
		}
	}
	gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Couldn't add archive '%s'", fileName.c_str());
	return 0;
}

u32	FileSystem::addSrc(string fileName, string archiveType)
{
	FSArchiveMapIt it = archives.find(fileName);
	if(it != archives.end()) return 1;

	for(u32 i = 0; i < archMgrs.size(); i++)
	{
		if(archMgrs[i]->getType() == archiveType)
		{
			if(!archMgrs[i]->canLoad(fileName))
				break;

			IFSArchive *arch = archMgrs[i]->load(fileName);
			if(arch)
			{
				archives.insert( make_pair(fileName, arch) );
				if(g_logvfs)
					gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Added archive '%s' (%s)", fileName.c_str(), archMgrs[i]->getType().c_str());
				return 1;
			}
		}
	}
	gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Couldn't add archive '%s'", fileName.c_str());
	return 0;
}

u32	FileSystem::load(string fileName, Buffer &buffer, SearchType searchType)
{
	prepFileName(fileName);

	//PREPARE
	loadedFiles.insert(fileName);

	if(searchType & ST_DISK)
	{
		std::ifstream file;
		file.open(fileName.c_str(), std::ios::binary);
		if(file.is_open())
		{
			file.seekg(0, std::ios::end);
			u32 size = file.tellg();
			file.seekg(0);
			
			gEngine.kernel->mem->freeBuff(buffer);
			if(!gEngine.kernel->mem->allocBuff(buffer, size, FILELINE))return 0;

			file.read(buffer.data, buffer.size);
			file.close();
			if(g_logvfs)
					gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Loaded buffer '%s' from file", fileName.c_str());
			return 1;
		}
	}

	if(searchType & ST_ARCHIVE)
	{
		for(FSArchiveMapIt it = archives.begin(); it != archives.end(); it++)
		{
			if(it->second->exists(fileName))
				return it->second->load(fileName, buffer);
		}
	}

	gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Couldn't load file %s", fileName.c_str());
	return 0;
}

u32	FileSystem::loadCached(string fileName, Buffer &buffer, string cacheName)
{
	prepFileName(fileName);
	addSrc(cacheName, "XPAK");
	if(getModDate(fileName, ST_ARCHIVE) >= getModDate(fileName, ST_DISK))
	{
		return load(fileName, buffer, ST_ARCHIVE);
	}
	else if(exists(fileName, ST_DISK))
	{
		//update archive
		FSPakFileInfo file;
		file.compType = COMP_ZLIB;
		file.packedFileName = file.fileName = fileName;
		addToArchive(cacheName, file);

		return load(fileName, buffer, ST_DISK);
	}
	else if(exists(fileName, ST_ARCHIVE))
	{
		return load(fileName, buffer, ST_ARCHIVE);
	}
	else
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Couldn't load file %s", fileName.c_str());
		return 0;
	}
}

u32	FileSystem::exists(string fileName, SearchType searchType)
{
	prepFileName(&fileName[0]);

	if(searchType & ST_DISK)
	{
		std::ifstream file;
		file.open(fileName.c_str(), std::ios::binary);
		if(file.is_open())
		{
			file.close();
			return 1;
		}
	}

	if(searchType & ST_ARCHIVE)
	{
		for(FSArchiveMapIt it = archives.begin(); it != archives.end(); it++)
			if(it->second->exists(fileName))
				return 1;
	}

	return 0;
}


u64	FileSystem::getModDate(string fileName, SearchType searchType)
{
	if(searchType & ST_DISK)
	{
		if(GetFileAttributes(fileName.c_str()) != INVALID_FILE_ATTRIBUTES )
		{
			struct _stat fileStats;
			_stat(fileName.c_str(), &fileStats);
			return fileStats.st_mtime;
		}
	}

	if(searchType & ST_ARCHIVE)
	{
		for(FSArchiveMapIt it = archives.begin(); it != archives.end(); it++)
			if(it->second->exists(fileName))
				return it->second->getModDate(fileName);
	}

	return 0;
}

u32	FileSystem::addToArchive(string archiveName, FSPakFileInfo &file, string archiveType)
{
	FSArchiveMapIt it = archives.find(archiveName);
	if(it != archives.end())
	{
		it->second->add(file);
	}
	else
	{
		if(archiveType.size()) addSrc(archiveName, archiveType);
		else addSrc(archiveName);

		it = archives.find(archiveName);
		if(it != archives.end())
		{
			it->second->add(file);
		}
	}
	return 0;
}

u32 FileSystem::updateArchive(string archiveName)
{
	FSArchiveMapIt it = archives.find(archiveName);
	if(it != archives.end())
	{
		it->second->update();
	}
	return 0;
}

u32 FileSystem::findFile(string fileName, string &filePath, string dir)
{ 
	WIN32_FIND_DATA wfd;
	BOOL IsFile = TRUE; 
	HANDLE hFile;
	c8 currentDir[512], path[512];
	GetCurrentDirectory(511, currentDir);
	if(dir.size()>0)SetCurrentDirectory(dir.c_str()); 
	hFile = FindFirstFile("*.*", &wfd);
	while(IsFile)
	{ 
		GetFullPathName(wfd.cFileName, 511, path, NULL);
		if(_strcmpi(wfd.cFileName, fileName.c_str())==0)
		{
			filePath = path;
			return 1;
		}
		if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (wfd.cFileName[0] != '.'))
			if(findFile(fileName, filePath, path))return 1;
		IsFile = FindNextFile(hFile, &wfd);
	}
	SetCurrentDirectory(currentDir);
	return 0;
}

void FileSystem::listAllFiles(vector<string> &fileList, string dir)
{ 
	static int canRemoveCurrentDirPath = 0;
	WIN32_FIND_DATA wfd;
	BOOL IsFile = TRUE; 
	HANDLE hFile;
	c8 currentDir[512], path[512];
	GetCurrentDirectory(511, currentDir);
	if(dir.size()>0)SetCurrentDirectory(dir.c_str()); 
	hFile = FindFirstFile("*.*", &wfd);
	while(IsFile)
	{
		GetFullPathName(wfd.cFileName, 511, path, NULL);
		if((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && (wfd.cFileName[0] != '.'))
		{
			canRemoveCurrentDirPath++;
			listAllFiles(fileList, path);
			canRemoveCurrentDirPath--;
		}
		else if(exists(path))fileList.push_back(path);
		IsFile = FindNextFile(hFile, &wfd);
	}
	SetCurrentDirectory(currentDir);

	if(canRemoveCurrentDirPath == 0)
	{
		for(u32 i=0; i<fileList.size(); i++)
		{
			string currentDirStr = currentDir;
			if(_strcmpi(fileList[i].substr(0, currentDirStr.size()).c_str(), currentDirStr.c_str()) == 0)
				fileList[i] = fileList[i].substr(((currentDirStr.size())+1));
		}
	}
}

u32 FileSystem::writeBuffToFile(string fileName, Buffer &buffer)
{
	if(!buffer.size || !buffer.data)return 0;

	ofstream file;
	file.open(fileName.c_str(), ios::trunc | ios::binary);
	if(file.is_open())
	{
		file.write(buffer.data, buffer.size);
		file.close();
		return 1;
	}
	return 0;
}

u32 FileSystem::dumpUsedFilesToPack(string archiveName, string archiveType, u8 compType)
{
	vector<FSPakFileInfo> files;
	FSPakFileInfo temp;

	FSUsedFilesSetIt it;
	for(it = loadedFiles.begin(); it != loadedFiles.end(); it++)
	{
		if(exists(*it))
		{
			temp.fileName = *it;
			temp.packedFileName = temp.fileName;
			temp.compType = compType;
			files.push_back(temp);
		}
	}

	return createArchive(archiveName, archiveType, files);
}

c8* Compression::getName(u8 method)
{
	switch(method)
	{
		case COMP_NONE:		return "No compression";
		case COMP_RLE:		return "RLE compression";
		case COMP_HUFF:		return "Huffman compression";
		case COMP_RICE8:	return "Rice 8-bit compression";
		case COMP_RICE16:	return "Rice 16-bit compression";
		case COMP_RICE32:	return "Rice 32-bit compression";
		case COMP_RICE8S:	return "Rice 8-bit signed compression";
		case COMP_RICE16S:	return "Rice 16-bit signed compression";
		case COMP_RICE32S:	return "Rice 32-bit signed compression";
		case COMP_LZ:		return "LZ77 compression";
		case COMP_SF:		return "Shannon-Fano compression";
		case COMP_BESTBCL:	return "Best BCL compression for current buffer";
		case COMP_ZLIBS:	return "ZLib best-speed compression";
		case COMP_ZLIBC:	return "ZLib best-compression compression";
		case COMP_ZLIB:		return "ZLib default compression";
		default:			return "Unknown compression";
	}
}

u32 Compression::getMaxCompressedSize(u8 method, u32 insize)
{
	switch(method)
	{
		case COMP_NONE:		return insize;
		case COMP_RLE:		return (u32)( ( (double)insize * 257.0/ 256.0 ) + 1 );
		case COMP_HUFF:		return (u32)( ( (double)insize * 101.0/ 100.0 ) + 320 );
		case COMP_RICE8:
		case COMP_RICE16:
		case COMP_RICE32:
		case COMP_RICE8S:
		case COMP_RICE16S:
		case COMP_RICE32S:	return insize + 1;
		case COMP_LZ:		return (u32)( ( (double)insize * 257.0/ 256.0 ) + 1 );
		case COMP_SF:		return (u32)( ( (double)insize * 101.0/ 100.0 ) + 384 );
		case COMP_ZLIBS:	
		case COMP_ZLIBC:	
		case COMP_ZLIB:		return (u32)( ( (double)insize * 1.5 ) + 12 );
		default:			return 0;
	}
}

u32 Compression::comp(u8 method, u8 *in, u8 *out, u32 insize, u32 *outsize)
{
	switch(method)
	{
		case COMP_RLE:
			(*outsize) = 0;
			(*outsize) = RLE_Compress(in, out, insize);
			break;
		case COMP_HUFF:
			(*outsize) = 0;
			(*outsize) = Huffman_Compress(in, out, insize);
			break;
		case COMP_RICE8:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_UINT8);
			break;
		case COMP_RICE16:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_UINT16);
			break;
		case COMP_RICE32:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_UINT32);
			break;
		case COMP_RICE8S:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_INT8);
			break;
		case COMP_RICE16S:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_INT16);
			break;
		case COMP_RICE32S:
			(*outsize) = 0;
			(*outsize) = Rice_Compress(in, out, insize, RICE_FMT_INT32);
			break;
		case COMP_LZ:
			{
				(*outsize) = 0;
				u32 *work = (u32*)malloc(sizeof(unsigned int) * (65536+insize));
				if(work)
				{
					(*outsize) = LZ_CompressFast(in, out, insize, work);
					free(work);
				}
				else
				{
					(*outsize) = LZ_Compress(in, out, insize);
				}
			}break;
		case COMP_SF:
			(*outsize) = 0;
			(*outsize) = SF_Compress(in, out, insize);
			break;
		case COMP_ZLIBS:
			{
				int result = compress2((Bytef*)out, (uLongf*)outsize, (const Bytef*)in, insize, Z_BEST_SPEED);
				if(result == Z_MEM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_MEM_ERROR");
					return 0;
				}
				else if(result == Z_BUF_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_BUF_ERROR");
					return 0;
				}
				else if(result == Z_STREAM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_STREAM_ERROR");
					return 0;
				}
			}break;
		case COMP_ZLIBC:
			{
				int result = compress2((Bytef*)out, (uLongf*)outsize, (const Bytef*)in, insize, Z_BEST_COMPRESSION);
				if(result == Z_MEM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_MEM_ERROR");
					return 0;
				}
				else if(result == Z_BUF_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_BUF_ERROR");
					return 0;
				}
				else if(result == Z_STREAM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_STREAM_ERROR");
					return 0;
				}
			}break;
		case COMP_ZLIB:
			{
				int result = compress2((Bytef*)out, (uLongf*)outsize, (const Bytef*)in, insize, Z_DEFAULT_COMPRESSION);
				if(result == Z_MEM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_MEM_ERROR");
					return 0;
				}
				else if(result == Z_BUF_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_BUF_ERROR");
					return 0;
				}
				else if(result == Z_STREAM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Z_STREAM_ERROR");
					return 0;
				}
			}break;
		default:
			gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Compressing data failed - Unknown compression method");
			return 0;
	}

	if(g_logker)
		gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Compressed data [%s] (insize: %u, outsize: %u)", Compression::getName(method), insize, *outsize);
	return ((*outsize) > 0);
}

void Compression::decomp(u8 method, u8 *in, u8 *out, u32 insize, u32 outsize)
{
	switch(method)
	{
		//case COMP_NONE:
		case COMP_RLE:
			RLE_Uncompress(in, out, insize);
			break;
		case COMP_HUFF:
			Huffman_Uncompress(in, out, insize, outsize);
			break;
		case COMP_RICE8:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_UINT8);
			break;
		case COMP_RICE16:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_UINT16);
			break;
		case COMP_RICE32:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_UINT32);
			break;
		case COMP_RICE8S:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_INT8);
			break;
		case COMP_RICE16S:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_INT16);
			break;
		case COMP_RICE32S:
			Rice_Uncompress(in, out, insize, outsize, RICE_FMT_INT32);
			break;
		case COMP_LZ:
			LZ_Uncompress(in, out, insize);
			break;
		case COMP_SF:
			SF_Uncompress(in, out, insize, outsize);
			break;
		case COMP_ZLIBS:
		case COMP_ZLIBC:
		case COMP_ZLIB:
			{
				u32 outsizezlib;
				int result = uncompress((Bytef*)out, (uLongf*)&outsizezlib, (Bytef*)in, insize);
				if(result == Z_MEM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Decompressing data failed - Z_MEM_ERROR");
					return;
				}
				else if(result == Z_BUF_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Decompressing data failed - Z_BUF_ERROR");
					return;
				}
				else if(result == Z_STREAM_ERROR)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Decompressing data failed - Z_STREAM_ERROR");
					return;
				}
				else if(outsizezlib != outsize)
				{
					gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Decompressing data failed - outsizezlib != outsize");
					return;
				}
			}break;
		default:
			gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Decompressing data failed - Unknown compression method");
			return;
	}
	if(g_logker)
		gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Uncompressed data [%s] (in: %u, out: %u)", Compression::getName(method), insize, outsize);
}

u32 Compression::compBuff(u8 method, Buffer &in, Buffer &out)
{
	if(!in.size || !in.data)
	{
		gEngine.kernel->log->prn(LT_ERROR, "Kernel", "Couln't decompress buffer. Invalid data.");
		return 0;
	}
	if(out.size || out.data) gEngine.kernel->mem->freeBuff(out);
	if(!gEngine.kernel->mem->allocBuff(out, Compression::getMaxCompressedSize(method, in.size), FILELINE))
	{
		gEngine.kernel->log->prn(LT_ERROR, "Kernel", "Couln't decompress buffer. Not enough memory for output buffer.");
		return 0;
	}
	return comp(method, (u8*)in.data, (u8*)out.data, in.size, &out.size);
}

u32 Compression::decompBuff(u8 method, Buffer &in, Buffer &out, u32 outsize)
{
	if(!in.size || !in.data)
	{
		gEngine.kernel->log->prn(LT_ERROR, "Kernel", "Couln't decompress buffer. Invalid data.");
		return 0;
	}
	if(outsize)
	{
		if(out.size || out.data) gEngine.kernel->mem->freeBuff(out);
		if(!gEngine.kernel->mem->allocBuff(out, outsize, FILELINE))
		{
			gEngine.kernel->log->prn(LT_ERROR, "Kernel", "Couln't decompress buffer. Not enough memory for output buffer.");
			return 0;
		}
	}
	decomp(method, (u8*)in.data, (u8*)out.data, in.size, out.size);
	return 1;
}

void FileSystem::registerArchiveMgr(IFSArchiveMgr *archMgr)
{
	if(archMgr)
		archMgrs.push_back(archMgr);
}
/*** Console ***/

static ConsoleItem tempItem;

	#define ID_EDITINPUT 666
	HINSTANCE g_hInstance;
	HWND g_hwndLogWindow, g_hwndLogOutput,g_hwndLogInput;
	LRESULT CALLBACK ConsoleWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam){
	 switch(msg){
	  case WM_COMMAND: //if(LOWORD(wParam) == ID_EDITSUBMIT){  gEngine.kernel->con->exec(str); SetWindowText(g_hwndLogInput, ""); } break;
		  if(LOWORD(wParam) == ID_EDITINPUT && HIWORD(wParam) == EN_CHANGE){ 
			  static c8 str[256]; GetWindowText(g_hwndLogInput, str, 256);
			  static string cmd;
			  cmd = str;
			  if(cmd.size() > 0)
			  if(cmd.substr(cmd.size()-1) == "\n")
			  {
				  gEngine.kernel->con->exec(str);
				  SetWindowText(g_hwndLogInput, "");
			  }
		  }break;
	  case WM_CTLCOLORSTATIC:
	  case WM_CTLCOLOREDIT:
		  //HBRUSH g_hbBarva = CreateSolidBrush(RGB(0, 0, 0));
		  SetBkColor((HDC)wParam, RGB(0,0,0));
		  SetTextColor((HDC)wParam, RGB(255,100,1));
		  return (LRESULT)(HBRUSH) GetStockObject(BLACK_BRUSH);
		  break;
	  case WM_CLOSE:	/*DestroyWindow(hwnd);*/break;
	  case WM_DESTROY:	/*PostQuitMessage(0);*/break;	
	  case WM_SETFOCUS: SetFocus(g_hwndLogInput);break;
	  default:return	DefWindowProc(hwnd, msg, wParam, lParam);}
	 return 0;}
	DWORD WINAPI ConsoleWnd(void *ptr){ MSG msg; HFONT logwnd_font;
		WNDCLASSEX wc;	wc.cbSize = sizeof(WNDCLASSEX);
		wc.style = 0; wc.lpfnWndProc = ConsoleWndProc;
		wc.cbClsExtra = wc.cbWndExtra = 0;
		wc.hInstance = g_hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH); //(COLOR_WINDOW+1); <-White
		wc.lpszMenuName = NULL;	wc.lpszClassName = "LOGWINDOWOUTPUT";
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
		if(RegisterClassEx(&wc)){
			g_hwndLogWindow = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_APPWINDOW, "LOGWINDOWOUTPUT", XGINE_VER " - Console Window", WS_CAPTION | WS_BORDER | WS_SYSMENU, 0, 0, 600, 315, 0, NULL, g_hInstance, NULL);
			g_hwndLogOutput = CreateWindowEx(NULL, "LISTBOX", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_HSCROLL | WS_VSCROLL , 5, 5, 585, 260, g_hwndLogWindow, NULL, g_hInstance, NULL);
			g_hwndLogInput  = CreateWindowEx(NULL, "EDIT", "", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE, 5, 265, 585, 20, g_hwndLogWindow, (HMENU)ID_EDITINPUT, g_hInstance, NULL);
		}
		logwnd_font = CreateFont( 11, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, FF_MODERN|FIXED_PITCH, "Lucida Console");
		SendMessage(g_hwndLogOutput, WM_SETFONT, (WPARAM)logwnd_font, TRUE); 
		SendMessage(g_hwndLogInput, WM_SETFONT, (WPARAM)logwnd_font, TRUE);
		SendMessage(g_hwndLogOutput, LB_SETHORIZONTALEXTENT, 2000, 0);
		ShowWindow(g_hwndLogWindow, SW_SHOW);
		UpdateWindow(g_hwndLogWindow);
		gEngine.kernel->log->prn(LT_SUCCESS, "Kernel", "Initialized Console Window.");
		*((byte*)ptr) = 1;
		while(GetMessage(&msg, NULL, 0, 0)){ TranslateMessage(&msg); DispatchMessage(&msg); }
		return msg.wParam;}


i32 Console::initWnd(HINSTANCE hInstance)
{
	g_hInstance = hInstance;
	initializedWnd = 0;
	CreateThread(NULL,NULL,ConsoleWnd,(void*)&initializedWnd,NULL,NULL);
	while(!initializedWnd)Sleep(0);
	for(u32 i=0;i<txtBuff.size();i++)
		SendMessage(g_hwndLogOutput, LB_ADDSTRING, (WPARAM) 0, (LPARAM)txtBuff[i].c_str());
	return 0;
}

void Console::addFunc(ConsoleFuncPtr func, c8 *name)
{
	if(!func)return;
	CItemMapIt it = items.find(name);
	if(it == items.end())
	{
		tempItem.type = CTYPE_FUNCTION;
		tempItem.function = func;
		items.insert(std::make_pair(name, tempItem));
	}	
}

void Console::addVar(void *ptr, ConsoleItemType type, c8 *name)
{
	if(!ptr || type == CTYPE_FUNCTION)return;

	CItemMapIt it = items.find(name);
	if(it == items.end())
	{
		tempItem.type = type;
		tempItem.var_ptr = ptr;
		items.insert(std::make_pair(name, tempItem));
	
		CCfgVarMapIt iter = cfgVars.find(name);
		if(iter != cfgVars.end())
		{
			std::stringstream conv; 
			conv << iter->second;
			switch(type)
			{
					case CTYPE_UINT:	conv >> *((unsigned int*)ptr); break;
					case CTYPE_INT:		conv >> *((int*)ptr); break;
					case CTYPE_FLOAT:	conv >> *((float*)ptr); break;
					case CTYPE_STRING:	conv.unsetf(std::ios::skipws); conv >> *((std::string*)ptr); break;
					case CTYPE_VEC3:	conv >> (*((Vec3*)ptr)).x >> (*((Vec3*)ptr)).y >> (*((Vec3*)ptr)).z; break;
			}
			cfgVars.erase(iter);
		}
	}	
}

void Console::remove(c8 *name)
{
	CItemMapIt it = items.find(name);
	if(it == items.end())
	{
		items.erase(it);
	}
}

void Console::prn(string str)
{
	if(str.length())
	{
		txtBuff.push_back(str);
		if(initializedWnd)
		{
			SendMessage(g_hwndLogOutput, LB_ADDSTRING, (WPARAM) -1, (LPARAM)str.c_str());
			SendMessage(g_hwndLogOutput, LB_SETCURSEL, (WPARAM)gEngine.kernel->con->txtBuff.size()-1, 0);
		}
	}
}

void Console::exec(string strCmd)
{
	if(strCmd.length() <= 0)return;

	std::ostringstream out;
	//std::string::size_type index = 0;
	std::vector<std::string> arguments;

	//if(echo_enabled)
	prn(">"+strCmd);

	cmdBuff.push_back(strCmd);
	//if(m_commandBuffer.size() > max_commands)
	//m_commandBuffer.erase(m_commandBuffer.begin());

	//Tokenize
	std::string token;
	Tokenizer tokenizer(strCmd);
	while(tokenizer.nextToken(&token))	
		arguments.push_back(token);

	//for(int i = 0; i < arguments.size(); i++)
	//	Print("Arg: '"+arguments[i]+"'");

	//Execute
	if(arguments.size() < 0)return;
	if(arguments[0] == "help")
	{
		if(arguments.size() == 1)
		{
			prn("-To show variable value           type: 'var_name'");
			prn("-To set  variable value           type: 'var_name var_new_value'");
			prn("-To call function                 type: 'func_name param1 param2 ...'");
			prn("-To show all registered variables type: 'vars'");
			prn("-To show all registered functions type: 'funcs'");
			prn("-To show all registered items     type: 'items'");
		}
		return;
	}
	else if(arguments[0] == "vars")
	{
		CItemMapIt iter = items.begin();
		for(; iter != items.end(); iter++)
		{
			if(iter->second.type != CTYPE_FUNCTION)
				prn("V: "+iter->first);
		}
		return;
	}
	else if(arguments[0] == "funcs")
	{
		CItemMapIt iter = items.begin();
		for(; iter != items.end(); iter++)
		{
			if(iter->second.type == CTYPE_FUNCTION)
				prn(iter->first);
		}
		return;
	}
	else if(arguments[0] == "items")
	{
		CItemMapIt iter = items.begin();
		for(; iter != items.end(); iter++)
		{
			if(iter->second.type == CTYPE_FUNCTION)
				prn("F: "+iter->first);
			else
				prn("V: "+iter->first);
		}
		return;
	}
	else
	{
		CItemMapIt iter = items.find(arguments[0]);
		if(iter != items.end())
		{
			switch(iter->second.type)
			{
				case CTYPE_UINT:
				{
					if(arguments.size() > 2)return;
					else if(arguments.size() == 1)
					{
						out.str("");
						out << iter->first << " = " <<	*((unsigned int *)iter->second.var_ptr);
						prn(out.str());
						return;
					}
					else if(arguments.size() == 2)
					{
						std::stringstream conv; 
						//conv.unsetf(std::ios::skipws);
						conv << arguments[1];
						conv >> *((unsigned int *)iter->second.var_ptr) ;
						return;
					}
					break;
				}

				case CTYPE_INT:
				{
					if(arguments.size() > 2)return;
					else if(arguments.size() == 1)
					{
						out.str("");
						out << iter->first << " = " <<	*((int *)iter->second.var_ptr);
						prn(out.str());
						return;
					}
					else if(arguments.size() == 2)
					{
						std::stringstream conv; 
						//conv.unsetf(std::ios::skipws);
						conv << arguments[1];
						conv >> *((int *)iter->second.var_ptr) ;
						return;
					}
					break;
				}

				case CTYPE_FLOAT:
				{
					if(arguments.size() > 2)return;
					else if(arguments.size() == 1)
					{
						out.str("");
						out << iter->first << " = " <<	*((float *)iter->second.var_ptr);
						prn(out.str());
						return;
					}
					else if(arguments.size() == 2)
					{
						std::stringstream conv; 
						//conv.unsetf(std::ios::skipws);
						conv << arguments[1];
						conv >> *((float *)iter->second.var_ptr) ;
						return;
					}
					break;
				}

				case CTYPE_STRING:
				{
					if(arguments.size() > 2)return;
					else if(arguments.size() == 1)
					{
						out.str("");
						out << iter->first << " = " << *((std::string *)iter->second.var_ptr);
						prn(out.str());
						return;
					}
					else if(arguments.size() == 2)
					{
						std::stringstream conv; 
						conv.unsetf(std::ios::skipws);
						conv << arguments[1];
						conv >> *((std::string*)iter->second.var_ptr) ;
						return;
					}
					break;
				}

				case CTYPE_FUNCTION:
					iter->second.function(arguments);
					return;
				break;
			}
		}
	}

	prn( arguments[0] + " is not a recognized command. Type 'help' for help." );
}

i32 Console::loadCfg(string fileName)
{
	Buffer buff;
	if(!gEngine.kernel->fs->load(fileName, buff))
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Config file has not been loaded.", fileName.c_str());
		return 0;
	}
	
	Tokenizer tokenizer(string(buff.data, buff.size));
	gEngine.kernel->mem->freeBuff(buff);

	tokenizer.clear(";");

	string token, name, value;
	while(tokenizer.nextToken(&token))
	{
		name = token;
		if(!tokenizer.nextToken(&token) || token != "=")
		{
			gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Expected '=' after name of variable. Config '%s' has not been loaded.", fileName.c_str());
			return 0;
		}
		if(!tokenizer.nextToken(&token))
		{
			gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Expected value of variable after '='. Config '%s' has not been loaded.", fileName.c_str());
			return 0;
		}
		value = token;

		CItemMapIt iter = items.find(name);
		if(iter != items.end())
		{
			if(iter->second.type != CTYPE_FUNCTION)
			{
				std::stringstream conv; 
				conv << value;
				switch(iter->second.type)
				{
					case CTYPE_UINT:	conv >> *((unsigned int*)iter->second.var_ptr); break;
					case CTYPE_INT:		conv >> *((int*)iter->second.var_ptr); break;
					case CTYPE_FLOAT:	conv >> *((float*)iter->second.var_ptr); break;
					case CTYPE_STRING:	conv.unsetf(std::ios::skipws); conv >> *((std::string*)iter->second.var_ptr); break;
					case CTYPE_VEC2:	conv >> (*((Vec2*)iter->second.var_ptr)).x >> (*((Vec2*)iter->second.var_ptr)).y; break;
					case CTYPE_VEC3:	conv >> (*((Vec3*)iter->second.var_ptr)).x >> (*((Vec3*)iter->second.var_ptr)).y >> (*((Vec3*)iter->second.var_ptr)).z; break;
					case CTYPE_VEC4:	conv >> (*((Vec4*)iter->second.var_ptr)).x >> (*((Vec4*)iter->second.var_ptr)).y >> (*((Vec4*)iter->second.var_ptr)).z >> (*((Vec4*)iter->second.var_ptr)).w; break;
				}
			}
		}
		else
		{
			cfgVars[name] = value;	
		}
		if(g_logker)
			gEngine.kernel->log->prnEx(LT_INFO, "Kernel", "Loaded variable '%s' with value: \"%s\"", name.c_str(), value.c_str());
	}

	gEngine.kernel->log->prnEx(LT_SUCCESS, "Kernel", "Config '%s' loaded.", fileName.c_str());
	return 1;
}

i32 Console::saveCfg(string fileName)
{
	std::ofstream file;
	
	file.open(fileName.c_str(), std::ios::trunc);
	if(!file.is_open())
	{
		gEngine.kernel->log->prnEx(LT_ERROR, "Kernel", "Couldn't open file %s. Config file has not been saved.", fileName);
		return 0;
	}
	
	file << "; " << XGINE_VER << "\n"
		    ";-----------------------------------------------\n"
			"; Config file, auto-generated: " << getTime() << " " << getDate() << "\n"
			"; Editing on your own risk.\n\n";

	for(CItemMapIt it = items.begin(); it != items.end(); it++)
	{
		if(it->second.type == CTYPE_FUNCTION)continue;
		file << it->first << " = ";
		switch(it->second.type)
		{
			case CTYPE_UINT: file << *((unsigned int*)it->second.var_ptr); break;
			case CTYPE_INT: file << *((int*)it->second.var_ptr); break;
			case CTYPE_FLOAT:  file << *((float*)it->second.var_ptr); break;
			case CTYPE_STRING: file << "\"" << *((std::string*)it->second.var_ptr) << "\""; break;
			case CTYPE_VEC2: file << "\"" << (*((Vec2*)it->second.var_ptr)).x << " " << (*((Vec2*)it->second.var_ptr)).y << "\""; break;
			case CTYPE_VEC3: file << "\"" << (*((Vec3*)it->second.var_ptr)).x << " " << (*((Vec3*)it->second.var_ptr)).y << " " << (*((Vec3*)it->second.var_ptr)).z << "\""; break;
			case CTYPE_VEC4: file << "\"" << (*((Vec4*)it->second.var_ptr)).x << " " << (*((Vec4*)it->second.var_ptr)).y << " " << (*((Vec4*)it->second.var_ptr)).z << " " << (*((Vec4*)it->second.var_ptr)).w << "\""; break;
		}
		file << "\n";
	}
	
	file.flush();
	file.close();
	gEngine.kernel->log->prnEx(LT_INFO, "Kernel", "Config '%s' saved.", fileName.c_str());
	return 1;
}

string Console::getItemName(string namePart)
{
	if(namePart.size()>0)
		for(CItemMapIt it = items.begin(); it != items.end(); it++)
			if(strncmp(namePart.c_str(), it->first.c_str(), namePart.size()) == 0)
				return it->first;
	return namePart;
}

/*** Memory Manager ***/

void memadd(Buffer &buffer, u32& currentPos, c8* data, u32 size)
{
	if(!buffer.data || !buffer.size || !data || !size)return;
	if(currentPos + size > buffer.size)return;
	memcpy((void*)(buffer.data + currentPos), data, size);
	currentPos += size;
}

void memget(Buffer &buffer, u32& currentPos, c8* data, u32 size)
{
	if(!buffer.data || !buffer.size || !data || !size)return;
	if(currentPos + size > buffer.size)return;
	memcpy(data, (void*)(buffer.data + currentPos), size);
	currentPos += size;
}

void* MemoryManager::alloc(u32 size, c8 *file, u32 line)
{
	static MemoryTrack track;
	static void* ptr;

	if(!size)return 0;

	ptr = ::malloc(size);
	if(ptr)
	{
		track.ptr	= ptr;
		track.file	= file;
		track.line	= line;
		track.size	= size;
		allocs.insert(std::make_pair((u32)ptr, track));

		if(g_logmem)
			gEngine.kernel->log->prnEx(LT_DEBUG, "Kernel", "MemAlloc %u bytes in file '%s' at line %u", size, file, line);
	}
	return ptr;
}

void MemoryManager::free(void *ptr)
{
	MMTrackMapIt it = allocs.find((u32)ptr);
	if(it != allocs.end())
	{
		if(g_logmem)
			gEngine.kernel->log->prnEx(LT_DEBUG, "Kernel", "MemFree %u bytes, allocated in '%s' at line %u", it->second.size, it->second.file, it->second.line);
		allocs.erase(it);		
	}
	::free(ptr);
}

u32	MemoryManager::allocBuff(Buffer &buffer, u32 size, c8 *file, u32 line)
{
	if(!size)return 0;
	freeBuff(buffer);
	buffer.size = size;
	buffer.data = (c8*)alloc(buffer.size * sizeof(c8), file, line);
	return (buffer.data != 0);
}

void MemoryManager::freeBuff(Buffer &buffer)
{
	if(buffer.data)free((void*)buffer.data);
	buffer.size = 0;
	buffer.data = 0;
}

void MemoryManager::dump()
{
	if(allocs.size()<1)
	{
		if(g_logmem)
			gEngine.kernel->log->prn(LT_DEBUG, "Kernel", "MemDump: No unreleased memory.");
	}
	else
	{
		gEngine.kernel->log->prnEx(LT_WARNING, "Kernel", "MemDump: %u unreleased memory tracks.", allocs.size());
		MMTrackMapIt it = allocs.begin();
		for(; it != allocs.end(); it++)
		{
			gEngine.kernel->log->prnEx(LT_DEBUG, "Kernel", "MemDump: Unreleased memory 0x%p, size %u bytes, allocated in '%s' at line %u", it->first, it->second.size, it->second.file, it->second.line);
		}
	}
}

void MemoryManager::freeAll()
{
	if(allocs.size() > 0)
	{
		gEngine.kernel->log->prnEx(LT_DEBUG, "Kernel", "Releasing %u unreleased memory tracks.", allocs.size());
		MMTrackMapIt it = allocs.begin();
		for(; it != allocs.end(); it++)
		{
			gEngine.kernel->log->prnEx(LT_DEBUG, "Kernel", "Releasing unreleased memory 0x%p, size %u bytes, allocated in '%s' at line %u", it->first, it->second.size, it->second.file, it->second.line);
			::free(it->second.ptr);
		}
		allocs.clear();
	}
}

/*** Profiler ***/
void Profiler::add(const std::string& name)
{
	ProfilerCounter temp;
	memset(&temp,0,sizeof(ProfilerCounter));
	counters.insert(make_pair(name,temp));
}

void Profiler::del(const std::string& name)
{		
	tContainerIterator it = counters.find(name);
	if(it != counters.end())
	{
		counters.erase(it);
	}
}

void Profiler::start(const std::string& name)
{
	tContainerIterator it = counters.find(name);
	if(it != counters.end())
	{
			it->second.calls++;
			it->second.start = gEngine.kernel->tmr->getAppTime();
	}
}

void Profiler::end(const std::string& name)
{
	f32 t = gEngine.kernel->tmr->getAppTime();
	tContainerIterator it = counters.find(name);
	if(it != counters.end())
	{
		if(it->second.start != 0)
		{
			f32 len = t - it->second.start;
			it->second.sumtimecalls += len;
			it->second.avgtimecalls = it->second.sumtimecalls / it->second.calls ;
			it->second.start = 0;
		}
	}
}

std::string Profiler::result(const std::string& name)
{
	tContainerIterator it = counters.find(name);
	if(it != counters.end())
	{
		return "[" + name + "] Calls: " 
				+ toString(it->second.calls) + ", SumTime: " 
				+ toString(it->second.sumtimecalls) + " ms, AvgTime: " 
				+ toString(it->second.avgtimecalls) + " ms";
	}
	else return "";
}

/////////////////////////////////////////////////////////////////////////////////////////////////

HandyProfile::HandyProfile(const std::string& name)
	: m_name(name)
{
	gEngine.kernel->prf->add(name);
	gEngine.kernel->prf->start(name);
}

HandyProfile::~HandyProfile()
{
	gEngine.kernel->prf->end(m_name);
	gEngine.renderer->addTxt((gEngine.kernel->prf->result(m_name)).c_str());
	gEngine.kernel->prf->del(m_name);
}

/*** Kernel ***/
static u32 g_kernelLoggerCfgLoaded = 0;

u32	Kernel::init(c8 *logFileName, c8 *cfgFileName)
{
	this->log	= (ILogger*)		new Logger();
	this->tmr	= (ITimer*)			new Timer();
	this->prf	= (IProfiler*)		new Profiler();
	this->mem	= (IMemoryManager*)	new MemoryManager();
	this->con	= (IConsole*)		new Console();
	this->fs	= (IFileSystem*)	new FileSystem();

	{
		this->con->addFunc(confunc_fs, "fs");
	}

	{
		REGCVARU32(g_logvfs);
		REGCVARU32(g_logmem);
		REGCVARU32(g_logker);
		REGCVARU32(r_width);
		REGCVARU32(r_height);
		REGCVARU32(r_windowed);
		REGCVARU32(r_wireframe);
		REGCVARU32(r_hidestats);
		REGCVARU32(r_multisample);
		REGCVARU32(r_geometryInstancing);
		REGCVARF32(r_cameraNearZ);
		REGCVARF32(r_cameraFarZ);
		REGCVARSTR(g_path_shaders);
		REGCVARSTR(g_path_textures);
		REGCVARSTR(g_path_models);
		REGCVARU32(r_shadowMapSpotSize);
		REGCVARU32(r_shadowMapDirSize);
		REGCVARF32(g_shadowMapDirViewVolume);
		REGCVARF32(g_shadowMapDirViewDist);
		REGCVARF32(g_shadowMapDirNearZ);
		REGCVARF32(g_shadowMapDirFarZ);
		REGCVARF32(r_shadowMappingZBias);
		REGCVARU32(r_optimizeRSChanges);
		REGCVARU32(g_allowShaderCache);
		REGCVARU32(g_allowMeshCache);
		REGCVARU32(g_allowResourcesCache);
		REGCVARU32(r_autoGenerateLODs);
		REGCVARU32(r_enableLODs);
		REGCVARU32(r_packInstanceDataToFloat16);
		REGCVARSTR(g_terrainShader);
		REGCVARSTR(g_terrainTexColorMap1);
		REGCVARSTR(g_terrainTexColorMap2);
		REGCVARSTR(g_terrainTexColorMap3);
		REGCVARSTR(g_terrainTexColorMap4);
		REGCVARSTR(g_terrainTexNormalMap1);
		REGCVARSTR(g_terrainTexNormalMap2);
		REGCVARSTR(g_terrainTexNormalMap3);
		REGCVARSTR(g_terrainTexNormalMap4);
		REGCVARU32(g_terrainDebugBB);
		REGCVARU32(g_terrainSectorSize);
		REGCVARF32(g_terrainMaxHeight);
		REGCVARU32(g_terrainMaxLODLevels);
		REGCVARF32(g_terrainLowVBExponent);
		REGCVARF32(g_terrainLowVBExponentX);
		REGCVARF32(g_terrainMaxDistHighGeo);
		REGCVARF32(g_terrainMaxDistHighGeoStep);
		REGCVARF32(g_terrainMaxDistHighTex);
		REGCVARF32(g_terrainMaxDistHighTexFade);
		REGCVARU32(g_terrainLodStart);
		REGCVARU32(g_terrainLodEnabled);
		REGCVARU32(g_rendObjDebugBB);
		REGCVARU32(g_rendOctreeDebugBB);
		REGCVARU32(r_shadowDist);
		REGCVARF32(r_bloomBrightThreshold);
		REGCVARF32(r_bloomAmount);
		REGCVARF32(r_bloomIntensity);
		REGCVARF32(r_bloomBaseIntensity);
		REGCVARF32(r_bloomSaturation);
		REGCVARF32(r_bloomBaseSaturation);
		REGCVARU32(r_bloomEnabled);		
		REGCVARU32(r_enabledSoftParticles);
		REGCVARU32(r_enabledReflectionBlur);
		REGCVARU32(r_enabledSkyReflection);
		REGCVARU32(r_enabledTerrainReflection);
		REGCVARU32(r_enabledSurfacesReflection);
		REGCVARU32(r_enabledEntitiesReflection);
		REGCVARU32(r_deferredStencilOptimize);
		REGCVARU32(r_deferredScissorOptimize);
		REGCVARU32(r_deferredDebugComplexity);
		REGCVARU32(r_enabledNormalMap);
		REGCVARU32(r_enabledParallaxMap);
		REGCVARU32(r_enabledSpecular);
		REGCVARU32(r_enabledAlbedo);
		REGCVARU32(r_enabledCubeMap);
		REGCVARU32(r_enabledSubsurfaceScattering);
		REGCVARU32(r_enabledVegetationAnimation);
		REGCVARU32(r_cullingLights);
		REGCVARU32(r_cullingFrustum);
		REGCVARU32(r_cullingOcclusion);
		REGCVARU32(r_enabledShadows);
		REGCVARU32(r_enabledShadowMapping);
		REGCVARU32(r_enabledVarianceShadowMapping);
		REGCVARU32(r_enabledBlurVarianceShadowMaps);
		REGCVARU32(r_enabledBoxFilterVarianceShadowMaps);
		REGCVARU32(r_enabledExponentialShadowMapping);
		REGCVARU32(r_enabledShadowMappingGeoRendering);
		REGCVARU32(r_enabledShadowMappingDeferred);
		REGCVARU32(r_enabledShadowMappingPCF);
		REGCVARU32(r_enabledShadowMappingPCFHigh);
		REGCVARU32(r_enabledShadowMappingPCFMed);
		REGCVARU32(r_enabledSMScreenSpaceBlur);
		REGCVARU32(r_enabledShadowVolume);
		REGCVARU32(r_guassianBlurKernelSize);
		REGCVARU32(r_enabledDeferred);
		REGCVARU32(r_enabledLightPrePass);
		REGCVARU32(r_forceForward);
		REGCVARU32(r_enabledZPrePass);
		REGCVARU32(r_renderSky);
		REGCVARU32(r_renderTerrain);
		REGCVARU32(r_renderSurfaces);
		REGCVARU32(r_renderEntities);
		REGCVARU32(r_enabledSSAO);
		REGCVARF32(r_ssaoRadius);
		REGCVARF32(r_ssaoFresZSize);
		REGCVARF32(r_ssaoCap);
		REGCVARF32(r_ssaoMultiplier);
		REGCVARF32(r_ssaoDepthTolerance);
		REGCVARU32(r_hdrRendering);
		REGCVARU32(r_hdrToneMap);
		REGCVARF32(r_hdrToneMapScale);
		REGCVARU32(r_hdrEyeAdaptation);
		REGCVARF32(r_hdrEyeAdaptationTimeScale);
		REGCVARU32(r_hdrBloom);
		REGCVARF32(r_hdrBloomAmount);
		REGCVARU32(r_hdrBrightPass);
		REGCVARU32(r_dofEnabled);
		REGCVARF32(r_dofFocalPlane);
		REGCVARF32(r_dofNearFocalPlane);
		REGCVARF32(r_dofFarFocalPlane);
		REGCVARU32(r_dofEyeAdaptation);
		REGCVARF32(r_dofEyeAdaptationTimeScale);
		REGCVARF32(r_dofBlurExponent);
		REGCVARU32(r_heatHazeEnabled);
		REGCVARF32(r_heatHazeStrength);
		REGCVARF32(r_heatHazeTimeScale);
		REGCVARU32(r_waterRenderingEnabled);
		REGCVARU32(r_highQualityWaterEnabled);
		REGCVARU32(r_waterOcean);
		REGCVARF32(r_waterAmplitude);
		REGCVARF32(r_waterH0coef);
		REGCVARF32(r_waterH1coef);
		REGCVARF32(r_waterH2coef);
		REGCVARF32(r_waterH3coef);
		REGCVARF32(r_waterDetailNormalMapCoef);
		REGCVARF32(r_waterDetailNormalMapScale);
		REGCVARF32(r_waterFresnelR0);
		REGCVARF32(r_waterRefractionStrength);
		REGCVARF32(r_waterSpecularExp);
		REGCVARVEC3(r_waterColor);
		REGCVARF32(r_refExp);
		REGCVARU32(r_sunColorEnabled);
		REGCVARVEC3(r_sunColor);
		REGCVARU32(r_skyColorEnabled);
		REGCVARVEC3(r_skyColorFront);
		REGCVARVEC3(r_skyColorBack);
		REGCVARF32(r_sunSizeExp);
		REGCVARU32(r_enabledVLS);
		REGCVARU32(r_enabledHDRVLS);
		REGCVARVEC4(r_fogParams);
		REGCVARVEC3(r_fogColor);
	}

	u32 logInit					= log->init(logFileName);
		g_kernelLoggerCfgLoaded = con->loadCfg(cfgFileName);
	u32 tmrInit					= tmr->init();

	u32 fsInit					= fs->init();

	if(!logInit || !tmrInit || !fsInit)
		return 0;
	else 
		return 1;
}

u32	Kernel::close(c8 *cfgSaveFileName, i32 dumpMem , i32 freeMem)
{
	i32 result = 1;
	fs->close();
	if(cfgSaveFileName)result = con->saveCfg(cfgSaveFileName);
	if(dumpMem)mem->dump();
	if(freeMem)mem->freeAll();
	log->close();

	delete(prf);	prf = 0;
	delete(fs);		fs  = 0;
	delete(con);	con = 0;
	delete(mem);	mem = 0;
	delete(tmr);	tmr = 0;
	delete(log);	log = 0;
	return result;
}

void Kernel::addMemUsage(string name, u32 sizeInBytes)
{
	memUsage[name] += sizeInBytes;
}

void Kernel::subMemUsage(string name, u32 sizeInBytes)
{
	mapMemUsageIt it = memUsage.find(name);
	if(it != memUsage.end())
	{
		if(it->second < sizeInBytes) it->second = 0;
		else it->second += sizeInBytes;
	}
}