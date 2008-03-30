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
#define __FILE_ID__ 0x03

/**
 * VM Frame
 *
 * VM frame operations.
 *
 * Log
 * ---
 *
 * 2007/01/09   #75: fo_isImport for thread support (P.Adelt)
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/04/20   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include "pm.h"


/***************************************************************
 * Functions
 **************************************************************/

PmReturn_t
frame_new(pPmObj_t pfunc, pPmObj_t *r_pobj)
{
    PmReturn_t retval = PM_RET_OK;
    int16_t fsize = 0;
    int8_t stacksz = (int8_t)0;
    int8_t nlocals = (int8_t)0;
    pPmCo_t pco = C_NULL;
    pPmFrame_t pframe = C_NULL;
    uint8_t const *paddr = C_NULL;
    uint8_t *pchunk;

    /* Get fxn's code obj */
    pco = ((pPmFunc_t)pfunc)->f_co;

    /* TypeError if passed func's CO is not a true COB */
    if (OBJ_GET_TYPE(*pco) != OBJ_TYPE_COB)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Get sizes needed to calc frame size */
    paddr = pco->co_codeimgaddr + CI_STACKSIZE_FIELD;
    stacksz = mem_getByte(pco->co_memspace, &paddr);

    /* Now paddr points to CI_NLOCALS_FIELD */
    nlocals = mem_getByte(pco->co_memspace, &paddr);
    fsize = sizeof(PmFrame_t) + (stacksz + nlocals - 1) * sizeof(pPmObj_t);

    /* Allocate a frame */
    retval = heap_getChunk(fsize, &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pframe = (pPmFrame_t)pchunk;

    /* Set frame fields */
    OBJ_SET_TYPE(*pframe, OBJ_TYPE_FRM);
    pframe->fo_back = C_NULL;
    pframe->fo_func = (pPmFunc_t)pfunc;
    pframe->fo_memspace = pco->co_memspace;

    /* Init instruction pointer, line number and block stack */
    pframe->fo_ip = pco->co_codeaddr;
    pframe->fo_line = 0;
    pframe->fo_blockstack = C_NULL;

    /* Get globals and attrs from the function object */
    pframe->fo_globals = ((pPmFunc_t)pfunc)->f_globals;
	if(pframe->fo_globals)
	{
		OBJ_INC_REF(pframe->fo_globals);
	}
    pframe->fo_attrs = ((pPmFunc_t)pfunc)->f_attrs;
	if(pframe->fo_attrs)
	{
		OBJ_INC_REF(pframe->fo_attrs);
	}

    /* Empty stack points to one past locals */
    pframe->fo_sp = &(pframe->fo_locals[nlocals]);

    /* By default, this is a normal frame, not an import call one */
    pframe->fo_isImport = 0;
	pframe->fo_numberOfLocals = 0;

    /* Return ptr to frame */
    *r_pobj = (pPmObj_t)pframe;
    return retval;
}


PmReturn_t
frame_delete(pPmObj_t pobj)
{
    PmReturn_t retval = PM_RET_OK;
	pPmFrame_t pframe = C_NULL;

	if (OBJ_GET_TYPE(*pobj) != OBJ_TYPE_FRM)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }
	pframe = (pPmFrame_t)pobj;
	if(pframe->fo_func) {
		if(OBJ_GET_TYPE(*pframe->fo_func) == OBJ_TYPE_FXN) {
			OBJ_DEC_REF((pPmObj_t)pframe->fo_func);
		}
	}
	if(pframe->fo_attrs) {
		OBJ_DEC_REF((pPmObj_t)pframe->fo_attrs);
	}
	retval = heap_freeChunk(pobj);
	return retval;
}
