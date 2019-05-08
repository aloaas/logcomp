@echo off
if "%1"=="" goto viga
shift

attrib -r *.*
set f=%0.tex
if exist %0.ltx set f=%0.ltx
lahti %f%
set f=

log32 %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
satikas %0
goto lopp

:viga
echo Metacomp: no file!

:lopp
