@echo off
cd /D "%~dp0"
call scripts\Generate_Project_Files.bat vs2022 opengl
exit