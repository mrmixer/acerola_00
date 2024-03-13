@echo off
set directory=%~dp0
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
call "C:\Program Files (x86)\Microsoft GDK\Command Prompts\GamingDesktopVars.cmd" GamingDesktopVS2022
cd /D %directory%
rc -nologo cbuild.rc
cl main.c cbuild.res -Feredundancy.exe -nologo -GR- -EHa- -Oi -WX -W4 -wd4100 -wd4189 -wd4201 -wd4204 -wd4505 -wd4996 -FC -Fm -Zi -Zo -diagnostics:caret -diagnostics:column -Od -MTd -RTCsu -DCOMPILER_CL=1 -DDEBUG=1 -D_ASSERT=1 -DWINDOWS=1 -DTARGET_x64=1 -DLOG=1 -link -INCREMENTAL:NO -opt:ref,icf -subsystem:windows -entry:mainCRTStartup user32.lib GDI32.lib Ole32.lib Shell32.lib avrt.lib d3d11.lib dxgi.lib d3dcompiler.lib 