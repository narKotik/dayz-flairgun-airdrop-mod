@echo off
title FGAM — Package clean folder for Workshop upload
setlocal

:: -- Paths --------------------------------------------------------------------
set BUILD=D:\Projects\DayZMods\@FlareGunAirdropMod
set PUBLISH=D:\Projects\DayZMods\@FlareGunAirdropMod_Publish
set PROJECT=D:\Projects\dayz-flairgun-airdrop-mod

if not exist "%BUILD%\addons\FlareGunAirdropMod.pbo" (
    echo ERROR: No build found at %BUILD%\addons\FlareGunAirdropMod.pbo
    echo Run 1_copy_to_P.bat first, then re-run this script.
    goto :end
)

echo === 1/4 Wiping previous publish folder ===
if exist "%PUBLISH%" rmdir /s /q "%PUBLISH%"
mkdir "%PUBLISH%"

echo === 2/4 Copying PBO + signature ===
robocopy "%BUILD%\addons" "%PUBLISH%\addons" *.pbo *.bisign /njh /njs

echo === 3/4 Copying public key only (never the private key) ===
mkdir "%PUBLISH%\keys"
copy /y "%BUILD%\keys\*.bikey" "%PUBLISH%\keys\" >nul

echo === 4/4 Copying mod.cpp + Configs (server setup docs/files) ===
copy /y "%BUILD%\mod.cpp" "%PUBLISH%\mod.cpp" >nul
robocopy "%PROJECT%\Configs" "%PUBLISH%\Configs" /e /njh /njs /ndl /nc /ns >nul

echo.
echo === Safety check: scanning for anything that shouldn't ship ===
set FOUND_BAD=0
for /r "%PUBLISH%" %%F in (*.biprivatekey) do (
    echo   FOUND PRIVATE KEY: %%F
    set FOUND_BAD=1
)
for /r "%PUBLISH%" %%F in (*.cfg) do (
    echo   FOUND CFG FILE: %%F
    set FOUND_BAD=1
)
if "%FOUND_BAD%"=="1" (
    echo.
    echo *** STOP — %PUBLISH% contains files that must never be published. ***
    echo *** Remove them and re-run before uploading. ***
) else (
    echo   Clean — no private key or .cfg files found.
    echo.
    echo === Done ===
    echo Point the DayZ Tools Publisher at:
    echo   %PUBLISH%
    echo (NOT %BUILD% — that folder still holds your private key for local signing.)
)

:end
echo.
pause
endlocal
