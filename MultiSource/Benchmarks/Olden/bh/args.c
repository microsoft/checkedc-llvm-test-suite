/* For copyright information, see olden_v1.0/COPYRIGHT */
#include <stdlib.h>

#pragma CHECKED_SCOPE ON

extern int NumNodes;
extern int nbody;

int dealwithargs(int argc, _Array_ptr<_Nt_array_ptr<char>> argv : count(argc)) {
  int level;

  if (argc > 2) 
    NumNodes = atoi(argv[2]);
  else 
    NumNodes = 4;

  if (argc > 1)
    nbody = atoi(argv[1]);
  else
    nbody = 32;

  return level;
}





