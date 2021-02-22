add_subdirectory(XSBench)
add_subdirectory(Pathfinder)
# The miniAMR test fails after the Clang 11 upgrade.
# See https://github.com/microsoft/checkedc-llvm-test-suite/issues/102
#add_subdirectory(miniAMR)
add_subdirectory(miniGMG)
add_subdirectory(RSBench)
add_subdirectory(SimpleMOC)
