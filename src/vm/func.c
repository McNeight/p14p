/*
 * PyMite - A flyweight Python interpreter for 8-bit microcontrollers and more.
 * Copyright 2002 Dean Hall
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#undef __FILE_ID__
#define __FILE_ID__ 0x04

/**
 * Function Object Type
 *
 * Function object type operations.
 *
 * Log
 * ---
 *
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/05/04   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include "pm.h"


/***************************************************************
 * Functions
 **************************************************************/

PmReturn_t
func_new(pPmObj_t pco, pPmObj_t pglobals, pPmObj_t *r_pfunc)
{
    PmReturn_t retval = PM_RET_OK;
    pPmFunc_t pfunc = C_NULL;
    uint8_t *pchunk;
    pPmObj_t pobj;

    C_ASSERT(OBJ_GET_TYPE(*pco) != OBJ_TYPE_COB
             || OBJ_GET_TYPE(*pco) != OBJ_TYPE_NOB);
    C_ASSERT(OBJ_GET_TYPE(*pglobals) == OBJ_TYPE_DIC);

    /* Allocate a func obj */
    retval = heap_getChunk(sizeof(PmFunc_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pfunc = (pPmFunc_t)pchunk;

    /* Init func */
    OBJ_SET_TYPE(*pfunc, OBJ_TYPE_FXN);
    pfunc->f_co = (pPmCo_t)pco;
	OBJ_INC_REF(pco);

    /* Create attrs dict for regular func (not native) */
    if (OBJ_GET_TYPE(*pco) == OBJ_TYPE_COB)
    {
        retval = dict_new(&pobj);
        PM_RETURN_IF_ERROR(retval);
        pfunc->f_attrs = (pPmDict_t)pobj;

        /* Store the given globals dict */
        pfunc->f_globals = (pPmDict_t)pglobals;
		OBJ_INC_REF(pglobals);
    }
    else
    {
        pfunc->f_attrs = C_NULL;
    }

    /* Clear default args (will be set later, if at all) */
    pfunc->f_defaultargs = C_NULL;

    *r_pfunc = (pPmObj_t)pfunc;
    return PM_RET_OK;
}

PmReturn_t 
func_delete(pPmObj_t pobj) 
{
    PmReturn_t retval = PM_RET_OK;
    pPmFunc_t pfunc   = C_NULL;

	if (OBJ_GET_TYPE(*pobj) != OBJ_TYPE_FXN)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
	}
	pfunc = (pPmFunc_t)pobj;
	if(pfunc->f_attrs) {
		OBJ_DEC_REF((pPmObj_t)pfunc->f_attrs);
	}
	if(pfunc->f_co) {
		OBJ_DEC_REF((pPmObj_t)pfunc->f_co);
	}
	retval = heap_freeChunk(pobj);	
	return retval;
}


/*
 * This function requires that all parameters have their
 * expected type.  (parm types already checked).
 *
 * Deprecated, there is no replacement.
 */
PmReturn_t
class_new(pPmObj_t pmeths,
          pPmObj_t pbases, pPmObj_t pname, pPmObj_t *r_pclass)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pkey = C_NULL;
    uint8_t const *btstr = (uint8_t const *)"__bt";
    uint8_t const *nmstr = (uint8_t const *)"__nm";

    /* Ensure types */
    if ((OBJ_GET_TYPE(*pmeths) != OBJ_TYPE_DIC) ||
        (OBJ_GET_TYPE(*pbases) != OBJ_TYPE_TUP) ||
        (OBJ_GET_TYPE(*pname) != OBJ_TYPE_STR))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Allocate a class obj */
    retval = heap_getChunk(sizeof(PmFunc_t), (uint8_t **)r_pclass);
    PM_RETURN_IF_ERROR(retval);
    OBJ_SET_TYPE(**r_pclass, OBJ_TYPE_CLO);

    /* Class has no access to its CO */
    ((pPmFunc_t)*r_pclass)->f_co = C_NULL;
    ((pPmFunc_t)*r_pclass)->f_attrs = (pPmDict_t)pmeths;

    /* Store base tuple in __bt slot */
    retval = string_new(&btstr, &pkey);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem((pPmObj_t)((pPmFunc_t)*r_pclass)->f_attrs,
                          pkey, (pPmObj_t)pbases);
    PM_RETURN_IF_ERROR(retval);

    /* Store the name of the class in the __nm slot */
    retval = string_new(&nmstr, &pkey);
    PM_RETURN_IF_ERROR(retval);
    retval = dict_setItem((pPmObj_t)((pPmFunc_t)*r_pclass)->f_attrs,
                          pkey, (pPmObj_t)pname);
    return retval;
}
