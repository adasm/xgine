#include "Headers.h"

struct IConfig
{
public:
	friend ConfigManager;
	virtual		CfgVariable*			getVar(string) = 0;
	virtual		const vector<Table*>*	getTable(string) = 0;
	virtual		string					getTexturesList() = 0;
	virtual		string					getMeshesList() = 0;
protected:
	virtual		void					load(string);
};