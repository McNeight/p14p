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

#ifndef __DICT_H__
#define __DICT_H__

/**
 * Dict Object Type
 *
 * Dict object type header.
 *
 * Log:
 *
 * 2007/01/09   #75: Printing support (P.Adelt)
 * 2002/04/30   First.
 */


/***************************************************************
 * Types
 **************************************************************/

/**
 * Dict
 *
 * Contains ptr to two seglists,
 * one for keys, the other for values;
 * and a length, the number of key/value pairs.
 */
typedef struct PmDict_s
{
    /** object descriptor */
    PmObjDesc_t od;
    /** number of key,value pairs in the dict */
    int16_t length;
    /** ptr to seglist containing keys */
    pSeglist_t d_keys;
    /** ptr to seglist containing values */
    pSeglist_t d_vals;
} PmDict_t,
 *pPmDict_t;


/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Clears the contents of a dict.
 * after this operation, the dict should in the same state
 * as if it were just created using dict_new().
 *
 * @param   pdict ptr to dict to clear.
 * @return  nothing
 */
PmReturn_t dict_clear(pPmObj_t pdict);

/**
 * Gets the value in the dict using the given key.
 *
 * @param   pdict ptr to dict to search
 * @param   pkey ptr to key obj
 * @param   r_pobj Return; addr of ptr to obj
 * @return  Return status
 */
PmReturn_t dict_getItem(pPmObj_t pdict, pPmObj_t pkey, pPmObj_t *r_pobj);

/**
 * Allocates space for a new Dict.
 * Return a pointer to the dict by reference.
 *
 * @param   r_pdict Return; Addr of ptr to dict
 * @return  Return status
 */
PmReturn_t dict_new(pPmObj_t *r_pdict);

PmReturn_t dict_delete(pPmObj_t pdict);


/**
 * Sets a value in the dict using the given key.
 *
 * If the dict already contains a matching key, the value is
 * replaced; otherwise the new key,val pair is inserted
 * at the front of the dict (for fast lookup).
 * In the later case, the length of the dict is incremented.
 *
 * @param   pdict ptr to dict in which (key,val) will go
 * @param   pkey ptr to key obj
 * @param   pval ptr to val obj
 * @return  Return status
 */
PmReturn_t dict_setItem(pPmObj_t pdict, pPmObj_t pkey, pPmObj_t pval);

#ifdef HAVE_PRINT
/**
 * Prints out a dict. Uses obj_print() to print elements.
 *
 * @param pobj Object to print.
 * @return Return status
 */
PmReturn_t dict_print(pPmObj_t pdict);
#endif /* HAVE_PRINT */

/**
 * Updates the destination dict with the key,value pairs from the source dict
 *
 * @param   pdestdict ptr to destination dict in which key,val pairs will go
 * @param   psourcedict ptr to source dict which has all key,val pairs to copy
 * @return  Return status
 */
PmReturn_t dict_update(pPmObj_t pdestdict, pPmObj_t psourcedict);

#endif /* __DICT_H__ */
