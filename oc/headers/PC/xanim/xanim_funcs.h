#ifndef CLEAN_PC_XANIM_XANIM_FUNCS_H
#define CLEAN_PC_XANIM_XANIM_FUNCS_H

#include "common_types.h"
#include "bytematch.h"

#ifndef XANIM_CALC_ABI
#  define XANIM_CALC_ABI COD2_REGPARM(3) COD2_SSEREGPARM
#endif

void DObjInit(void);
void DObjShutdown(void);
void DObjAbort(void);
Bool DObjIgnoreCollision(const DObj *obj, int modelIndex);
int DObjSkelIsBoneUpToDate(DObj_s *obj, int boneIndex);
int DObjSkelAreBonesUpToDate(const DObj *obj, int *partBits);
void DObjFree(DObj_s *obj);
int DObjGetAllocSkelSize(const DObj *obj);
qboolean DObjSkelExists(DObj *obj, int timeStamp);
void DObjSkelClear(const DObj *obj);
void DObjCreateSkel(const DObj *obj, char *buf, int timeStamp);
int DObjGetNumModels(const DObj *obj);
XModel *DObjGetModel(const DObj *obj, int modelIndex);
DObjAnimMat* DObjGetRotTransArray(const DObj_s *obj);
int DObjGetMatOffset(const DObj *obj, int modelIndex);
void DObjGetBoneInfo(const DObj *obj, XBoneInfo **boneInfo);
int DObjGetNumSurfaces(const DObj *obj, char *lods);
struct XSurface_s *DObjGetSurface(const DObj *obj, int modelIndex, int subMatIndex, int lod);
const char *DObjGetSurfaceName(DObj *obj, int modelIndex, int subMatIndex, int lod);
int DObjGetBoneIndex(const DObj *obj, unsigned int boneName);
XAnimTree *DObjGetTree(const DObj *obj);
int DObjBad(const DObj *obj);
int DObjNumBones(const DObj *obj);
int DObjGetLodForDist(const DObj *obj, int modelIndex, float dist);
float DObjGetLodOutDist(const DObj *obj);
int DObjHasContents(DObj *obj, int contentmask);
void DObjSetModel(DObj *obj, const XModel *model);
void DObjGetBounds(const DObj *obj, vec_t *mins, vec_t *maxs);
void DObjGeomTraceline(DObj *obj, vec_t *localStart, vec_t *localEnd, int contentmask, DObjTrace *results);
int DObjGetSurfaces(const DObj *obj, DSurface *surfaces, int *partBits, char *lods);
void DObjCompleteHierarchyBits(DObj_s *obj, int *partBits);
void DObjGetHierarchyBits(DObj *obj, int boneIndex, int *partBits);
void DObjSetLocalTagInternal(const DObj *obj, const vec_t *trans, const vec_t *angles, int boneIndex);
qboolean DObjSetLocalTag(const DObj *obj, int *partBits, unsigned int tagName, const vec_t *trans, const vec_t *angles);
qboolean DObjSetControlTagAngles(const DObj_s *obj, int *partBits, unsigned int boneIndex, float *angles);
void DObjDumpInfo(const DObj *obj);
void DObjCreate(DObjModel_s *dobjModels, unsigned int numModels, XAnimTree_s *tree, char *buf, unsigned int entnum);
void DObjCalcSkel(DObj *obj, int *partBits);
void DObjTraceline(DObj *obj, vec_t *start, vec_t *end, unsigned char *priorityMap, DObjTrace *trace);

