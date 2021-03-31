list(APPEND CFLAGS -std=gnu89 -DNO_IMPLICIT_INCLUDE_CHECKED_HDRS)
set(RUN_OPTIONS < testset)
llvm_multisource(cdecl)
llvm_test_data(cdecl testset)
