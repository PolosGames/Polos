SET PRESET=win-debug
SET INSTALL_DIR=%~dp0out/build/%PRESET%

cmake --preset %PRESET% -DPOLOS_INSTALL_DIR=%INSTALL_DIR% -DBUILD_EDITOR=OFF

if errorlevel 1 exit /B

cmake --build --preset %PRESET%
