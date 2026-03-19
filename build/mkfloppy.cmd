@ECHO OFF
set PROJECT_NAME=AHTHEX
imdisk -a -o fd -f %PROJECT_NAME%.img -s 1440K -m A: -p "/FS:FAT /F:1.44 /Q /Y /V:%PROJECT_NAME%"

copy %PROJECT_NAME%D.COM A:\

REM cd %PROJECT_NAME%
REM xcopy *.* A:\ /E
REM cd ..

imdisk -D -m A: