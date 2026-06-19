@echo off
title FGAM — Copy source to P:\

set PROJECT=D:\Projects\dayz-flairgun-airdrop-mod
set P_MOD=P:\FlareGunAirdropMod

echo === Copying source to P:\FlareGunAirdropMod ===

if not exist "%P_MOD%" mkdir "%P_MOD%"

:: config.cpp and mod.cpp go into P:\FlareGunAirdropMod root
copy /y "%PROJECT%\config.cpp" "%P_MOD%\config.cpp"
copy /y "%PROJECT%\mod.cpp"    "%P_MOD%\mod.cpp"

:: scripts
robocopy "%PROJECT%\FlareGunAirdropMod\scripts" "%P_MOD%\scripts" /e /purge /njh /njs

:: data (rvmat files)
robocopy "%PROJECT%\FlareGunAirdropMod\data" "%P_MOD%\data" /e /purge /njh /njs

echo.
echo === Done. Now open Addon Builder and pack P:\FlareGunAirdropMod ===
echo.
pause
