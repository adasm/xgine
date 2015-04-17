#pragma once
#include "XGine.h"

enum XGINE_API FuncCustomShader
{
	FUNC_DEFERRED,
	FUNC_FORWARD,
	FUNC_RAW
};

struct XGINE_API IEntity : Node<IEntity>
{
	virtual void				update(ICamera* cam = 0)			= 0;
	virtual u32					renderDeferred()					= 0;
	virtual u32					renderForward(Light* light = 0)		= 0;
	virtual u32					renderRaw()							= 0;
	virtual u32					hasShadow()							= 0;
	virtual	void				setSunLight(Light *light)			= 0;
	virtual u32					selfRendering()						= 0;
	virtual REND_TYPE			getRendType()						= 0;
	virtual Mesh*				getMesh()							= 0;
	virtual Shader*				getShader()							= 0;
	virtual Texture*			getTexture()						= 0;
	virtual Material*			getMaterial()						= 0;
	virtual REND_SHADER_TYPE	getShaderType()									{ return (REND_SHADER_TYPE)1; }
	virtual u32					setupCustomShader(FuncCustomShader func)		{ return 0; }
};
