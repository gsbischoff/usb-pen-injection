@echo off

set CompileLibs=Bthprops.lib User32.lib ws2_32.lib
set CompileFlags=-nologo

set FileToBuild=%1

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj