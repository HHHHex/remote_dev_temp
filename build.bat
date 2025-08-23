@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
msbuild ThousChannel.sln /p:Configuration=Debug /p:Platform=x64
pause
