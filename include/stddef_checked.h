//---------------------------------------------------------------------//
// Bounds-safe interfaces for functions in math.h that                 //
// take pointer arguments.                                             //
//                                                                     //
// These are listed in the same order that they occur in the C11       //
// specification.                                                      //
//                                                                     //
// TODO: revise string types after support for pointers to             //
// null-terminated arrays is added to C.                               //
//                                                                     //
// TODO: Better Support for _FORTIFY_SOURCE > 0                        //
/////////////////////////////////////////////////////////////////////////

#include <stddef.h>

// Mac OS X and Linux Headers overwrite NULL to expand to ((void*) 0)
// which doesn't work in a checked scope. 0 will work fine.
#define NULL 0
