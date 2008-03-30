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

#ifndef __PM_H__
#define __PM_H__

/**
 * PyMite Header
 *
 * Include things that are needed by nearly everything.
 *
 * Log
 * ---
 *
 * 2006/09/16   #16: Create pm_init() that does the initial housekeeping
 * 2006/08/31   #9: Fix BINARY_SUBSCR for case stringobj[intobj]
 * 2006/08/30   #6: Have pmImgCreator append a null terminator to image list
 * 2002/05/04   Merged most of contents to respective files.
 *              Started doxygen comments.
 * 2002/04/15   Merging object descriptor into tops of objects.
 * 2002/02/06   First.
 */

/***************************************************************
 * Includes
 **************************************************************/

#include <stdint.h>
#include <stdio.h>


/***************************************************************
 * Configurables
 **************************************************************/

/**
 * Value indicating the release of PyMite
 *
 * This value should be incremented for every public release.
 * It helps locate a defect when used in conjunction with a fileID
 * and line number.
 */
#define PM_RELEASE 5


/***************************************************************
 * Constants
 **************************************************************/

/** null for C code */
#define C_NULL 0

/** false for C code */
#define C_FALSE 0

/** true for C code */
#define C_TRUE (!C_FALSE)

/** Comparison result is that items are the same */
#define C_SAME (int8_t)0

/** Comparison result is that items differ */
#define C_DIFFER (int8_t)-1

/** PORT inline for C code */
#define INLINE //__inline__


/***************************************************************
 * Macros
 **************************************************************/

/**
 * Returns an exception error code and stores debug data
 *
 * This macro must be used as an rval statement.  That is, it must
 * be used after an assignment such as "retval = " or a return statement
 */
#if __DEBUG__
#define PM_RAISE(retexn, exn) \
        do \
        { \
            retexn = (exn); \
            gVmGlobal.errFileId = __FILE_ID__; \
            gVmGlobal.errLineNum = (uint16_t)__LINE__; \
        } while (0)
#else
#define PM_RAISE(retexn, exn) \
        retexn = (exn)
#endif

/** return an error code if it is not PM_RET_OK */
#define PM_RETURN_IF_ERROR1(retval)  if((retval) != PM_RET_OK) { \
										plat_reportError(retval); \
										while(1) {  } }

#define PM_RETURN_IF_ERROR(retval)  if((retval) != PM_RET_OK) \
                                        return (retval)

/** print an error message if argument is not PM_RET_OK */
#define PM_REPORT_IF_ERROR(retval)   if ((retval) != PM_RET_OK) \
                                        plat_reportError(retval)

#if __DEBUG__
/** If the boolean expression fails, return the ASSERT error code */
#define C_ASSERT(boolexpr) \
    do \
    { \
        if (!((boolexpr))) \
        { \
            gVmGlobal.errFileId = __FILE_ID__; \
            gVmGlobal.errLineNum = (uint16_t)__LINE__; \
            return PM_RET_ASSERT_FAIL; \
        }\
    } \
    while (0)

#else
/** Assert statements are removed from production code */
#define C_ASSERT(boolexpr)
#endif


/***************************************************************
 * Types
 **************************************************************/

/**
 * Return values for system functions
 * to report status, errors, exceptions, etc.
 * Normally, functions which use these values
 * should propagate the same return value
 * up the call tree to the interpreter.
 */
typedef enum //PmReturn_e
{
    /* general status return values */
    PM_RET_OK = 0,              /**< Everything is ok */
    PM_RET_NO = 0xFF,           /**< General "no result" */
    PM_RET_ERR = 0xFE,          /**< General failure */
    PM_RET_STUB = 0xFD,         /**< Return val for stub fxn */
    PM_RET_ASSERT_FAIL = 0xFC,  /**< Assertion failure */
    PM_RET_FRAME_SWITCH = 0xFB, /**< Frame pointer was modified */

    /* return vals that indicate an exception occured */
	PM_RET_EX_NUM_ARGS = 0xDF,  /**< Invalid number of arguments on local frame */
    PM_RET_EX = 0xE0,           /**< General exception */
    PM_RET_EX_EXIT = 0xE1,      /**< System exit */
    PM_RET_EX_IO = 0xE2,        /**< Input/output error */
    PM_RET_EX_ZDIV = 0xE3,      /**< Zero division error */
    PM_RET_EX_ASSRT = 0xE4,     /**< Assertion error */
    PM_RET_EX_ATTR = 0xE5,      /**< Attribute error */
    PM_RET_EX_IMPRT = 0xE6,     /**< Import error */
    PM_RET_EX_INDX = 0xE7,      /**< Index error */
    PM_RET_EX_KEY = 0xE8,       /**< Key error */
    PM_RET_EX_MEM = 0xE9,       /**< Memory error */
    PM_RET_EX_NAME = 0xEA,      /**< Name error */
    PM_RET_EX_SYNTAX = 0xEB,    /**< Syntax error */
    PM_RET_EX_SYS = 0xEC,       /**< System error */
    PM_RET_EX_TYPE = 0xED,      /**< Type error */
    PM_RET_EX_VAL = 0xEE,       /**< Value error */
    PM_RET_EX_STOP = 0xEF,      /**< Stop iteration */
    PM_RET_EX_WARN = 0xF0      /**< Warning */
} PmReturn_t;


/***************************************************************
 * Globals
 **************************************************************/

extern volatile uint32_t pm_timerMsTicks;

/***************************************************************
 * Includes (order is critical)
 **************************************************************/

#include "pmfeatures.h"
#include "sli.h"
#include "mem.h"
#include "obj.h"
#include "seq.h"
#include "heap.h"
#include "int.h"
#include "string.h"
#include "tuple.h"
#include "seglist.h"
#include "list.h"
#include "dict.h"
#include "codeobj.h"
#include "func.h"
#include "module.h"
#include "frame.h"
#include "interp.h"
#include "img.h"
#include "global.h"
#include "misc.h"
#include "thread.h"
#include "plat/plat.h"


/***************************************************************
 * Prototypes
 **************************************************************/

/**
 * Initializes the PyMite virtual machine and indexes the user's application
 * image.  The VM heap and globals are reset.  The argument, pusrimg, may be
 * null for interactive sessions.
 *
 * @param memspace      Memory space in which the user image is located
 * @param pusrimg       Address of the user image in the memory space
 * @return Return status
 */
PmReturn_t pm_init(PmMemSpace_t memspace, uint8_t *pusrimg);

/**
 * Executes the named module
 *
 * @param modstr        Name of module to run
 * @return Return status
 */
PmReturn_t pm_run(uint8_t const *modstr);

/**
 * Needs to be called periodically by the host program.
 * For the desktop target, it is periodically called using a signal.
 * For embedded targets, it needs to be called periodically. It should
 * be called from a timer interrupt.
 *
 * @param usecsSinceLastCall Microseconds (not less than those) that passed
 *                           since last call. This must be <64535.
 * @return Return status
 */
PmReturn_t pm_vmPeriodic(uint16_t usecsSinceLastCall);

#endif /* __PM_H__ */
