@echo off
title FGAM - Make a coloured flare particle
setlocal

if "%~4"=="" goto :usage

powershell -NoProfile -ExecutionPolicy Bypass -File "%~dp0make_flare.ps1" -Name %1 -R %2 -G %3 -B %4
echo.
pause
exit /b 0

:usage
echo.
echo Usage:  make_flare.bat ^<name^> ^<R^> ^<G^> ^<B^>
echo.
echo   name   lowercase colour name, e.g. purple
echo   R G B  colour channels 0..1 (use dots), e.g.  0.6 0.1 1
echo.
echo Examples:
echo   make_flare.bat purple 0.6 0.1 1
echo   make_flare.bat cyan   0.1 0.9 1
echo   make_flare.bat pink   1   0.4 0.7
echo.
echo Writes FlareGunAirdropMod\Graphics\Particles\fgam_flare_^<name^>.ptc
echo Then follow ADDING_FLARES.md to register and use it.
echo.
pause
exit /b 1
