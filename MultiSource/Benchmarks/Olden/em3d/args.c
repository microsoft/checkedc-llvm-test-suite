/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"
#include <stdchecked.h>

#ifndef TORONTO
#include <cm/cmmd.h>
#include <fcntl.h>
#endif

#pragma BOUNDS_CHECKED ON

#ifdef TORONTO
int NumNodes;
#else
#ifdef OLDEN
extern int __NumNodes;
#endif
#endif

extern int DebugFlag;

#ifndef TORONTO
unchecked void filestuff()
{
  CMMD_fset_io_mode(stdout, CMMD_independent);
  fcntl(fileno(stdout), F_SETFL, O_APPEND);
  if (CMMD_self_address()) exit(0);
  __InitRegs(0);
}
#endif

unchecked void dealwithargs(int argc, array_ptr<char*> argv : count(argc))
{
#ifdef TORONTO
  if (argc > 4)
    NumNodes = atoi(argv[4]);    
  else
    NumNodes = 1;
#else
#ifdef OLDEN
  if (argc > 5)
    DebugFlag = atoi(argv[5]);
  else DebugFlag = 0;

  if (argc > 4)
    __NumNodes = atoi(argv[4]);    
  else
    __NumNodes = 4;
#endif
#endif

  if (argc > 1)
    n_nodes = atoi(argv[1]);
  else
    n_nodes = 64;

  if (argc > 2)
    d_nodes = atoi(argv[2]);
  else
    d_nodes = 3;

  if (argc > 3)
    local_p = atoi(argv[3]);
  else
    local_p = 75;
}


