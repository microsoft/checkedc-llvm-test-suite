/* For copyright information, see olden_v1.0/COPYRIGHT */

/* ========== PROCEDURE TYPES/NUMS ================== */


_Ptr<HANDLE> RandTree(int n, int seed, int node, int level);

void SwapValue(_Ptr<HANDLE>, _Ptr<HANDLE>);
void SwapValLeft(_Ptr<HANDLE>, _Ptr<HANDLE>, _Ptr<HANDLE>, _Ptr<HANDLE>, int, int);
void SwapValRight(_Ptr<HANDLE>, _Ptr<HANDLE>, _Ptr<HANDLE>, _Ptr<HANDLE>, int, int);
int Bimerge(_Ptr<HANDLE>, int, int);
int Bisort(_Ptr<HANDLE>, int, int);
int dealwithargs(int argc, _Array_ptr<char*> argv : count(argc));
