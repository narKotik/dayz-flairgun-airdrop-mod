@echo off
title FGAM — Deploy to server and DayZ client

set PROJECT=D:\Projects\dayz-flairgun-airdrop-mod
set BUILD=D:\Projects\DayZMods\@FlareGunAirdropMod
set SERVER=D:\Installs\steam\steamapps\common\DayZServer
set CLIENT=D:\Installs\steam\steamapps\common\DayZ
set MISSION=%SERVER%\mpmissions\dayzOffline.chernarusplus

echo === 1. Copy PBO + keys from build folder to server ===
robocopy "%BUILD%\addons" "%SERVER%\@FlareGunAirdropMod\addons" /e /purge /njh /njs
robocopy "%BUILD%\keys"   "%SERVER%\@FlareGunAirdropMod\keys"   /e /purge /njh /njs
copy /y "%BUILD%\mod.cpp"  "%SERVER%\@FlareGunAirdropMod\mod.cpp"

:: Copy bikey to server keys folder
copy /y "%BUILD%\keys\*.bikey" "%SERVER%\keys\"

echo.
echo === 2. Copy mod to DayZ client ===
robocopy "%BUILD%\addons" "%CLIENT%\@FlareGunAirdropMod\addons" /e /purge /njh /njs
robocopy "%BUILD%\keys"   "%CLIENT%\@FlareGunAirdropMod\keys"   /e /purge /njh /njs
copy /y "%BUILD%\mod.cpp"  "%CLIENT%\@FlareGunAirdropMod\mod.cpp"

echo.
echo === 3. Copy mission files (cfgeconomycore, FGAM_types, ServerProfile config) ===
copy /y "%PROJECT%\db\FGAM_types.xml" "%MISSION%\db\FGAM_types.xml"
copy /y "%PROJECT%\db\FGAM_spawnabletypes.xml" "%MISSION%\db\FGAM_spawnabletypes.xml"
copy /y "%PROJECT%\cfgeconomycore.xml" "%MISSION%\cfgeconomycore.xml" 2>nul

:: Server JSON config (goes to Profiles folder, created at runtime if missing)
if not exist "%SERVER%\Profiles\FlareGunAirdropMod" mkdir "%SERVER%\Profiles\FlareGunAirdropMod"
copy /y "%PROJECT%\ServerProfile\FlareGunAirdropMod\config.json" "%SERVER%\Profiles\FlareGunAirdropMod\config.json"

echo.
echo === Done! Restart the server to apply changes. ===
echo.
pause
