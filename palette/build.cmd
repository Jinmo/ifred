@echo off

setlocal
echo Setting vs variables
call "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

echo Setting path
set PATH=%PATH%;C:\Qt\5.6.0-x64\bin;C:\Qt\Tools\QtCreator\bin;C:\Program Files\JetBrains\CLion 2018.3.4\bin\cmake\win\bin

python ida-cmake\build.py -i %CD%\..\idasdk70 -t 7.0 --skip-install

endlocal