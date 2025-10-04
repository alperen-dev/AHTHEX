@ECHO OFF

rem USER DEFINES

rem 1 will use upx, 0 is not
SET USE_UPX=1

rem C compile path (i use turbo c++ 1.01)
SET CC=C:\TC\BIN\TCC.EXE

rem USER DEFINES END
rem ---------------------------------


if %USE_UPX%==1 SET UPX_PATH=C:\DOS\UPX.EXE
DEL AHTHEXD.EXE > NUL
%CC% -eAHTHEXD.EXE -rd -mh AHTHEX.C FILES.C DOSPROC.C OPTIONS.C
DEL *.OBJ > NUL
if %USE_UPX%==1 %UPX_PATH% -9 AHTHEXD.EXE

rem remove used environment variable
SET USE_UPX=
SET CC=
SET UPX_PATH=