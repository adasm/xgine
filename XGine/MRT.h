/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API MRT
{
	MRT();
	~MRT();

	u32		init(u32 numMRTs, D3DFORMAT *mrtFormat);
	void	begin(u32 depthStencilEnabled = 1);
	void	end();
	void	bindDepthStencil();
	void	bindTextures();
	void	close();
	
	inline u32					isInitialized()		{ return m_initialized; }
	inline LPDIRECT3DTEXTURE9	getRT(u32 index)	{ return m_rt[index]; } //Index = value [0, numMRTs-1]
	inline LPDIRECT3DSURFACE9	getDepth()			{ return m_rtDepth; }
	inline u32					getNumRT()			{ return m_numMRTs; }
	inline D3DFORMAT*			getFormat()			{ return m_mrtFormat; }

protected:
	u32						m_initialized;
	LPDIRECT3DTEXTURE9		m_rt[4];
	LPDIRECT3DSURFACE9		m_rtSurface[4];
	LPDIRECT3DSURFACE9		m_rtDepth;
	D3DFORMAT				m_mrtFormat[4];
	u32						m_numMRTs;

	LPDIRECT3DSURFACE9		m_pOldDSSurface;
	LPDIRECT3DSURFACE9		m_pOldRenderTarget;
};