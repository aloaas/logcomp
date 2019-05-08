@echo off
if "%1"=="" goto viga
shift
attrib -r *.*


log32 %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
satikas %0
goto lopp

:viga
echo transcomp: no file!

:lopp
