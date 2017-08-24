
/*
 *
 * option.h
 *
 */


/*
 *
 * Includes.
 *
 */

#ifndef OPTION_H
#define OPTION_H

#pragma BOUNDS_CHECKED ON

/*
 *
 * Defines.
 *
 */


/*
 *
 * Types.
 *
 */


/*
 *
 * Globals.
 *
 */

#ifdef OPTION_CODE

#else	/* OPTION_CODE */

#endif	/* OPTION_CODE */


/*
 *
 * Prototypes.
 *
 */

#ifdef OPTION_CODE

_Unchecked void
Option(int argc,
       _Array_ptr<char*> : count(argc));

#else	/* OPTION_CODE */

_Unchecked extern void
Option(int argc,
       _Array_ptr<char*> : count(argc));

#endif	/* OPTION_CODE */

#pragma BOUNDS_CHECKED OFF
#endif	/* OPTION_H */
