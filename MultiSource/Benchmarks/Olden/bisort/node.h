/* For copyright information, see olden_v1.0/COPYRIGHT */

/* =============== NODE STRUCTURE =================== */

#include <stdchecked.h>

#pragma BOUNDS_CHECKED ON

struct node { 
  int value;
  ptr<struct node> left;
  ptr<struct node> right;
};

typedef struct node HANDLE;

typedef struct future_cell_int{
  ptr<HANDLE> value;
} future_cell_int;

#define NIL ((ptr<HANDLE>) 0)

#pragma BOUNDS_CHECKED OFF
