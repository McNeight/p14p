/**
 * Regression Test for Issue #9
 *
 * Fix BINARY_SUBSCR for case stringobj[intobj]
 *
 * Log
 * ---
 *
 * 2006/09/01   #11: Make src/tests/ build module images as C files, 
 *              not header files 
 */

#if defined(__AVR__)
#include <avr/pgmspace.h>
#endif

#include "py.h"

extern unsigned char lib_img[];


int main(void)
{
    /* ptr to code imgs */
    P_U8 pimg = (P_U8)&lib_img;
    pPyObj_t pstring = C_NULL;
    /* ptr to module obj */
    pPyFunc_t pmod;
    /* name of module to run */
    P_U8 modstr = (P_U8)"t009";
    PyReturn_t retval = PY_RET_OK;

    heap_init();
    global_init();

    /* get image info into global struct */
    retval = img_findInMem(MEMSPACE_FLASH, &pimg);
    PY_RETURN_IF_ERROR(retval);

    /* import module from global struct */
    retval = string_new(&modstr, &pstring);
    PY_RETURN_IF_ERROR(retval);
    retval = mod_import(pstring, &pmod);
    PY_RETURN_IF_ERROR(retval);

    /* load builtins into root module */
    retval = global_loadBuiltins(pmod);
    PY_RETURN_IF_ERROR(retval);

    /* XXX set "__name__" == "__main__" in mod's attrs here? */

    /* interpret the module's bcode */
    retval = interpret(pmod);

    return retval;
}
