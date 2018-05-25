@echo off

set CompileLibs=User32.lib
set CompileFlags=-nologo

set FileToBuild=%1
rem IF NOT EXIST %1 set FileToBuild=Project1Skeleton.c

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj