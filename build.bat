@echo off

set CompileLibs=NetworkUtils.c Point.c Serial.c ws2_32.lib User32.lib
set CompileFlags=-nologo

set FileToBuild=%1
IF NOT EXIST %1 set FileToBuild=Project1Skeleton.c

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj