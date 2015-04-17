/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */

#include "Plugin.h"

Plugin::Plugin(const std::string &sFilename) :
	m_hDLL(0),
	m_funcGetVersion(0),
	m_funcRegisterPlugin(0),
	m_pDLLRefCount(0) 
{
	m_hDLL = ::LoadLibrary(sFilename.c_str());
	if(!m_hDLL)
	{
		MessageBox(0, sFilename.c_str(), "Could not load library!", 0); 
		throw runtime_error(string("Could not load '") + sFilename + "'");
	}

	try 
	{
		m_funcGetVersion = reinterpret_cast<funcGetVersion*>(::GetProcAddress(m_hDLL, "getVersion"));
		m_funcRegisterPlugin = reinterpret_cast<funcRegisterPlugin*>(::GetProcAddress(m_hDLL, "registerPlugin"));
		if(! m_funcRegisterPlugin)
		{
			MessageBox(0, sFilename.c_str(), "Not valid plugin!", 0); 
			throw runtime_error(string("'") + sFilename + "' is not a valid plugin");
		}

			
		m_pDLLRefCount = new size_t(1);
	}
	catch(...) 
	{
		::FreeLibrary(m_hDLL);
		throw;
	}
} 

Plugin::Plugin(const Plugin &Other) :
	m_hDLL(Other.m_hDLL),
	m_funcGetVersion(Other.m_funcGetVersion),
	m_funcRegisterPlugin(Other.m_funcRegisterPlugin),
	m_pDLLRefCount(Other.m_pDLLRefCount) 
{
	++*m_pDLLRefCount;
}

Plugin::~Plugin() 
{
	if(!(--(*m_pDLLRefCount))) 
	{
		delete (m_pDLLRefCount);
		::FreeLibrary(m_hDLL);
	}
}