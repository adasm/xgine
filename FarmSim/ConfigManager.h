#pragma once
#include "Headers.h"

enum VariableType
{
	V_INT,
	V_FLOAT,
	V_STRING,
	V_VEC3,
	V_VEC4,
};

struct CfgVariable
{
	VariableType variableType;
	void* variable;
	string variableName;
	CfgVariable(VariableType varType, void* var, string varName)
	{
		variableType = varType;
		variable = var;
		variableName = varName;
	};
	~CfgVariable()
	{
		variableName = "";
	}
};

struct Table
{
	string tableName;
	vector<TableEntry*> vars;
};

struct TableEntry
{
	void *var;
	VariableType type;
};

class ConfigManager
{
public:
	template<class T> T*					load(string fname);
	string									getMeshesList();
	string									getTexturesList();
protected:
	map<std::string, IConfig*>				m_configs;
	map<std::string, IConfig*>::iterator	m_configsIt;
};