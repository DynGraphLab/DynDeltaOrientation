# Adapted from https://support.gurobi.com/hc/en-us/articles/360039499751-How-do-I-use-CMake-to-build-Gurobi-C-C-projects-

# Defines GUROBI_INCLUDE_DIR, GUROBI_CXX_LIBRARY, GUROBI_LIBRARY

MESSAGE("Gurobi_DIR: ${Gurobi_DIR}")

find_path(
        GUROBI_INCLUDE_DIRS
        NAMES gurobi_c.h gurobi_c++.h
        HINTS ${Gurobi_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES include)

find_library(
        GUROBI_LIBRARY
        NAMES gurobi gurobi81 gurobi90 gurobi95
        HINTS ${Gurobi_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)

find_library(
        GUROBI_CXX_LIBRARY
        NAMES gurobi_c++
        HINTS ${Gurobi_DIR} $ENV{GUROBI_HOME}
        PATH_SUFFIXES lib)

set(GUROBI_CXX_DEBUG_LIBRARY ${GUROBI_CXX_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Gurobi DEFAULT_MSG GUROBI_LIBRARY GUROBI_CXX_LIBRARY
        GUROBI_INCLUDE_DIRS)
