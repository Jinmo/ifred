@echo off

setlocal
echo Setting vs variables
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

echo Setting path
set PATH=%PATH%;C:\Qt\5.6.0-x64\bin;C:\Qt\Tools\QtCreator\bin

echo Qmake
qmake ..
echo Jom
jom -f Makefile.Release

endlocal