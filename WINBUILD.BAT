@ECHO OFF

rem USER DEFINES

rem 1 will use upx, 0 is not
SET USE_UPX=1

rem C compile path (i use gcc (MinGW-W64 x86_64-msvcrt-posix-seh, built by Brecht Sanders, r3) 15.1.0)
SET CC=gcc

rem USER DEFINES END
rem ---------------------------------


if %USE_UPX%==1 SET UPX_PATH=upx.exe
DEL AHTHEXW.EXE > NUL
%CC% -x c -std=gnu89 -mconsole -s AHTHEX.C FILES.C WINPROC.C OPTIONS.C LNKLST.C -o AHTHEXW.EXE
DEL *.OBJ > NUL
if %USE_UPX%==1 %UPX_PATH% -9 AHTHEXW.EXE

rem remove used environment variable
SET USE_UPX=
SET CC=
SET UPX_PATH=

pause