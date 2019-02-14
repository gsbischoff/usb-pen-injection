@echo off

set CompileLibs=SetupAPI.lib User32.lib ws2_32.lib
set CompileFlags=-nologo

set FileToBuild=%1

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj