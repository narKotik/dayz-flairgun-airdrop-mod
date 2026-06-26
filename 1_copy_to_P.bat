@echo off
title FGAM - Build PBO (sync + pack + sign)
setlocal

:: -- Paths --------------------------------------------------------------------
set PROJECT=D:\Projects\dayz-flairgun-airdrop-mod
set P_MOD=P:\FlareGunAirdropMod
set BUILD=D:\Projects\DayZMods\@FlareGunAirdropMod
set TOOLS=D:\Installs\steam\steamapps\common\DayZ Tools\Bin
set KEY=%BUILD%\keys\FGAM_v1.biprivatekey

echo === 1/4 Syncing source to %P_MOD% ===
if not exist "%P_MOD%" mkdir "%P_MOD%"
copy /y "%PROJECT%\config.cpp" "%P_MOD%\config.cpp" >nul
copy /y "%PROJECT%\mod.cpp"    "%P_MOD%\mod.cpp"    >nul
:: /purge removes files in P: that were deleted from the repo (prevents stale builds)
robocopy "%PROJECT%\FlareGunAirdropMod\scripts"   "%P_MOD%\scripts"   /e /purge /njh /njs /ndl /nc /ns >nul
robocopy "%PROJECT%\FlareGunAirdropMod\data"      "%P_MOD%\data"      /e /purge /njh /njs /ndl /nc /ns >nul
robocopy "%PROJECT%\FlareGunAirdropMod\Graphics"  "%P_MOD%\Graphics"  /e /purge /njh /njs /ndl /nc /ns >nul

echo === 2/4 Binarizing config.cpp -^> config.bin ===
"%TOOLS%\CfgConvert\CfgConvert.exe" -bin -dst "%P_MOD%\config.bin" "%P_MOD%\config.cpp"

echo === 3/4 Packing + signing PBO (CLI, no GUI) ===
:: -packonly keeps the .c scripts as source (binarize would strip them); -sign signs it.
"%TOOLS%\AddonBuilder\AddonBuilder.exe" "%P_MOD%" "%BUILD%\addons" -packonly -prefix=FlareGunAirdropMod -sign="%KEY%" -temp="P:\_fgam_packtmp" -clear

echo === 4/4 Done ===
if exist "%BUILD%\addons\FlareGunAirdropMod.pbo" (
    echo Built: "%BUILD%\addons\FlareGunAirdropMod.pbo"
    echo Now run 2_deploy.bat to push it to the server + client, then restart the server.
) else (
    echo BUILD FAILED - no PBO produced. Check the AddonBuilder output above.
)
echo.
pause
endlocal
