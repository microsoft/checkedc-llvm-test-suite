/* For copyright information, see olden_v1.0/COPYRIGHT */

/* =================== PROGRAM bitonic===================== */
/* UP - 0, DOWN - 1 */
#include "node.h"   /* Node Definition */
#include "proc.h"   /* Procedure Types/Nums */
#include <stdio.h>

#pragma CHECKED_SCOPE ON

#define printf(...) _Unchecked { printf(__VA_ARGS__); }

#define CONST_m1 10000
#define CONST_b 31415821
#define RANGE 100

int NumNodes, NDim;

int random(int);
void *calloc(size_t nmemb, size_t size) : byte_count(nmemb * size);

int flag=0,foo=0;

#define LocalNewNode(h,v) \
{ \
    h = (ptr<HANDLE>) calloc(1,sizeof(struct node)); \
      h->value = v; \
	h->left = NIL; \
	  h->right = NIL; \
	  };

#define NewNode(h,v,procid) LocalNewNode(h,v)

void InOrder(ptr<HANDLE> h) {
  ptr<HANDLE> l = NIL, r = NIL;
  if ((h != NIL)) {
    l = h->left;
    r = h->right;
    InOrder(l);
    static unsigned char counter = 0;
    if (counter++ == 0)   /* reduce IO */
      printf("%d @ 0x%x\n",h->value, 0);
    InOrder(r);
  }
}

int mult(int p, int q) {
  int p1, p0, q1, q0;
	
  p1 = p/CONST_m1; p0 = p%CONST_m1;
  q1 = q/CONST_m1; q0 = q%CONST_m1;
  return ((p0*q1+p1*q0) % CONST_m1)*CONST_m1+p0*q0;
}

/* Generate the nth random # */
int skiprand(int seed, int n) {
  for (; n; n--) seed=random(seed);
  return seed;
}

int random(int seed) {
  return mult(seed,CONST_b)+1;
}

ptr<HANDLE> RandTree(int n, int seed, int node, int level) {
  int next_val,my_name;
  future_cell_int f_left = { 0 }, f_right = {0};
  ptr<HANDLE> h = NIL;
  my_name=foo++;
  if (n > 1) {
    int newnode;
    if (level<NDim)
      newnode = node + (1 <<  (NDim-level-1));
    else
      newnode = node;
    seed = random(seed);
    next_val=seed % RANGE;
    NewNode(h,next_val,node);
    f_left.value = RandTree((n/2),seed,newnode,level+1);
    f_right.value = RandTree((n/2),skiprand(seed,(n)+1),node,level+1);

    h->left = f_left.value;
    h->right = f_right.value;
  } else {
    h = 0;
  }
  return h;
}

// CHECKED-C: The only two conversions that checked-c-convert could
// perform automatically were these two.
void SwapValue(ptr<HANDLE> l, ptr<HANDLE> r) {
  int temp,temp2;
  
  temp = l->value;
  temp2 = r->value;
  r->value = temp;
  l->value = temp2;
} 

void
/***********/
SwapValLeft(ptr<HANDLE> l, ptr<HANDLE> r, ptr<HANDLE> ll, ptr<HANDLE> rl, int lval, int rval)
/***********/
{
  r->value = lval;
  r->left = ll;
  l->left = rl;
  l->value = rval;
} 


void
/************/
SwapValRight(ptr<HANDLE> l, ptr<HANDLE> r, ptr<HANDLE> lr, ptr<HANDLE> rr, int lval, int rval)
/************/
{  
  r->value = lval;
  r->right = lr;
  l->right = rr;
  l->value = rval;
  /*printf("Swap Val Right l 0x%x,r 0x%x val: %d %d\n",l,r,lval,rval);*/
} 

int
/********************/
Bimerge(ptr<HANDLE> root, int spr_val, int dir)
/********************/

{ int rightexchange;
  int elementexchange;
  ptr<HANDLE> pl = NIL, pll = NIL, plr = NIL;
  ptr<HANDLE> pr = NIL, prl = NIL, prr = NIL;
  ptr<HANDLE> rl = NIL;
  ptr<HANDLE> rr = NIL;
  int rv,lv;


  /*printf("enter bimerge %x\n", root);*/
  rv = root->value;

  pl = root->left;
  pr = root->right;
  rightexchange = ((rv > spr_val) ^ dir);
  if (rightexchange)
    {
      root->value = spr_val;
      spr_val = rv;
    }
  
  while ((pl != NIL))
    {
      /*printf("pl = 0x%x,pr = 0x%x\n",pl,pr);*/
      lv = pl->value;        /* <------- 8.2% load penalty */
      pll = pl->left;
      plr = pl->right;       /* <------- 1.35% load penalty */
      rv = pr->value;         /* <------ 57% load penalty */
      prl = pr->left;         /* <------ 7.6% load penalty */
      prr = pr->right;        /* <------ 7.7% load penalty */
      elementexchange = ((lv > rv) ^ dir);
      if (rightexchange)
        if (elementexchange)
          { 
            SwapValRight(pl,pr,plr,prr,lv,rv);
            pl = pll;
            pr = prl;
          }
        else 
          { pl = plr;
            pr = prr;
          }
      else 
        if (elementexchange)
          { 
            SwapValLeft(pl,pr,pll,prl,lv,rv);
            pl = plr;
            pr = prr;
          }
        else 
          { pl = pll;
            pr = prl;
          }
    }
  if ((root->left != NIL))
    { 
      int value;
      rl = root->left;
      rr = root->right;
      value = root->value;

      root->value=Bimerge(rl,value,dir);
      spr_val=Bimerge(rr,spr_val,dir);
    }
  /*printf("exit bimerge %x\n", root);*/
  return spr_val;
} 

int
/*******************/
Bisort(ptr<HANDLE> root, int spr_val, int dir)
/*******************/

{ ptr<HANDLE> l = NIL;
  ptr<HANDLE> r = NIL;
  int val;
  /*printf("bisort %x\n", root);*/
  if ((root->left == NIL))  /* <---- 8.7% load penalty */
    { 
     if (((root->value > spr_val) ^ dir))
        {
	  val = spr_val;
	  spr_val = root->value;
	  root->value =val;
	}
    }
  else 
    {
      int ndir;
      l = root->left;
      r = root->right;
      val = root->value;
      /*printf("root 0x%x, l 0x%x, r 0x%x\n", root,l,r);*/
      root->value=Bisort(l,val,dir);
      ndir = !dir;
      spr_val=Bisort(r,spr_val,ndir);
      spr_val=Bimerge(root,spr_val,dir);
    }
  /*printf("exit bisort %x\n", root);*/
  return spr_val;
} 

int main(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc)) {
  ptr<HANDLE> h = NIL;
  int sval;
  int n;
   
  n = dealwithargs(argc,argv);

  printf("Bisort with %d size of dim %d\n", n, NDim);

  h = RandTree(n,12345768,0,0);
  sval = random(245867) % RANGE;
  if (flag) {
    InOrder(h);
    printf("%d\n",sval);
  }
  printf("**************************************\n");
  printf("BEGINNING BITONIC SORT ALGORITHM HERE\n");
  printf("**************************************\n");

  sval=Bisort(h,sval,0);

  if (flag) {
    printf("Sorted Tree:\n"); 
    InOrder(h);
    printf("%d\n",sval);
  }

  sval=Bisort(h,sval,1);

  if (flag) {
    printf("Sorted Tree:\n"); 
    InOrder(h);
    printf("%d\n",sval);
  }

  return 0;
} 







