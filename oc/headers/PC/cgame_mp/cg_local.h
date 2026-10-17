#ifndef CLEAN_PC_CGAME_MP_CG_LOCAL_H
#define CLEAN_PC_CGAME_MP_CG_LOCAL_H

#include "common_types.h"

extern cg_t cgArray[1];
/* `cg` is a const pointer to the single cgArray element, so `cg->field` folds into one
 * absolute access. Deliberately a plain identifier and NOT a macro: the functions that
 * take a `cg_t *cg` PARAMETER must still be able to shadow it under normal C scoping. */
static cg_t * const cg = &cgArray[0];
extern cgs_t cgsArray[1];
static cgs_t * const cgs = &cgsArray[0];
extern centity_t cg_entitiesArray[1][1024];
static centity_t * const cg_entities = cg_entitiesArray[0];

#endif
