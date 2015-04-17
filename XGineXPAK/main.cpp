/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#include "../XGine/XGine.h"
#pragma comment(lib, "../Debug/XGine.lib");
#include <iostream>
using namespace std;

string input, output, compType;
vector< FSPakFileInfo > files;
int change;
u32 type, i;
FSPakFileInfo tempFile;

void inputProc();
void prepFileList();

int main()
{
	gEngine.init();
	gEngine.kernel->init();
	gEngine.kernel->con->initWnd(0);
	gEngine.loadPluginCfg("plugins.txt");
		inputProc();
		prepFileList();
		string ss = (type == 1)?("Quake 1 PACK"):("XGine XPAK");
		bool found = false;
			for(u32 i = 0; i < gEngine.kernel->fs->archMgrs.size(); i++)
				{
					if(gEngine.kernel->fs->archMgrs[i]->getType() == ss)
					{
						gEngine.kernel->fs->archMgrs[i]->create(output, files);
						found = true;
						break;
					}
				}
		if(!found)gEngine.kernel->log->prnEx(LT_ERROR, "XGineXPAK", "FAILED....");
		system("pause");
	gEngine.kernel->close();
	gEngine.close();
	return 0;
}

void inputProc()
{
	cout << "XGineXPAK by Adam Michalowski (c) 2009\n" << XGINE_VER << endl;
	cout << "Avaialable compression types:\n"
		 <<	"rle     - RLE compression\n"
		 <<	"huff    - Huffman compression\n"
		 <<	"rice8   - Rice 8-bit compression\n"
		 <<	"rice16  - Rice 16-bit compression\n"
		 <<	"rice32  - Rice 32-bit compression\n"
		 <<	"rice8s  - Rice 8-bit signed compression\n"
		 <<	"rice16s - Rice 16-bit signed compression\n"
		 <<	"rice32s - Rice 32-bit signed compression\n"
		 <<	"lz      - LZ77 compression\n"
		 <<	"sf      - Shannon-Fano compression\n"
		 <<	"b       - Best bcl compression for current buffer\n"
		 <<	"zs      - ZLib best-speed compression\n"
		 <<	"zc      - ZLib best-compression compression\n"
		 <<	"z       - ZLib default compression\n. . .\n";

	input		= "filesToPack.txt";
	output		= "pak0.xpak";
	type		= 2;
	compType	= "zc";

	cout << "\nName of File-list file:                   " << input << endl;
	cout << "Output file:                                 " << output << endl;
	cout << "Type of pack(1- Quake PACK , 2- XGine XPAK): " << type << endl;
	cout << "Compression type:                            "  << compType;

	change = 0;
	cout << "\nChange?(0/1) : " ;
	cin  >> change;
	if(change)
	{
		cout << "\nName of File-list file:                   ";
		cin  >> input;
		cout << "Output file:                                 ";
		cin  >> output;
		cout << "Type of pack(1- Quake PACK , 2- XGine XPAK): ";
		cin  >> type;
		cout << "Compression type:                            " ;
		cin  >> compType;
	}
}

void prepFileList()
{
		 if( strcmp( compType.c_str(), "rle" ) == 0 )		tempFile.compType = COMP_RLE;
	else if( strcmp( compType.c_str(), "huff" ) == 0 )		tempFile.compType = COMP_HUFF;
	else if( strcmp( compType.c_str(), "rice8" ) == 0 )		tempFile.compType = COMP_RICE8;
	else if( strcmp( compType.c_str(), "rice16" ) == 0 )	tempFile.compType = COMP_RICE16;
	else if( strcmp( compType.c_str(), "rice32" ) == 0 )	tempFile.compType = COMP_RICE32;
	else if( strcmp( compType.c_str(), "rice8s" ) == 0 )	tempFile.compType = COMP_RICE8S;
	else if( strcmp( compType.c_str(), "rice16s" ) == 0 )	tempFile.compType = COMP_RICE16S;
	else if( strcmp( compType.c_str(), "rice32s" ) == 0 )	tempFile.compType = COMP_RICE32S;
	else if( strcmp( compType.c_str(), "lz" ) == 0 )		tempFile.compType = COMP_LZ;
	else if( strcmp( compType.c_str(), "sf" ) == 0 )		tempFile.compType = COMP_SF;
	else if( strcmp( compType.c_str(), "b" ) == 0 )			tempFile.compType = COMP_BESTBCL;
	else if( strcmp( compType.c_str(), "zs" ) == 0 )		tempFile.compType = COMP_ZLIBS;
	else if( strcmp( compType.c_str(), "zc" ) == 0 )		tempFile.compType = COMP_ZLIBC;
	else if( strcmp( compType.c_str(), "z" ) == 0 )			tempFile.compType = COMP_ZLIB;
	else													tempFile.compType = COMP_NONE;

	cout << "Preparing file list . . ." << endl;

	string			name;
	vector<string>	fileList;
	ifstream		file;

	FSPakFileInfo f;
	f.compType = COMP_ZLIB;

	file.open(input.c_str());
	if(file.is_open())
	{
		while( (file >> name) )
		{
			cout << "File-list file: " << name << endl;
			f.packedFileName = name;
			f.fileName = name;
			files.push_back(f);
		}
	}

	cout << "Preparing file list has been finished." << endl;
}