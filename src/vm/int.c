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
#define __FILE_ID__ 0x08

/**
 * Integer Object Type
 *
 * Integer object type operations.
 *
 * Log
 * ---
 *
 * 2007/01/09   #75: Printing support (P.Adelt)
 * 2006/08/29   #15 - All mem_*() funcs and pointers in the vm should use
 *              unsigned not signed or void
 * 2002/05/04   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include "pm.h"

#if defined(HAVE_PRINT) && defined(TARGET_AVR)
#include <avr/pgmspace.h>
#endif


/***************************************************************
 * Functions
 **************************************************************/

PmReturn_t
int_dup(pPmObj_t pint, pPmObj_t *r_pint)
{
    PmReturn_t retval = PM_RET_OK;

    /* Allocate new int */
    retval = heap_getChunk(sizeof(PmInt_t), (uint8_t **)r_pint);
    PM_RETURN_IF_ERROR(retval);

    /* Copy value */
    OBJ_SET_TYPE(**r_pint, OBJ_TYPE_INT);
    ((pPmInt_t)*r_pint)->val = ((pPmInt_t)pint)->val;
    return retval;
}


PmReturn_t
int_new(int32_t n, pPmObj_t *r_pint)
{
    PmReturn_t retval = PM_RET_OK;

    /* If n is 0,1,-1, return static int objects from global struct */
    if (n == 0)
    {
        *r_pint = PM_ZERO;
		OBJ_INC_REF(*r_pint);
        return PM_RET_OK;
    }
    if (n == 1)
    {
        *r_pint = PM_ONE;
		OBJ_INC_REF(*r_pint);
        return PM_RET_OK;
    }
    if (n == -1)
    {
        *r_pint = PM_NEGONE;
		OBJ_INC_REF(*r_pint);
        return PM_RET_OK;
    }

    /* Else create and return new int obj */
    retval = heap_getChunk(sizeof(PmInt_t), (uint8_t **)r_pint);
    PM_RETURN_IF_ERROR(retval);
    OBJ_SET_TYPE(**r_pint, OBJ_TYPE_INT);
    ((pPmInt_t)*r_pint)->val = n;
    return retval;
}


PmReturn_t
int_positive(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(*pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(((pPmInt_t)pobj)->val, r_pint);
}


PmReturn_t
int_negative(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(*pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(-((pPmInt_t)pobj)->val, r_pint);
}


PmReturn_t
int_bitInvert(pPmObj_t pobj, pPmObj_t *r_pint)
{
    PmReturn_t retval;

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(*pobj) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    /* Create new int obj */
    return int_new(~((pPmInt_t)pobj)->val, r_pint);
}

#ifdef HAVE_PRINT
PmReturn_t
int_print(pPmObj_t pint)
{
    /* 2^31-1 has 10 decimal digits, plus sign and zero byte */
    uint8_t tBuffer[10 + 1 + 1];
    uint8_t bytesWritten;
    uint8_t k;
    PmReturn_t retval = PM_RET_OK;

    C_ASSERT(pint != C_NULL);

    /* Raise TypeError if obj is not an int */
    if (OBJ_GET_TYPE(*pint) != OBJ_TYPE_INT)
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

#ifdef TARGET_AVR
    bytesWritten = snprintf_P((uint8_t *)&tBuffer, sizeof(tBuffer),
                              PSTR("%li"), ((pPmInt_t)pint)->val);
#else
    /* This does not use snprintf because glibc's snprintf is only
     * included for compiles without strict-ansi.
     */
    bytesWritten =
        sprintf((void *)&tBuffer, "%li", (long int)((pPmInt_t)pint)->val);
#endif /* !TARGET_AVR */


    /* Sanity check */
    C_ASSERT(bytesWritten != 0);
    C_ASSERT(bytesWritten < sizeof(tBuffer));

    for (k = (uint8_t)0; k < bytesWritten; k++)
    {
        retval = plat_putByte(tBuffer[k]);
        PM_RETURN_IF_ERROR(retval);
    }
    return PM_RET_OK;
}


PmReturn_t
int_printHexByte(uint8_t b)
{
    uint8_t nibble;
    PmReturn_t retval;

    nibble = (b >> 4) + '0';
    if (nibble > '9')
        nibble += ('a' - '0' - 10);
    retval = plat_putByte(nibble);
    PM_RETURN_IF_ERROR(retval);

    nibble = (b & (uint8_t)0x0F) + '0';
    if (nibble > '9')
        nibble += ('a' - '0' - (uint8_t)10);
    retval = plat_putByte(nibble);
    return retval;
}


PmReturn_t
_int_printHex(int32_t n)
{
    PmReturn_t retval;

    /* Print the hex value, most significant byte first */
    retval = int_printHexByte((n >> (uint8_t)24) & (uint8_t)0xFF);
    PM_RETURN_IF_ERROR(retval);
    retval = int_printHexByte((n >> (uint8_t)16) & (uint8_t)0xFF);
    PM_RETURN_IF_ERROR(retval);
    retval = int_printHexByte((n >> (uint8_t)8) & (uint8_t)0xFF);
    PM_RETURN_IF_ERROR(retval);
    retval = int_printHexByte(n & (uint8_t)0xFF);

    return retval;
}


PmReturn_t
int_printHex(pPmObj_t pint)
{
    C_ASSERT(OBJ_GET_TYPE(*pint) == OBJ_TYPE_INT);

    /* Print the integer object */
    return _int_printHex(((pPmInt_t)pint)->val);
}
#endif /* HAVE_PRINT */


PmReturn_t
int_pow(pPmObj_t px, pPmObj_t py, pPmObj_t *r_pn)
{
    int32_t x;
    int32_t y;
    int32_t n;
    PmReturn_t retval;

    /* Raise TypeError if args aren't ints */
    if ((OBJ_GET_TYPE(*px) != OBJ_TYPE_INT)
        || (OBJ_GET_TYPE(*py) != OBJ_TYPE_INT))
    {
        PM_RAISE(retval, PM_RET_EX_TYPE);
        return retval;
    }

    x = ((pPmInt_t)px)->val;
    y = ((pPmInt_t)py)->val;

    /* Raise Value error if exponent is negative */
    if (y < 0)
    {
        PM_RAISE(retval, PM_RET_EX_VAL);
        return retval;
    }

    /* Calculate x raised to y */
    n = 1;
    while (y > 0)
    {
        n = n * x;
        y--;
    }
    retval = int_new(n, r_pn);

    return retval;
}
