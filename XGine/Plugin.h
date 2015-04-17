/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#pragma once
#include "XGine.h"

typedef struct XGINE_API Plugin 
{
	Plugin(const std::string &sFilename);
	Plugin(const Plugin &Other);
	~Plugin();

	int		getVersion() const				{ return m_funcGetVersion();    }
	void	registerPlugin(Engine &engine)	{ m_funcRegisterPlugin(engine); }    

private:
	Plugin &operator =(const Plugin &Other);

	typedef int  funcGetVersion();
	typedef void funcRegisterPlugin(Engine &);

	HMODULE					 m_hDLL;
	size_t					*m_pDLLRefCount;
	funcGetVersion			*m_funcGetVersion;
	funcRegisterPlugin		*m_funcRegisterPlugin;
}*Plugin_ptr;