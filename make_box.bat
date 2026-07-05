@echo off
title FGAM - Make an ammo-box label texture
setlocal

if "%~4"=="" goto :usage

python "%~dp0make_box.py" %1 %2 %3 %4
echo.
pause
exit /b 0

:usage
echo.
echo Usage:  make_box.bat ^<name^> ^<R^> ^<G^> ^<B^>
echo.
echo   name   lowercase colour name, e.g. purple (must match your FGAM_Box_^<Name^> class)
echo   R G B  colour channels 0..255, e.g. 155 30 220
echo.
echo Examples:
echo   make_box.bat purple 155 30 220
echo   make_box.bat cyan   20  220 230
echo   make_box.bat pink   235 90  150
echo.
echo Writes FlareGunAirdropMod\data\fgam_box_^<name^>_co.paa
echo (needs Python + Pillow: pip install pillow; needs DayZ Tools installed for ImageToPAA.exe)
echo.
pause
exit /b 1
