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

#ifndef __FUNC_H__
#define __FUNC_H__

/**
 * Function Object Type
 *
 * Function object type header.
 *
 * Log:
 *
 * 2002/05/04   First.
 */

/***************************************************************
 * Types
 **************************************************************/

/**
 * Function obj
 *
 * A function is like an instance of a code obj.
 * Contains ptr to its code obj and has its own attributes dict.
 *
 * The first (__main__) module that is executed has a function obj
 * created for it to execute the bytecode which builds the module.
 */
typedef struct PmFunc_s
{
    /** Object descriptor */
    PmObjDesc_t od;

    /** Ptr to code obj */
    pPmCo_t f_co;

    /** Ptr to attribute dict */
    pPmDict_t f_attrs;

    /** Ptr to globals dict */
    pPmDict_t f_globals;

    /** Ptr to tuple holding default args */
    pPmTuple_t f_defaultargs;
} PmFunc_t,
 *pPmFunc_t;


/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Creates a Function Obj for the given Code Obj.
 * Allocate space for a Func obj and fill the fields.
 *
 * @param   pco ptr to code obj
 * @param   pglobals ptr to globals dict (from containing func/module)
 * @return  ptr to new func obj
 */
PmReturn_t func_new(pPmObj_t pco, pPmObj_t pglobals, pPmObj_t *r_pfunc);

PmReturn_t func_delete(pPmObj_t pobj);

/**
 * Creates a new Class object from the methods dict, bases tuple,
 * and name string.
 *
 * @param   pmeths ptr to methods dict.
 * @param   pbases ptr to bases tuple.
 * @param   pname ptr to name string.
 * @param   r_pclass Return by ref, ptr to new class
 * @return  ptr to new class obj.
 */
PmReturn_t class_new(pPmObj_t pmeths,
                     pPmObj_t pbases, pPmObj_t pname, pPmObj_t *r_pclass);

#endif /* __FUNC_H__ */
