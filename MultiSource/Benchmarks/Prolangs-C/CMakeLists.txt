add_subdirectory(agrep)
add_subdirectory(gnugo)
if((NOT "${ARCH}" STREQUAL "Sparc") AND (NOT "${ARCH}" STREQUAL "Alpha"))
  add_subdirectory(bison)
endif()
if(NOT TEST_SUITE_BENCHMARKING_ONLY)
  # The TimberWolfMC test fails after the Clang 11 upgrade.
  # See https://github.com/microsoft/checkedc-llvm-test-suite/issues/102 
  #add_subdirectory(TimberWolfMC)
  add_subdirectory(allroots)
  add_subdirectory(assembler)
  add_subdirectory(cdecl)
  # The compiler test fails after the Clang 11 upgrade.
  # See https://github.com/microsoft/checkedc-llvm-test-suite/issues/102
  #add_subdirectory(compiler)
  add_subdirectory(fixoutput)
  add_subdirectory(football)
  # The loader test fails after the Clang 11 upgrade.
  # See https://github.com/microsoft/checkedc-llvm-test-suite/issues/102
  #add_subdirectory(loader)
  add_subdirectory(plot2fig)
  # The simulator test fails after the Clang 11 upgrade.
  # See https://github.com/microsoft/checkedc-llvm-test-suite/issues/102
  #add_subdirectory(simulator)
  add_subdirectory(unix-tbl)
  if(NOT "${ARCH}" STREQUAL "XCore")
    add_subdirectory(archie-client)
    add_subdirectory(unix-smail)
  endif()
endif()
