REM Build script for testbed
@ECHO OFF
SetLocal EnableDelayedExpansion

REM Get a list of all the .c files.
SET cFilenames=
FOR /R %%f in (*.c) do (
    SET cFilenames=!cFilenames! %%f
)

REM echo "Files:" %cFilenames%

SET assembly=skyBoxGame
SET compilerFlags=-g 
REM -Wall -Werror
SET includeFlags=-Isrc -I../engine/src/ -I../engine/vendor/glad/include
SET linkerFlags=-L../bin/ -lengine
SET defines=-D_DEBUG -DAGINA_IMPORT

ECHO "Building %assembly%%..."
clang %cFilenames% %compilerFlags% -o ../bin/%assembly%.exe %defines% %includeFlags% %linkerFlags%