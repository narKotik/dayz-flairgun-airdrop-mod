@echo off
title FGAM — Switch to Steam Workshop version
setlocal

set SERVER=D:\Installs\steam\steamapps\common\DayZServer
set CLIENT=D:\Installs\steam\steamapps\common\DayZ
set WORKSHOP_ITEM=D:\Installs\steam\steamapps\workshop\content\221100\3758787089

echo This points the server + client @FlareGunAirdropMod folder at the published
echo Steam Workshop copy (via a directory junction), so you test exactly what
echo subscribers get. Your local build and private key in
echo D:\Projects\DayZMods\@FlareGunAirdropMod are never touched by this script.
echo.

if not exist "%WORKSHOP_ITEM%" (
    echo ERROR: Workshop item not found at %WORKSHOP_ITEM%
    echo Make sure you're subscribed to the published mod in Steam first.
    goto :end
)

call :relink "%SERVER%\@FlareGunAirdropMod"
call :relink "%CLIENT%\@FlareGunAirdropMod"

echo.
echo Done. Server + client now use the Steam Workshop build. Run
echo 4_switch_to_local.bat to go back to testing local changes.
goto :end

:relink
if exist "%~1" (
    fsutil reparsepoint query "%~1" >nul 2>&1
    if errorlevel 1 (
        echo Removing local build copy: %~1
        rmdir /s /q "%~1"
    ) else (
        echo Removing existing link: %~1
        rmdir "%~1"
    )
)
echo Linking %~1 -^> %WORKSHOP_ITEM%
mklink /J "%~1" "%WORKSHOP_ITEM%"
exit /b

:end
echo.
pause
endlocal
