/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#include <stddef_checked.h>

#pragma BOUNDS_CHECKED ON

#ifdef TORONTO
extern int NumNodes;
#define chatting      printf
#endif

#ifndef TORONTO
#include <cm/cmmd.h>
#ifdef FUTURES
#include "future-cell.h"
#endif
#include "mem-ref.h"
#endif

typedef enum {black, white, grey} Color;
typedef enum {northwest, northeast, southwest, southeast} ChildType;
typedef enum {north, east, south, west} Direction;


typedef struct quad_struct {
  Color color;
  ChildType childtype;

#ifndef TORONTO
  ptr<struct quad_struct> nw {50};
  ptr<struct quad_struct> ne {50};
  ptr<struct quad_struct> sw {50};
  ptr<struct quad_struct> se {50};
  ptr<struct quad_struct> parent {50};
#else
  ptr<struct quad_struct> nw;
  ptr<struct quad_struct> ne;
  ptr<struct quad_struct> sw;
  ptr<struct quad_struct> se;
  ptr<struct quad_struct> parent;
#endif

} quad_struct;

typedef ptr<struct quad_struct> QuadTree;


QuadTree MakeTree(int size, int center_x, int center_y, int lo_proc,
                  int hi_proc, QuadTree parent, ChildType ct, int level);








#pragma BOUNDS_CHECKED OFF
