/*
 * item.c
 *
 * The author of this software is Alain K\"{a}gi.
 *
 * Copyright (c) 1993 by Alain K\"{a}gi and the University of Wisconsin
 * Board of Trustees.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose without fee is hereby granted, provided that this entire notice
 * is included in all copies of any software which is or includes a copy
 * or modification of this software and in all copies of the supporting
 * documentation for such software.
 *
 * THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY.  IN PARTICULAR, NEITHER THE AUTHOR NOR THE UNIVERSITY OF
 * WISCONSIN MAKE ANY REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING
 * THE MERCHANTABILITY OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR
 * PURPOSE.
 *
 * ------------------------------------------------------------------------
 *
 * Interface between graph and Fheap.
 *
 * ------------------------------------------------------------------------
 *
 * $Id$
 *
 */

#include <stdio_checked.h>
#include <stdlib_checked.h>
#include <assert.h>

#include "item.h"

#pragma CHECKED_SCOPE ON

int LessThan(_Ptr<Item>  item1, _Ptr<Item>  item2)
{
  return(KEY(item1) < KEY(item2));
}

int Equal(_Ptr<Item>  item1, _Ptr<Item>  item2)
{
  return(KEY(item1) == KEY(item2));
}

_Ptr<Item>  Subtract(_Ptr<Item>  item, int delta)
{
    assert(delta > 0);

    KEY(item) = KEY(item) - delta;

    return(item);
}

