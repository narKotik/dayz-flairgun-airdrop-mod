@echo off
title FGAM — Deploy to server and DayZ client

set PROJECT=D:\Projects\dayz-flairgun-airdrop-mod
set BUILD=D:\Projects\DayZMods\@FlareGunAirdropMod
set SERVER=D:\Installs\steam\steamapps\common\DayZServer
set CLIENT=D:\Installs\steam\steamapps\common\DayZ
set MISSION=%SERVER%\mpmissions\dayzOffline.chernarusplus

:: If @FlareGunAirdropMod currently points at the Steam Workshop copy (see
:: 3_switch_to_workshop.bat), it's a junction. robocopy /purge would traverse
:: it and delete/overwrite files inside the actual Workshop item folder, so
:: remove the junction first (removes only the link, not its target) before
:: writing a real local copy.
call :unlink_if_junction "%SERVER%\@FlareGunAirdropMod"
call :unlink_if_junction "%CLIENT%\@FlareGunAirdropMod"

echo === 1. Copy PBO + public key from build folder to server ===
robocopy "%BUILD%\addons" "%SERVER%\@FlareGunAirdropMod\addons" /e /purge /njh /njs
if not exist "%SERVER%\@FlareGunAirdropMod\keys" mkdir "%SERVER%\@FlareGunAirdropMod\keys"
:: Only the public .bikey ships with the mod folder — never the .biprivatekey.
copy /y "%BUILD%\keys\*.bikey" "%SERVER%\@FlareGunAirdropMod\keys\"
copy /y "%BUILD%\mod.cpp"  "%SERVER%\@FlareGunAirdropMod\mod.cpp"

:: Copy bikey to server keys folder
copy /y "%BUILD%\keys\*.bikey" "%SERVER%\keys\"

echo.
echo === 2. Copy mod to DayZ client ===
robocopy "%BUILD%\addons" "%CLIENT%\@FlareGunAirdropMod\addons" /e /purge /njh /njs
if not exist "%CLIENT%\@FlareGunAirdropMod\keys" mkdir "%CLIENT%\@FlareGunAirdropMod\keys"
copy /y "%BUILD%\keys\*.bikey" "%CLIENT%\@FlareGunAirdropMod\keys\"
copy /y "%BUILD%\mod.cpp"  "%CLIENT%\@FlareGunAirdropMod\mod.cpp"

echo.
echo === 3. Copy mission files (cfgeconomycore, FGAM_types, server config) ===
copy /y "%PROJECT%\Configs\chernarusplus\db\FGAM_types.xml" "%MISSION%\db\FGAM_types.xml"
copy /y "%PROJECT%\Configs\chernarusplus\db\FGAM_spawnabletypes.xml" "%MISSION%\db\FGAM_spawnabletypes.xml"
copy /y "%PROJECT%\Configs\chernarusplus\cfgeconomycore.xml" "%MISSION%\cfgeconomycore.xml" 2>nul

:: Server JSON config (goes to Profiles folder, created at runtime if missing)
if not exist "%SERVER%\Profiles\FlareGunAirdropMod" mkdir "%SERVER%\Profiles\FlareGunAirdropMod"
copy /y "%PROJECT%\Configs\FlareGunAirdropMod\config.json" "%SERVER%\Profiles\FlareGunAirdropMod\config.json"

echo.
echo === Done! Restart the server to apply changes. ===
echo.
pause
goto :eof

:unlink_if_junction
fsutil reparsepoint query "%~1" >nul 2>&1
if not errorlevel 1 (
    echo Removing Workshop-mode junction: %~1
    rmdir "%~1"
)
exit /b
