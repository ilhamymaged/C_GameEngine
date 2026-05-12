REM Build And Run Everything

@ECHO OFF
SetLocal EnableDelayedExpansion

CALL build-all.bat
PUSHD bin
start "" cmd /k skyBoxGame.exe
POPD

ECHO Build and run completed successfully.