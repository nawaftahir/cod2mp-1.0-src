#pragma once
#include "FxCamera.h"

struct Material;
struct GfxEntity;
struct GfxWorldVertex;
struct refdef_s;
typedef refdef_s refdef_t;
class FxArchive;

struct FxHelper
{
	FxHelper();
	void Init();
	void AdjustTime(int intime);
	void WarpTime(int intime);
	void CalcFrustumPlanes(const refdef_t *refdef, float zfar);
	void AdjustCamera(const refdef_t *refdef, float zfar);
	bool CullSphere(const vec3_t worldPos, float radius, int planeCount);
	bool CullSpherePreviousFrame(const vec3_t worldPos, float radius);
	bool CullCylinder(const vec3_t worldPos0, const vec3_t worldPos1, float radius0, float radius1, int planeCount);
	void Trace(trace_t *tr, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipEntNum, int flags);
	void AddFxToScene(GfxEntity *ent, XModel *model);
	void SetIgnorePrecacheErrors(bool ignore);
	void AddPlume(const float *origin, int score, const float *color, int duration);
	const char *GetMaterialName(Material *material);
	void Archive(FxArchive *arch);
	int GetMaterialSubimageCount(Material *material);
	bool IsMaterialRefractive(Material *material);
	void AddLightToScene(const vec3_t org, float radius, float red, float green, float blue);
	void AddPolyToScene(Material *material, int vertCount, const GfxWorldVertex *verts);
	void CameraShake(const vec3_t origin, float intensity, int radius, int time);
	int GetSeed();

	int time;
	int mTime;
	int mOldTime;
	int mFrameTime;
	int mTimeFrozen;
	FxCamera mCamera;
	FxCamera mPrevCamera;
	int mSeed;
	float adsZoomFactor;
};
