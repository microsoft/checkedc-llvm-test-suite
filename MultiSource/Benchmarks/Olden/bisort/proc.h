/* For copyright information, see olden_v1.0/COPYRIGHT */

/* ========== PROCEDURE TYPES/NUMS ================== */

#include <stdchecked.h>

#pragma BOUNDS_CHECKED ON

ptr<HANDLE> RandTree(int, int, int, int);

void SwapValue(ptr<HANDLE>, ptr<HANDLE>);
void SwapValLeft(ptr<HANDLE>, ptr<HANDLE>, ptr<HANDLE>, ptr<HANDLE>, int, int);
void SwapValRight(ptr<HANDLE>, ptr<HANDLE>, ptr<HANDLE>, ptr<HANDLE>, int, int);
int Bimerge(ptr<HANDLE>, int, int);
int Bisort(ptr<HANDLE>, int, int);

#pragma BOUNDS_CHECKED OFF

int dealwithargs(int argc, array_ptr<char*> argv : count(argc));

