/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <stdchecked.h>
#pragma CHECKED_SCOPE ON

extern int atoi(const char * : itype(nt_array_ptr<const char>));

int NumNodes = 1;

int dealwithargs(int argc, array_ptr<nt_array_ptr<char>> argv : count(argc)) {
  int level;

  if (argc > 1)
    level = atoi(argv[1]);
  else
    level = 64;

  return level;
}
