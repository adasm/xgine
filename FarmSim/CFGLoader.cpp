#include "CFGLoader.h"

CfgLoader::CfgLoader(string fname, vector<CfgVariable*> *varList, bool debug)
{
	vector<CfgVariable*> variableList;
	variableList = *varList;
	if(variableList.size() < 1)
		return;
	Buffer buff;
	gEngine.kernel->fs->loadCached(fname, buff, g_appCache);
	Tokenizer tokenizer(buff.data, buff.size);
	gEngine.kernel->mem->freeBuff(buff);

	string token;

	while(tokenizer.nextToken(&token))
	{
		string varName = token;
		tokenizer.nextToken(&token);
		if(token != "=")
		{
			if(debug)
				gEngine.kernel->log->prnEx(LT_ERROR, "CfgLoader", "File '%s': Expected '=' after variable name '%s'.", fname.c_str(), varName.c_str());
			return;
		}
		else
		{
			int index = -1;
			for(int z = 0; z < variableList.size(); z++)
			{
				if(varName == variableList[z]->variableName)
					index = z;
			}
			if(index == -1)
			{
				if(debug)
					gEngine.kernel->log->prnEx(LT_ERROR, "CfgLoader", "File '%s': Unrecognizable variable name '%s'.", fname.c_str(), varName.c_str());
				tokenizer.nextToken(&varName);
			}
			else
			{
				switch(variableList[index]->variableType)
				{
					case V_INT:
						{
							tokenizer.nextToken(&token);
							std::stringstream conv;
							conv << token;
							conv >> *(int*)variableList[index]->variable;
							if(debug)
								gEngine.kernel->log->prnEx(LT_DEBUG, "CfgLoader", "variable name '%s'. Value: %d", varName.c_str(), *(int*)variableList[index]->variable);
						}
						break;
					case V_FLOAT:
						{
							tokenizer.nextToken(&token);
							std::stringstream conv;
							conv << token;
							conv >> *(float*)variableList[index]->variable;
							if(debug)
								gEngine.kernel->log->prnEx(LT_DEBUG, "CfgLoader", "variable name '%s'. Value: %f", varName.c_str(), *(float*)variableList[index]->variable);
						}
						break;
					case V_STRING:
						{
							tokenizer.nextToken((string*)variableList[index]->variable);
							if(debug)
								gEngine.kernel->log->prnEx(LT_DEBUG, "CfgLoader", "variable name '%s'. Value: %s", varName.c_str(), ((string*)variableList[index]->variable)->c_str());
						}
						break;
					case V_VEC3:
						{
							tokenizer.nextToken(&token);
							std::stringstream conv;
							conv << token;
							conv >> ((Vec3*)variableList[index]->variable)->x >> ((Vec3*)variableList[index]->variable)->y >> ((Vec3*)variableList[index]->variable)->z;
							if(debug)
								gEngine.kernel->log->prnEx(LT_DEBUG, "CfgLoader", "variable name '%s'. Value: %f", varName.c_str(), ((Vec3*)variableList[index]->variable)->x);
						}
						break;
					case V_VEC4:
						{
							tokenizer.nextToken(&token);
							std::stringstream conv;
							conv << token;
							conv >> ((Vec4*)variableList[index]->variable)->x >> ((Vec4*)variableList[index]->variable)->y >> ((Vec4*)variableList[index]->variable)->z >> ((Vec4*)variableList[index]->variable)->w;
							if(debug)
								gEngine.kernel->log->prnEx(LT_DEBUG, "CfgLoader", "variable name '%s'. Value: %f", varName.c_str(), ((Vec4*)variableList[index]->variable)->x);
						}
						break;
				}
			}
		}
	}
	varList = &variableList;
	gEngine.kernel->mem->freeBuff(buff);
}