#!/bin/bash

# args
if [ "$#" -ne 2 ]; then
    echo "Using Release and logging false"
    BUILDTYPE="Release"
    LOGGING="false"
else
BUILDTYPE=$1
LOGGING=$2
fi
# build-type options: Release,Debug
declare dynwmis_buildtype=$BUILDTYPE
# logging: true/false
declare dynwmis_is_logging=$LOGGING

# determine CPUs
declare NCORES=4
declare unamestr=$(uname)
if [[ "$unamestr" == "Linux" ]]; then
    NCORES=$(grep -c ^processor /proc/cpuinfo)
fi

if [[ "$unamestr" == "Darwin" ]]; then
    NCORES=$(sysctl -n hw.ncpu)
fi

# build
rm -rf build
mkdir build
cd build
if [[ "$unamestr" == "Linux" ]]; then
    cmake ../ -DCMAKE_BUILD_TYPE=${dynwmis_buildtype} -DDELTA_ORIENTATIONS_IS_LOGGING=${dynwmis_is_logging} -DGurobi_DIR="${GUROBI_HOME}"
fi
if [[ "$unamestr" == "Darwin" ]]; then
    cmake ../ -DCMAKE_BUILD_TYPE=${dynwmis_buildtype} -DDELTA_ORIENTATIONS_IS_LOGGING=${dynwmis_is_logging} -DGurobi_DIR="${GUROBI_HOME}"
fi
make -j $NCORES

# test
ctest

# deploy
cmake --install .
cd ..

echo "Build completed."