void XAnimInit(void);
void XAnimShutdown(void);
void XAnimAbort(void);
void XAnimFree(XAnimParts *parts);
void XAnimBlend(XAnim *anims, unsigned int animIndex, const char *name, unsigned int children, unsigned int num, unsigned int flags);
XAnim *XAnimCreateAnims(const char *debugName, int size, Alloc_t Alloc);
void XAnimFreeList(XAnim *anims);
XAnim *XAnimGetAnims(const XAnimTree *tree);
float XAnimGetLength(const XAnim *anims, unsigned int animIndex);
float XAnimGetTime(const XAnimTree *tree, unsigned int animIndex);
float XAnimGetWeight(const XAnimTree_s *tree, unsigned int animIndex);
Bool XAnimHasFinished(const XAnimTree *tree, unsigned int animIndex);
int XAnimGetNumChildren(const XAnim *anims, unsigned int animIndex);
unsigned int XAnimGetChildAt(const XAnim *anims, unsigned int animIndex, unsigned int childIndex);
const char *XAnimGetAnimName(const XAnim *anims, unsigned int animIndex);
const char *XAnimGetAnimTreeDebugName(const XAnim *anims);
unsigned int XAnimGetAnimTreeSize(const XAnim *anims);
int DObjGetClientNotifyList(XAnimNotify **notifyList);
Bool XAnimIsPrimitive(XAnim *anims, unsigned int animIndex);
void XAnimSetTime(XAnimTree *tree, unsigned int animIndex, float time);
void XAnimSetAnimRate(XAnimTree *tree, unsigned int animIndex, float rate);
bool XAnimIsLooped(const XAnim_s *anim, unsigned int animIndex);
Bool XAnimNotetrackExists(const XAnim *anims, unsigned int animIndex, unsigned int name);
const char *XAnimGetAnimDebugName(const XAnim *anims, unsigned int animIndex);
XAnimTree *XAnimCreateTree(XAnim *anims, Alloc_t Alloc);
XAnimParts *XAnimPrecache(const char *name, Alloc_t Alloc);
void XAnimCreate(XAnim *anims, unsigned int animIndex, const char *name);
int XAnimGetLengthMsec(const XAnim *anims, unsigned int anim);
void XAnimClearGoalWeight(XAnimTree *tree, unsigned int animIndex, float blendTime);
void XAnimGetAbsDelta(const XAnim *anims, unsigned int animIndex, vec_t *rot, vec_t *trans, float time);
void XAnimGetRelDelta(const XAnim *anims, unsigned int animIndex, vec_t *rot, vec_t *trans, float time1, float time2);
__attribute_regparm__(3) __attribute_sseregparm__ void XAnimCalcRelDeltaParts(const XAnimParts *parts, XAnimSimpleRotPos *rotPos, int quatIndex, float weightScale, float time1, float time2);
__attribute_regparm__(3) __attribute_sseregparm__ void XAnimCalcDeltaTree(const XAnimTree *tree, unsigned int animIndex, float weightScale, const Bool bClear, Bool bNormQuat, XAnimSimpleRotPos *rotPos);
void XAnimCalcAbsDelta(XAnimTree *tree, unsigned int animIndex, vec_t *rot, vec_t *trans);
void XAnimCalcDelta(XAnimTree *tree, unsigned int animIndex, vec_t *rot, vec_t *trans, int bUseGoalWeight);
void DObjInitServerTime(DObj *obj, float dtime);
void XAnimSetCompleteGoalWeight(XAnimTree *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void XAnimCloneAnimTree(const XAnimTree *from, XAnimTree *to);
void XAnimFreeTree(XAnimTree *tree, Free_t Free);
void XAnimClearTree(XAnimTree *tree);
void XANIM_CALC_ABI XAnimCalc(const DObj *obj, unsigned int animIndex, float weightScale, XAnimPart (*rotTransArray)(), int bClear, int bNormQuat, XAnimCalcAnimInfo *info, int rotTransArrayIndex);
void DObjCalcAnim(const DObj *obj, int *partBits);
void XAnimClearTreeGoalWeights(XAnimTree *tree, unsigned int animIndex, float blendTime);
void XAnimClearTreeGoalWeightsStrict(XAnimTree *tree, unsigned int animIndex, float blendTime);
int XAnimSetCompleteGoalWeightKnobAll(XAnimTree *tree, unsigned int animIndex, unsigned int rootIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, int bRestart);
int XAnimSetGoalWeight(XAnimTree *tree, unsigned int animIndex, float goalWeight, float goalTime, float rate, unsigned int notifyName, unsigned int notifyType, int bRestart);
void DObjDisplayAnim(DObj *obj);
void XAnimSetupSyncNodes(XAnim *anims);
void DObjUpdateClientInfo(DObj *obj, float dtime);
int DObjUpdateServerInfo(DObj *obj, float dtime, int bNotify);

XAnimParts *XAnimLoadFile(const char *name, Alloc_t Alloc);

qboolean XModelBad(XModel *model);
void XModelPartsFree(XModelParts *model);
XModelParts *XModelPartsFindData(const char *name);
void XModelPartsSetData(const char *name, XModelParts *modelParts, Alloc_t Alloc);
XModelSurfs *XModelSurfsFindData(const char *name);
void XModelSurfsSetData(const char *name, XModelSurfs *modelSurfs, Alloc_t Alloc);
XModel *XModelPrecache(const char *name, Alloc_t Alloc, Alloc_t AllocColl);
int XModelBoneNames(XModel *model);
int XModelGetBoneIndex(const XModel *model, unsigned int name);
const char *XModelGetLodName(const XModel *model, int lod);
int XModelGetContents(const XModel *model);
const struct trXSkin_t *XModelGetSkins(const XModel *model);
int XModelGetMemUsage(const XModel *model);
qboolean Com_ValidXModelName(const char *name);
void XModelGetBounds(const XModel *model, vec_t *mins, vec_t *maxs);
void XModelFree(XModel *model);
int XModelTraceLine(const XModel *model, trace_t *results, const DObjAnimMat *boneMtxList, vec_t *localStart, vec_t *localEnd, int contentmask);

int XModelGetStaticBounds(const XModel *model, vec3_t *axis, vec_t *mins, vec_t *maxs);
XModel *XModelLoad(const char *name, Alloc_t Alloc, Alloc_t AllocColl);

const char *XModelGetName(const XModel *model);
unsigned char XModelGetFlags(const XModel *model);
const char *XModelGetSurfaceName(const XModel *model, int subMatIndex, int lod);
int XModelGetSurfaces(const XModel *model, struct XSurface_s ***surfaces, int lod, int **partBits);
int XModelGetNumLods(const XModel *model);
int XModelNumBones(const XModel *model);
const DObjAnimMat *XModelGetBasePose(const XModel *model);
const DObjAnimMat *XModelGetBasePoseBone(const XModel *model, int skelMatBoneOffset);
void XModelSetTestLods(int lodLevel, float dist);
float XModelGetLodOutDist(const XModel *model);
int XModelGetLodForDist(const XModel *model, float dist);

#endif
