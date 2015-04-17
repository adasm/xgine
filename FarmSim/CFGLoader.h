#pragma once
#include "Headers.h"

enum VariableType
{
	V_UNKNOWN = -1,
	V_INT = 0,
	V_FLOAT = 1,
	V_STRING = 2,
	V_VEC2 = 3,
	V_VEC3 = 4,
	V_VEC4 = 5,
	V_STRUCT = 6,
};


struct CfgVariable
{
	void *variable;
	VariableType variableType;
	string variableName;
	CfgVariable(VariableType v_type, void* v_data, string v_name)
	{
		variable = v_data;
		variableType = v_type;
		variableName = v_name;
	}
};

class CfgLoader
{
public:
	CfgLoader(string fname, vector<CfgVariable*> *varList, bool debug = 0);
};