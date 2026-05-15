#!/bin/bash
# Build script for engine
set echo on

mkdir -p ../bin

# Get a list of all the .c files.
cFilenames=$(find . -type f -name "*.c")

# echo "Files:" $cFilenames

assembly="engine"
compilerFlags="-g -shared -fdeclspec -fPIC"
# -fms-extensions 
# -Wall -Werror
if [ -z "$ASSIMP_INCLUDE" ]; then
    ASSIMP_INCLUDE="-Ivendor/assimp/include"
fi
if [ -z "$ASSIMP_LIB" ]; then
    ASSIMP_LIB="-Lvendor/assimp/lib -lassimp"
fi
includeFlags="-Isrc -Ivendor/glad/include $ASSIMP_INCLUDE"
linkerFlags="-lxcb -lX11 -lX11-xcb -lxkbcommon -lxkbcommon-x11 $ASSIMP_LIB"
defines="-D_DEBUG -DAGINA_EXPORT"

echo "Building $assembly..."
clang $cFilenames $compilerFlags -o ../bin/lib$assembly.so $defines $includeFlags $linkerFlags