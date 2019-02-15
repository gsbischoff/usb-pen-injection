@echo off

set CompileLibs=NetworkUtils.c Point.c Serial.c ws2_32.lib User32.lib Gdi32.lib Shcore.lib
set CompileFlags=-nologo -O2

set FileToBuild=%1
IF NOT EXIST %1 set FileToBuild=Project1Skeleton.c

cl %FileToBuild% %CompileLibs% %CompileFlags%
del *.obj