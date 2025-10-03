SET PRESET=win-debug
SET INSTALL_DIR=%~dp0out/build/%PRESET%

powershell -Command "Get-ChildItem -Recurse -Include *.h, *.hpp, *.cpp -File | Where-Object { $_.FullName -notmatch '\\(out|ThirdParty)\\' } | ForEach-Object { clang-format --style=file -i $_.FullName }"

cmake --preset %PRESET%^
 -DPOLOS_INSTALL_DIR=%INSTALL_DIR%

if %ERRORLEVEL% neq 0 (
  echo CMake configuration failed!
  exit /b %ERRORLEVEL%
)

cmake --build --preset %PRESET%

if %ERRORLEVEL% neq 0 (
  echo Build failed!
  exit /b %ERRORLEVEL%
)

powershell -Command "Start-Process -FilePath %INSTALL_DIR%/dummy.exe -WorkingDirectory %INSTALL_DIR%"