@echo off

set CompileLibs=DieWithError.c User32.lib ws2_32.lib
set CompileFlags=-nologo

set FileToBuild=%1
rem IF NOT EXIST %1 set FileToBuild=Project1Skeleton.c

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj