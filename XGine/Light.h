#pragma once
#include "XGine.h"

enum XGINE_API LIGHT_TYPE
{
	LIGHT_UNKNOWN = 0,
	LIGHT_POINT = 1,
	LIGHT_SPOT = 2,
	LIGHT_DIR = 3
};

typedef struct XGINE_API Light
{
	Light(LIGHT_TYPE type, u32 castsShadows = 0, const Vec4& color = Vec4(1,1,1,1), const Vec3& position = Vec3(0,0,0), f32 range = 50.0f, const Vec3& direction = Vec3(0,0,0), f32 phi = 0);
	~Light();

	inline void			setColor(const Vec4* color) { *m_color = *color; }
	inline void			setEnabled(u32 enabled) { m_enabled = enabled; }
	inline u32			getEnabled() { return m_enabled; }
	void				setPos(const Vec3* position);
	void				setPos(const Vec4* position);
	void				setDir(const Vec3* direction);
	inline LIGHT_TYPE	getType() { return m_type; }
	inline Vec4*		getColor() { return m_color; }
	inline Vec3*		getPos() { return m_position; }
	inline Vec4*		getPosV4() { return m_positionV4; }
	inline Vec3*		getDir() { return m_direction; }
	inline f32			getRange() { return m_range; }
	inline f32			getInvRange() { return m_invRange; }
	inline f32			getPhi() { return m_phi; }
	inline f32			getCosPhiHalf() { return m_cosPhiHalf; }
	inline Mat*			getViewProjMat() { return m_viewProjection; }
	inline Mat*			getViewMat() { return m_view; }
	inline u32			castsShadows() { return	m_castsShadows; }
	
	inline BoundingCone*	getBoundingCone() const { return m_cone; }
	inline BoundingBox*		getBoundingBox() const { return m_box; }
	inline const Plane*		getConeCapping() const { return m_coneCapping; }
	inline i32				getConeCappingNum() const { return m_coneCappingNum; }
	inline bool				hasPos() { return m_type == LIGHT_SPOT || m_type == LIGHT_POINT; }
	inline bool				hasDir() { return m_type == LIGHT_SPOT || m_type == LIGHT_DIR; }
	inline bool				hasRange() { return m_type == LIGHT_SPOT || m_type == LIGHT_POINT; }
	inline bool				hasPhi() { return m_type == LIGHT_SPOT; }

	void				update(const ICamera* camera);
	void				renderDepth(RenderList* rl);
	void				renderVolume();

	inline RenderTexture*	getDepthMap() { return (r_enabledVarianceShadowMapping && !r_enabledExponentialShadowMapping)?(m_VdepthMap):(m_depthMap); }

protected:
	LIGHT_TYPE		m_type;
	bool			m_needsUpdate;
	Vec4*			m_color;				//psd
	Vec3*			m_position;				//ps-
	Vec4*			m_positionV4;			//ps-
	Vec3*			m_direction;			//-sd , normalized
	f32				m_range;				//psd
	f32				m_invRange;				//ps-
	f32				m_phi;					//-s-
	f32				m_cosPhiHalf;			//-s-
	Mat*			m_viewProjection;		//-s-
	Mat*			m_view;					//-s-
	BoundingCone*	m_cone;					//-s-
	BoundingBox*	m_box;					//psd
	Plane			m_coneCapping[6];		//psd
	i32				m_coneCappingNum;		//psd
	u32				m_castsShadows;
	RenderTexture*	m_depthMap;
	RenderTexture*	m_VdepthMap;
	LPD3DXMESH		lpVolumeMesh;
	u32				m_enabled;

	void calculateConeCapping(const ICamera* camera);
	void calculateViewProjectionMatrix();
	void calculateBoundingCone();
	void calculateBoundingBox(const Vec3* frustumVerts);
} *Light_ptr;

struct LighstList
{
	vector< Light* > lights;
};