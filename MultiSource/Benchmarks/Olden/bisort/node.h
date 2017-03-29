/* For copyright information, see olden_v1.0/COPYRIGHT */

/* =============== NODE STRUCTURE =================== */

struct node { 
  int value;
  _Ptr<struct node> left;
  _Ptr<struct node> right;
};

typedef struct node HANDLE;

typedef struct future_cell_int{
  _Ptr<HANDLE> value;
} future_cell_int;

extern void *malloc(unsigned);

#define NIL ((_Ptr<HANDLE>) 0)
