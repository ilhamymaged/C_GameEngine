REM Build And Run Everything

@ECHO OFF
SetLocal EnableDelayedExpansion

CALL build-all.bat
PUSHD bin
start "" cmd /c skyBoxGame.exe
POPD

ECHO Build and run completed successfully.