@echo off

set CompileLibs=User32.lib Hid.lib
set CompileFlags=-nologo

rem set FileToBuild=%1
rem IF NOT EXIST %1 set FileToBuild=Project1Skeleton.c

rem cl %FileToBuild% %CompileLibs% %CompileFlags%
rem del *.obj

forfiles /M *.c* /C "cmd /c cl @file %CompileLibs% %CompileFlags%"
del *.obj