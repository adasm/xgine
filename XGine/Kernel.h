/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

/*** Common Functions ***/

XGINE_API c8*			getDate();
XGINE_API c8*			getTime();
XGINE_API void			prepFileName(c8* fileName);
XGINE_API void			prepFileName(string &fileName);
XGINE_API std::string	getExt(const std::string& fileName);
XGINE_API std::string	getWithoutExt(const std::string& fileName);
XGINE_API std::string   convstr(const std::wstring& str);
XGINE_API std::wstring  convstr(const std::string& str);

template <typename T> inline std::string toString(const T &var)
{std::stringstream ss;ss << var;return ss.str();}
template <> inline std::string toString(const bool &var)
{return (var ? "TRUE" : "FALSE");}
template <> inline std::string toString(const D3DXVECTOR3& var)
{return ("(" + toString(var.x) + ", " + toString(var.y) + ", " + toString(var.z) + ")");}
template <typename T> inline T strConv(const std::string &var)
{std::stringstream ss; ss << var; T varConv; ss >> varConv; return varConv;}

/*** Logger ***/
enum XGINE_API LogType
{
	LT_INFO,
	LT_SUCCESS,
	LT_WARNING,
	LT_ERROR,
	LT_DEBUG,
	LT_FATAL
};

typedef struct XGINE_API ILogger
{
	virtual ~ILogger() { };
	virtual u32		init(const c8 *fileName) = 0;
	virtual void	close() = 0;
	virtual void	prn(LogType type, c8* module, c8 *str) = 0;
	virtual void	prnEx(LogType type, c8* module, c8 *str, ...) = 0;
}*ILogger_ptr;

typedef struct XGINE_API Logger : ILogger
{
	Logger() { }
	~Logger() { }

	u32		init(const c8 *fileName);
	void	close();

	void	prn(LogType type, c8* module, c8 *str);
	void	prnEx(LogType type, c8* module, c8 *str, ...);
	
private:
	std::ofstream	file;
}*Logger_ptr;

/*** FileSystem ***/
#define FSAT_AUTO		0x00
#define FSAT_PACK		0x01
#define FSAT_XPAK		0x02

#define COMP_NONE		0x00 //No compression
#define COMP_RLE		0x01 //RLE compression
#define COMP_HUFF		0x02 //Huffman compression
#define COMP_RICE8		0x03 //Rice 8-bit compression
#define COMP_RICE16		0x04 //Rice 16-bit compression
#define COMP_RICE32		0x05 //Rice 32-bit compression
#define COMP_RICE8S		0x06 //Rice 8-bit signed compression
#define COMP_RICE16S	0x07 //Rice 16-bit signed compression
#define COMP_RICE32S	0x08 //Rice 32-bit signed compression
#define COMP_LZ			0x09 //LZ77 compression
#define COMP_SF			0x0A //Shannon-Fano compression
#define COMP_BESTBCL	0x0B //Best bcl compression for current buffer
#define COMP_ZLIBS		0xC1 //ZLib best-speed compression
#define COMP_ZLIBC		0xC2 //ZLib best-compression compression
#define COMP_ZLIB		0xC3 //ZLib default compression

enum XGINE_API SearchType
{
	ST_ALL		= 0x00000011,
	ST_DISK		= 0x00000001,
	ST_ARCHIVE	= 0x00000010
};

typedef struct XGINE_API Compression
{
	static c8*		getName(u8 method);
	static u32		getMaxCompressedSize(u8 method, u32 insize);
	static u32		comp(u8 method, u8 *in, u8 *out, u32 insize, u32 *outsize);
	static void		decomp(u8 method, u8 *in, u8 *out, u32 insize, u32 outsize);
	static u32		compBuff(u8 method, Buffer &in, Buffer &out);
	static u32		decompBuff(u8 method, Buffer &in, Buffer &out, u32 outsize);
}*Compression_ptr;

typedef struct XGINE_API FSPakFileInfo
{
	FSPakFileInfo() : compType(0), mdate(0), buffer(0) { }
	string	fileName;
	string	packedFileName;
	u8		compType;
	u64		mdate;
	Buffer*	buffer;
}*FSPakFileInfo_ptr;

typedef struct XGINE_API IFSArchive
{
	virtual ~IFSArchive() { };
	virtual u32 open(string fileName) = 0;
	virtual u32 exists(string fileName) = 0;
	virtual u32 load(string fileName, Buffer &buffer) = 0;
	virtual u32 add(FSPakFileInfo &file) { return 0; }
	virtual u64	getModDate(string fileName) { return 0; }
	virtual u32 update() { return 0; }
	virtual u32	canUpdate() { return 0; }
}*IFSArchive_ptr;

typedef map<string, IFSArchive*>	FSArchiveMap;
typedef FSArchiveMap::iterator		FSArchiveMapIt;
typedef set<string>					FSUsedFilesSet;
typedef FSUsedFilesSet::iterator	FSUsedFilesSetIt;

typedef struct XGINE_API IFSArchiveMgr
{
	virtual ~IFSArchiveMgr() { };
	virtual u32			create(string archiveName, vector<FSPakFileInfo> &files) = 0;
	virtual u32			canLoad(string archiveName) = 0;
	virtual IFSArchive* load(string archiveName) = 0;
	virtual string		getType() = 0;
	virtual u32			canUpdate() { return 0; }
}*IFSArchiveMgr_ptr;

typedef vector<IFSArchiveMgr*>		FSArchMgrs;

typedef struct XGINE_API IFileSystem
{
	virtual ~IFileSystem() { };

	virtual u32				init() = 0;
	virtual void			close() = 0;

	virtual u32				createArchive(string archiveName, string archiveType, vector<FSPakFileInfo> &files) = 0;
	virtual u32				addSrc(string fileName) = 0;
	virtual u32				addSrc(string fileName, string archiveType) = 0;
	virtual u32				load(string fileName, Buffer &buffer, SearchType searchType = ST_ALL) = 0;
	virtual u32				loadCached(string fileName, Buffer &buffer, string cacheName) = 0;
	virtual u32				exists(string fileName, SearchType searchType = ST_ALL) = 0;
	virtual u64				getModDate(string fileName, SearchType searchType = ST_ALL) = 0;
	virtual u32				addToArchive(string archiveName, FSPakFileInfo &file, string archiveType = "") = 0;
	virtual u32				updateArchive(string archiveName) = 0;
	virtual u32				writeBuffToFile(string fileName, Buffer &buffer) = 0;
	virtual u32				findFile(string fileName, string &filePath, string dir = "") = 0;
	virtual void			listAllFiles(vector<string> &fileList, string dir = "") = 0;
	virtual void			registerArchiveMgr(IFSArchiveMgr *archMgr) = 0;
	virtual u32				dumpUsedFilesToPack(string archiveName, string archiveType, u8 compType = COMP_ZLIB) = 0;
	
	FSArchiveMap			archives;
	FSArchMgrs				archMgrs;
	FSUsedFilesSet			loadedFiles;
}*IFileSystem_ptr;

void XGINE_API fsRemoveCurrentDirPath(string &fileName);
void XGINE_API confunc_fs(const vector<string> &);

typedef struct XGINE_API FileSystem : IFileSystem
{
public:
	FileSystem() { }
	~FileSystem() { }
	
	u32				init();
	void			close();
	u32				createArchive(string archiveName, string archiveType, vector<FSPakFileInfo> &files);
	u32				addSrc(string fileName);
	u32				addSrc(string fileName, string archiveType);
	u32				load(string fileName, Buffer &buffer, SearchType searchType = ST_ALL);
	u32				loadCached(string fileName, Buffer &buffer, string cacheName);
	u32				exists(string fileName, SearchType searchType = ST_ALL);
	u64				getModDate(string fileName, SearchType searchType = ST_ALL);
	u32				addToArchive(string archiveName, FSPakFileInfo &file, string archiveType = "");
	u32				updateArchive(string archiveName);
	u32				writeBuffToFile(string fileName, Buffer &buffer);
	u32				findFile(string fileName, string &filePath, string dir = "");
	void			listAllFiles(vector<string> &fileList, string dir = "");
	void			registerArchiveMgr(IFSArchiveMgr *archMgr);
	u32				dumpUsedFilesToPack(string archiveName, string archiveType, u8 compType = COMP_ZLIB);
}*FileSystem_ptr;

/*** Universal Value ***/
enum XGINE_API UniversalValueType
{
	UTYPE_UINT,       // variable: u32
	UTYPE_INT,        // variable: i32
	UTYPE_FLOAT,      // variable: f32
	UTYPE_BOOL,		  // variable: bool
};

typedef struct XGINE_API UniversalValue
{
	UniversalValueType type;
	union
	{
		u32		u;
		i32		i;
		f32		f;
		bool	b;
	};
}*UniversalValue_ptr;

