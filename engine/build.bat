REM Build script for engine
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=engine
SET compilerFlags=-g -shared -Wvarargs -Wall -Werror
REM -Wall -Werror
IF NOT DEFINED ASSIMP_INCLUDE (
    SET ASSIMP_INCLUDE=-I"vendor\assimp\include"
)
IF NOT DEFINED ASSIMP_LIB (
    SET ASSIMP_LIB=-L"vendor\assimp\lib" -lassimp-vc143-mt
)
SET includeFlags=-Isrc -Ivendor/glad/include -Ivendor/stb_image %ASSIMP_INCLUDE%
SET linkerFlags=-luser32 -lgdi32 -lopengl32 -lkernel32 %ASSIMP_LIB%
SET defines=-D_DEBUG -DAGINA_EXPORT -D_CRT_SECURE_NO_WARNINGS

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.dll %defines% %includeFlags% %linkerFlags%