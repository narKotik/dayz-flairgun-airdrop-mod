@echo off
title FGAM — Switch to local dev build
setlocal

set BUILD=D:\Projects\DayZMods\@FlareGunAirdropMod
set SERVER=D:\Installs\steam\steamapps\common\DayZServer
set CLIENT=D:\Installs\steam\steamapps\common\DayZ

echo This points the server + client @FlareGunAirdropMod folder back at your local
echo build (%BUILD%), so you can test uncommitted/unpublished changes before
echo pushing a new Workshop update. Run 1_copy_to_P.bat first if you haven't
echo rebuilt since your last edit.
echo.

if not exist "%BUILD%\addons\FlareGunAirdropMod.pbo" (
    echo ERROR: No build found at %BUILD%\addons\FlareGunAirdropMod.pbo
    echo Run 1_copy_to_P.bat first, then re-run this script.
    goto :end
)

echo Deploying local build to server + client...
echo.
call "%~dp02_deploy.bat"
goto :eof

:end
echo.
pause
endlocal
