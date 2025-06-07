SET PRESET=win-debug
SET INSTALL_DIR=%~dp0out/build/%PRESET%

powershell -Command "Get-ChildItem -Recurse -Include *.h, *.hpp, *.cpp -File | Where-Object { $_.FullName -notmatch '\\(out|ThirdParty)\\' } | ForEach-Object { clang-format --style=file -i $_.FullName }"

cmake --preset %PRESET% -DPOLOS_INSTALL_DIR=%INSTALL_DIR% -DBUILD_EDITOR=OFF

if errorlevel 1 exit /B

cmake --build --preset %PRESET%