/*** Console ***/
enum XGINE_API ConsoleItemType
{
	CTYPE_UINT,       // variable: u32
	CTYPE_INT,        // variable: i32
	CTYPE_FLOAT,      // variable: f32
	CTYPE_STRING,     // variable: string
	CTYPE_VEC2,		  // variable: Vec2
	CTYPE_VEC3,		  // variable: Vec3
	CTYPE_VEC4,		  // variable: Vec4
	CTYPE_FUNCTION    // function
};

typedef void (*ConsoleFuncPtr)(const vector<string> &);

typedef struct XGINE_API ConsoleItem
{
	ConsoleItemType	type;
	union
	{
		void			*var_ptr;
		ConsoleFuncPtr	function;
	};
}*ConsoleItem_ptr;

typedef map<std::string, ConsoleItem>	CItemMap;
typedef CItemMap::iterator				CItemMapIt;

typedef map<string, string>				CCfgVarMap;
typedef CCfgVarMap::iterator			CCfgVarMapIt;

typedef struct XGINE_API IConsole
{
	virtual i32		initWnd(HINSTANCE hInstance) = 0;
	virtual void	addFunc(ConsoleFuncPtr func, c8 *name) = 0;
	virtual void	addVar(void *ptr, ConsoleItemType type, c8 *name) = 0;
	virtual void	remove(c8 *name) = 0;
	virtual void	prn(string str) = 0;
	virtual void	exec(string strCmd) = 0;
	virtual i32		loadCfg(string fileName) = 0;
	virtual i32		saveCfg(string fileName) = 0;
	virtual string	getItemName(string namePart) = 0;

	CItemMap	items;
	CCfgVarMap	cfgVars;

	vector<string>	txtBuff;
	vector<string>	cmdBuff;
}*IConsole_ptr;

typedef struct XGINE_API Console : IConsole
{
protected:
	byte	initializedWnd;

public:
	Console() : initializedWnd(0) { }
	~Console() {}

	i32		initWnd(HINSTANCE hInstance);
	void	addFunc(ConsoleFuncPtr func, c8 *name);
	void	addVar(void *ptr, ConsoleItemType type, c8 *name);
	void	remove(c8 *name);
	void	prn(string str);
	void	exec(string strCmd);
	i32		loadCfg(string fileName);
	i32		saveCfg(string fileName);
	string	getItemName(string namePart);
}*Console_ptr;

#define REGCVARI32(var) gEngine.kernel->con->addVar(&var,CTYPE_INT,#var)
#define REGCVARU32(var) gEngine.kernel->con->addVar(&var,CTYPE_UINT,#var)
#define REGCVARF32(var) gEngine.kernel->con->addVar(&var,CTYPE_FLOAT,#var)
#define REGCVARSTR(var) gEngine.kernel->con->addVar(&var,CTYPE_STRING,#var)
#define REGCVARVEC2(var) gEngine.kernel->con->addVar(&var,CTYPE_VEC2,#var)
#define REGCVARVEC3(var) gEngine.kernel->con->addVar(&var,CTYPE_VEC3,#var)
#define REGCVARVEC4(var) gEngine.kernel->con->addVar(&var,CTYPE_VEC4,#var)

/*** Memory Manager ***/
typedef struct XGINE_API Buffer
{
	Buffer() : data(0), size(0) { }
	~Buffer() { }

	c8	*data;
	u32	size;
}*Buffer_ptr;

XGINE_API void memadd(Buffer &buffer, u32& currentPos, c8* data, u32 size);
XGINE_API void memget(Buffer &buffer, u32& currentPos, c8* data, u32 size);

typedef struct XGINE_API MemoryTrack
{
	void *ptr;
	c8	*file;
	u32	line;
	u32 size;
}*MemoryTrack_ptr;

typedef map<u32, MemoryTrack>	MMTrackMap;
typedef MMTrackMap::iterator	MMTrackMapIt;

typedef struct IMemoryManager
{
	virtual ~IMemoryManager() { };
	virtual void*	alloc(u32 size, c8 *file, u32 line) = 0;
	virtual void	free(void *ptr) = 0;
	virtual u32		allocBuff(Buffer &buffer, u32 size, c8 *file, u32 line) = 0;
	virtual void	freeBuff(Buffer &buffer) = 0;
	virtual void	dump() = 0;
	virtual void	freeAll() = 0;
}*IMemoryManager_ptr;

typedef struct XGINE_API MemoryManager : IMemoryManager
{
	MemoryManager() { }
	~MemoryManager() { }

	void*	alloc(u32 size, c8 *file, u32 line);
	void	free(void *ptr);

	u32		allocBuff(Buffer &buffer, u32 size, c8 *file, u32 line);
	void	freeBuff(Buffer &buffer);

	void	dump();
	void	freeAll();
	
	MMTrackMap	allocs;
}*MemoryManager_ptr;

