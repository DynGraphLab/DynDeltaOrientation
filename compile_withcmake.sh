#!/bin/bash

cd "${0%/*}" || exit  # Run from current directory (source directory) or exit

if [[ -z "$NCORES" ]]; then 
    case "$(uname)" in
        Darwin)
            NCORES=$(sysctl -n hw.ncpu)
            ;;
        *)
            NCORES=$(getconf _NPROCESSORS_ONLN 2>/dev/null)
            ;;
    esac
    [ -n "$NCORES" ] || NCORES=4
fi

# build
rm -rf deploy
rm -rf build
mkdir build


ADDITIONAL_ARGS="$1"
echo $ADDITIONAL_ARGS
(cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release $ADDITIONAL_ARGS && \
    make -j $NCORES)

echo
echo "Copying files into 'deploy'"


mkdir deploy
cp build/delta-orientations deploy
cp build/convert_metis_seq deploy


echo "Build completed."
