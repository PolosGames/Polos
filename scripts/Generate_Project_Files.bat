@echo off
cd /D "%~dp0"
call premake5.exe %1 --gfxapi=%2 --file=../premake5.lua
PAUSE