/*** Timer ***/
typedef struct XGINE_API ITimer
{
	virtual ~ITimer() { };
	virtual u32		init() = 0;
	virtual void	update() = 0;
	virtual f32		get() = 0;
	virtual f32		getCurrent() = 0;
	virtual f32		getFrameTime() = 0;
	virtual u32		getFrameID() = 0;
	virtual f32		getAppTime() = 0;
}*ITimer_ptr;

typedef struct XGINE_API Timer : ITimer
{
	Timer() { m_frameID = 0; }
	~Timer() { }

	inline u32 init()
	{
		m_frameID = 0;
		if (!QueryPerformanceFrequency(&m_ticsPerSecond ))return 0;
		else{ QueryPerformanceCounter(&m_appStartTime); m_startTime = m_appStartTime; return 1; }
	}
	inline void  update()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		m_lastInterval = ((f32)currentTime.QuadPart - (f32)m_startTime.QuadPart) / (f32)m_ticsPerSecond.QuadPart * 1000;
		m_startTime = currentTime;
		m_frameID++;
	}
	inline f32 get() { return m_lastInterval; }
	inline f32 getCurrent()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		m_frameStartTime = (m_startTime.QuadPart) / (f32)m_ticsPerSecond.QuadPart * 1000;
		return ((f32)currentTime.QuadPart - (f32)m_startTime.QuadPart) / (f32)m_ticsPerSecond.QuadPart * 1000;
	}
	inline f32 getFrameTime()
	{
		return m_frameStartTime;
	}
	inline u32 getFrameID()
	{
		return m_frameID;
	}
	inline f32 getAppTime()
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return ((f32)currentTime.QuadPart - (f32)m_appStartTime.QuadPart) / (f32)m_ticsPerSecond.QuadPart * 1000;
	}

protected:
	LARGE_INTEGER m_ticsPerSecond;
	LARGE_INTEGER m_startTime;
	f32			  m_lastInterval;
	f32			  m_frameStartTime;
	LARGE_INTEGER m_appStartTime;
	u32			  m_frameID;

}*Timer_ptr;

/*** Profiler ***/

struct XGINE_API ProfilerCounter
{
	int				calls;
	double			avgtimecalls;
	double			sumtimecalls;
	int				start;
};

typedef std::map<std::string, ProfilerCounter>				tContainer;
typedef std::map<std::string, ProfilerCounter>::iterator	tContainerIterator;

typedef struct XGINE_API IProfiler
{
	virtual ~IProfiler() { };
	virtual void add(const std::string& name) = 0;
	virtual void del(const std::string& name) = 0;
	virtual void start(const std::string& name) = 0;
	virtual void end(const std::string& name) = 0;
	virtual std::string result(const std::string& name) = 0;

	tContainer counters;
}*IProfiler_ptr;

typedef struct XGINE_API Profiler : IProfiler
{
public:
	void add(const std::string& name); //Dodawanie nowego licznika o podanej nazwie
	void del(const std::string& name); //Usuwanie licznika

	void start(const std::string& name); //Start podanego licznika
	void end(const std::string& name); //Stop
	std::string result(const std::string& name); //Zwrocenie rezultatow podanego licznika w stringu	
}*Profiler_ptr;

typedef struct XGINE_API HandyProfile
{
public:
	HandyProfile(const std::string& name);
	~HandyProfile();

private:
	std::string m_name;
}*HandyProfile_ptr;

#define PROFILER(name) HandyProfile xxx0xxx(name);

/*** Kernel ***/
typedef std::map< std::string, u32 >			mapMemUsage;
typedef std::map< std::string, u32 >::iterator	mapMemUsageIt;
typedef struct XGINE_API Kernel
{
	Kernel() { log = 0; mem = 0; tmr = 0; }
	~Kernel() { }

	u32	init(c8 *logFileName = "error.html", c8 *cfgLoadFileName = "cfg.txt");
	u32	close(c8 *cfgSaveFileName = "cfg.txt", i32 dumpMem = 1, i32 freeMem = 1);

	ILogger			*log;
	IMemoryManager	*mem;
	IConsole		*con;
	IFileSystem		*fs;
	ITimer			*tmr;
	IProfiler		*prf;
	
	void			addMemUsage(string name, u32 sizeInBytes);
	void			subMemUsage(string name, u32 sizeInBytes);

	mapMemUsage		memUsage;
}*Kernel_ptr;