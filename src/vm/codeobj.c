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
#define __FILE_ID__ 0x01

/**
 * CodeObj Type
 *
 * CodeObj type operations.
 *
 * Log
 * ---
 *
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/06/04   making co_names a tuple,
 *              removing nameoff and codeoff from img.
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
co_loadFromImg(PmMemSpace_t memspace, uint8_t const **paddr, pPmObj_t parentobj, pPmObj_t *r_pco)
{
    PmReturn_t retval = PM_RET_OK;
    pPmObj_t pobj;
    pPmCo_t pco = C_NULL;
    uint8_t *pchunk;

    /* Store ptr to top of code img (less type byte) */
    uint8_t const *pci = *paddr - 1;

    /* Get size of code img */
    uint16_t size = mem_getWord(memspace, paddr);

    /* Allocate a code obj */
    retval = heap_getChunk(sizeof(PmCo_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pco = (pPmCo_t)pchunk;

    /* Fill in the CO struct */
    OBJ_SET_TYPE(*pco, OBJ_TYPE_COB);
    pco->co_memspace     = memspace;
	pco->co_codeimgaddr  = pci;
	pco->co_parentobject = parentobj;
	if(parentobj) 
	{
		OBJ_INC_REF(parentobj);
	}

    /* Load names (tuple obj) */
    *paddr = pci + CI_NAMES_FIELD;
    retval = obj_loadFromImg(memspace, paddr, parentobj, &pobj);
    PM_RETURN_IF_ERROR(retval);
    pco->co_names = (pPmTuple_t)pobj;

    /* Load consts (tuple obj) assume it follows names */
    retval = obj_loadFromImg(memspace, paddr, parentobj, &pobj);
    PM_RETURN_IF_ERROR(retval);
    pco->co_consts = (pPmTuple_t)pobj;

    /* Start of bcode always follows consts */
    pco->co_codeaddr = *paddr;

    /* Set addr to point one past end of img */
    *paddr = pci + size;

    *r_pco = (pPmObj_t)pco;
    return PM_RET_OK;
}

void co_trace(pPmObj_t pobj)
{
	pPmCo_t pco = C_NULL;

	pco = (pPmCo_t)pobj;
}

PmReturn_t co_delete(pPmObj_t pobj)
{
	PmReturn_t retval = PM_RET_OK;
	pPmCo_t pco       = C_NULL;

	if(OBJ_GET_TYPE(*pobj) != OBJ_TYPE_COB) 
	{
		PM_RAISE(retval, PM_RET_EX_TYPE);
		return retval;
	}
	pco = (pPmCo_t)pobj;
	if(pco->co_parentobject) 
	{
		OBJ_DEC_REF(pco->co_parentobject);
	}
	if(pco->co_names)
	{
		OBJ_DEC_REF((pPmObj_t)pco->co_names);
	}
	if(pco->co_consts)
	{
		OBJ_DEC_REF((pPmObj_t)pco->co_consts);
	}
	heap_freeChunk(pobj);
	return retval;
}


PmReturn_t
no_loadFromImg(PmMemSpace_t memspace, uint8_t const **paddr, pPmObj_t *r_pno)
{
    PmReturn_t retval = PM_RET_OK;
    pPmNo_t pno = C_NULL;
    uint8_t *pchunk;

    /* Allocate a code obj */
    retval = heap_getChunk(sizeof(PmNo_t), &pchunk);
    PM_RETURN_IF_ERROR(retval);
    pno = (pPmNo_t)pchunk;

    /* Fill in the NO struct */
    OBJ_SET_TYPE(*pno, OBJ_TYPE_NOB);
    pno->no_argcount = mem_getByte(memspace, paddr);

    /* Get index into native fxn table */
    pno->no_funcindx = (int16_t)mem_getWord(memspace, paddr);

    *r_pno = (pPmObj_t)pno;
    return PM_RET_OK;
}
