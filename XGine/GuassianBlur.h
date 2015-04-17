/* By Adam Micha³owski / J 0 |< e R / (c) 2009 */
#pragma once
#include "XGine.h"

struct XGINE_API GuassianBlur
{
	GuassianBlur();
	~GuassianBlur();

	u32		init(u32 sampleCount = 15);

	void	process(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output);
	void	processCC(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output);
	void	processLog(RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output);
	void	processD(LPDIRECT3DTEXTURE9 depthTexture, RenderTexture* input, RenderTexture* tempBlured, RenderTexture* output);
	void	processH(RenderTexture* input, RenderTexture* output);
	void	processV(RenderTexture* input, RenderTexture* output);
	void	processHC(RenderTexture* input, RenderTexture* output);
	void	processVC(RenderTexture* input, RenderTexture* output);
	void	processHL(RenderTexture* input, RenderTexture* output);
	void	processVL(RenderTexture* input, RenderTexture* output);

protected:
	u32			m_initialized;
	Shader*		m_shader;
	u32			m_uSampleCount;
};