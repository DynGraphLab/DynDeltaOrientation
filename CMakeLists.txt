cmake_minimum_required(VERSION 3.16)

project(delta-orientations C CXX)
enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# debugging
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address -fno-omit-frame-pointer")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include_directories(${INTERFACE_INCLUDE_DIRECTORIES})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/extern/argtable3-3.0.3)


set(GUROBI_LIBRARY /home/cschulz/local/opt/gurobi912/lib)
find_package(Gurobi REQUIRED)

MESSAGE("Gurobi: include header files: ${GUROBI_INCLUDE_DIRS}")
include_directories(${GUROBI_INCLUDE_DIRS})

SET(DELTA-ORIENTATIONS_INCLUDE_DIRS
        lib
        lib/algorithms
        lib/data_structure
        lib/io
        lib/tools
        extern/sparsehash/src
        extern/argtable3-3.0.3/
        )
include_directories(${DELTA-ORIENTATIONS_INCLUDE_DIRS})



# VARS
SET(DELTA-ORIENTATIONS delta-orientations)
#SET(CMAKE_BUILD_TYPE Debug)
MESSAGE("${CMAKE_BUILD_TYPE}")

#add_definitions(-g -pg)


SET(DELTA-ORIENTATIONS_CXX_FILES
        lib/data_structure/dyn_graph_access.cpp
        lib/io/graph_io.cpp
        lib/tools/timer.cpp
        lib/tools/random_functions.cpp
        lib/algorithms/dyn_edge_orientation_ILP.cpp
        lib/algorithms/dyn_edge_orientation_BFSCS.cpp
        lib/algorithms/dyn_edge_orientation_BrodalFagerbergCS.cpp
        lib/algorithms/dyn_edge_orientation_Naive.cpp
        lib/algorithms/dyn_edge_orientation_KFlipsCS.cpp
        lib/algorithms/dyn_edge_orientation_RWalkCS.cpp
        lib/algorithms/dyn_edge_orientation_MaxDecending.cpp
        lib/algorithms/ilp.cpp
        extern/argtable3-3.0.3/argtable3.c
        )

set(LIBCONVERT_SOURCE_FILES
./lib/io/graph_io.cpp
./lib/data_structure/dyn_graph_access.cpp
./lib/tools/random_functions.cpp
  extern/argtable3-3.0.3/argtable3.c)
add_library(libconvert OBJECT ${LIBCONVERT_SOURCE_FILES})

# Obejct Library
set(DELTA-ORIENTATIONS_LIB delta-orientations_lib)
add_library(${DELTA-ORIENTATIONS_LIB} OBJECT ${DELTA-ORIENTATIONS_CXX_FILES})


# build settings
# debugging is activated if NDEBUG is not set
# logging
if (NOT DEFINED DELTA_ORIENTATIONS_IS_LOGGING)
    set(DELTA_ORIENTATIONS_IS_LOGGING false)
endif ()
add_compile_definitions(DELTAORI_IS_LOGGING=${DELTA_ORIENTATIONS_IS_LOGGING})

set(GUROBI_CXX_LIBRARY "$ENV{GUROBI_HOME}/lib/libgurobi_g++5.2.a")

# Executables
add_executable(${DELTA-ORIENTATIONS} app/${DELTA-ORIENTATIONS}.cpp $<TARGET_OBJECTS:${DELTA-ORIENTATIONS_LIB}>)
target_link_libraries(${DELTA-ORIENTATIONS} optimized ${GUROBI_CXX_LIBRARY})
target_link_libraries(${DELTA-ORIENTATIONS} gurobi_g++5.2 gurobi91)
set(CMAKE_EXE_LINKER_FLAGS "-L$ENV{GUROBI_HOME}/lib")


set (CMAKE_SHARED_LINKER_FLAGS "-lgurobi_c++ -lgurobi95, --as-needed")

add_executable(convert_metis_seq app/metis_to_sequence.cpp $<TARGET_OBJECTS:libconvert> )
install(TARGETS convert_metis_seq DESTINATION ./)

# install
set(CMAKE_INSTALL_PREFIX ../deploy)
install(TARGETS ${DELTA-ORIENTATIONS} DESTINATION ./)
