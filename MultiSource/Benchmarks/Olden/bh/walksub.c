#define global 
#include "defs.h"

#pragma CHECKED_SCOPE ON

typedef struct {
  bodyptr pskip;		/* body to skip in force evaluation */
  vector pos0;			/* point at which to evaluate field */
  real phi0;			/* computed potential at pos0       */
  vector acc0;			/* computed acceleration at pos0    */
} hgstruct;
typedef _Ptr<struct hgstruct> hgsptr;

/*
 * WALKSUB: recursive routine to do hackwalk operation.
 * p: pointer into body-tree 
 * dsq: size of box squared 
 */
extern bool subdivp(nodeptr p, real dsq, real tolsq, hgstruct hg);
hgstruct walksub(nodeptr p, real dsq, real tolsq, hgstruct hg, int level);
hgstruct gravsub(nodeptr p, hgstruct hg);

hgstruct walksub(nodeptr p, real dsq, real tolsq, hgstruct hg, int level)
{
  register int k, i;
  register nodeptr r = NULL;
  nodeptr tmp _Checked[NSUB] = {0};


  if (subdivp(p, dsq, tolsq, hg)) {           /* should p be opened?    */
    for (k = 0; k < NSUB; k++) {              /* loop over the subcells */
      _Unchecked { r = Subp(_Assume_bounds_cast<cellptr>(p))[k];} /* <-- 6.7% load penalty */
      if (r != NULL)                  /* does this one exist?   */
	hg = walksub(r, dsq / 4.0, tolsq, hg, level+1);
    }
  }
  else if (p != (nodeptr) hg.pskip)   {         /* should p be included?  */
    hg = gravsub(p, hg);                           /* then use interaction   */
  }


  return hg;
}




