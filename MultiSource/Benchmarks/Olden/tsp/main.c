/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "tsp.h"

#pragma BOUNDS_CHECKED ON
#define conquer_thresold 150  /* tsp() will use conquer for problems <= conquer_thresold */
extern int flag;

unchecked void print_tree(Tree t)
{
  Tree left = NULL, right = NULL;

  double x,y;

  if (!t) return;
  x = t->x; y = t->y;
  chatting("x=%f,y=%f\n",x,y);
  left = t->left; right=t->right;
  print_tree(left);
  print_tree(right);
}

unchecked void print_list(Tree t)
{
  Tree tmp = NULL;
  double x,y;

  if (!t) return;
  x = t->x; y = t->y;
  chatting("%f %f\n",x,y);
  for (tmp=t->next; tmp!=t; tmp=tmp->next) 
    {
    x = tmp->x; y = tmp->y;
    chatting("%f %f\n",x,y);
    }
}

unchecked int main(int argc,array_ptr<char*> argv : count(argc))
{
  Tree t = NULL;
  int num;
 
  num=dealwithargs(argc,argv);

  chatting("Building tree of size %d\n",num);
  checked {t=build_tree(num,0,0,NumNodes,0.0,1.0,0.0,1.0);}
  if (!flag) chatting("Past build\n");
  if (flag) chatting("newgraph\n");
  if (flag) chatting("newcurve pts\n");

  printf("Call tsp(t, %d, %d)\n", conquer_thresold, NumNodes); 
  checked {tsp(t,conquer_thresold, NumNodes);}

  if (flag) print_list(t);
  if (flag) chatting("linetype solid\n");

  return 0;
}
