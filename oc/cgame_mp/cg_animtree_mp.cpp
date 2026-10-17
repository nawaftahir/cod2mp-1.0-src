#include "common_types.h"
#include "imports.h"
#include "headers/PC/cgame_mp/cg_local.h"

extern void Scr_BeginLoadAnimTrees(int param);
extern void Com_SafeClientDObjFree(int entNum);
extern void XAnimFreeTree(XAnimTree_s *tree, void (*Free)(void *, int));
extern void MT_Free(void *ptr, int size);

void CGScr_LoadAnimTrees(void)
{
    Scr_BeginLoadAnimTrees(0);
}

void CG_SetDObjInfo(int iEntNum, int iEntType, struct XModel *pXModel)
{
    cg->iEntityLastType[iEntNum] = iEntType;
    cg->pEntityLastXModel[iEntNum] = pXModel;
}

qboolean CG_CheckDObjInfoMatches(int iEntNum, int iEntType, struct XModel *pXModel)
{
    if (cg->iEntityLastType[iEntNum] == iEntType && cg->pEntityLastXModel[iEntNum] == pXModel)
        return 1;
    return 0;
}

void CG_SafeDObjFree(int iEntNum)
{
    Com_SafeClientDObjFree(iEntNum);

    cg->iEntityLastType[iEntNum] = 0;
    cg->pEntityLastXModel[iEntNum] = 0;

    if (cg_entities[iEntNum].tree) {
        XAnimFreeTree(cg_entities[iEntNum].tree, MT_Free);
        cg_entities[iEntNum].tree = 0;
    }
}

void CG_FreeClientDObjInfo(void)
{
    int i;
    for (i = 0; i < 64; i++)
        CG_SafeDObjFree(i);
}

void CG_FreeEntityDObjInfo(void)
{
    int i;
    for (i = 64; i < 1024; i++)
        CG_SafeDObjFree(i);
}

