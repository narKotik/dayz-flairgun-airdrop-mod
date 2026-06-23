class CfgPatches
{
    class FlareGunAirdropMod
    {
        units[] = {"FGAM_AirdropContainer"};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Weapons_Firearms",
            "DZ_Weapons_Projectiles",
            "DZ_Weapons_Ammunition",
            "DZ_Gear_Camping"
        };
    };
};

class CfgMods
{
    class FlareGunAirdropMod
    {
        dir = "FlareGunAirdropMod";
        picture = "";
        action = "";
        hideName = 1;
        hidePicture = 1;
        name = "Flare Gun Airdrop Mod";
        credits = "";
        author = "";
        authorID = "0";
        version = "1.0";
        extra = 0;
        type = "mod";

        dependencies[] = {"Game", "World", "Mission"};

        class defs
        {
            class gameScriptModule
            {
                value = "";
                files[] = {"FlareGunAirdropMod/scripts/3_Game"};
            };
            class worldScriptModule
            {
                value = "";
                files[] = {"FlareGunAirdropMod/scripts/4_World"};
            };
            class missionScriptModule
            {
                value = "";
                files[] = {"FlareGunAirdropMod/scripts/5_Mission"};
            };
        };
    };
};

// ──────────────────────────────────────────────────────────────────────────
// Airdrop container — a big-cargo crate so the whole loot table fits inside.
// Built from the vanilla SeaChest (recognizable metal crate, sounds, damage)
// but with a much larger cargo grid. The container is spawned high and lowered
// by FGAM_AirdropManager, then auto-removed after a configurable lifetime.
// ──────────────────────────────────────────────────────────────────────────
class CfgVehicles
{
    class Container_Base;
    class SeaChest;
    class ContaminatedArea_Local;

    class FGAM_AirdropContainer : SeaChest
    {
        scope = 2;
        displayName = "Airdrop Crate";
        descriptionShort = "Supply crate dropped from the sky. Loot it before it is recovered.";
        // Big cargo grid (10 wide x 40 tall = 400 cells) so rifles, vests and
        // bulky parts all fit inside instead of spilling onto the ground.
        class Cargo
        {
            itemsCargoSize[] = {10, 40};
            openable = 0;
            allowOwnedCargoManipulation = 1;
        };
    };

    // Toxic gas zone for the red flare. Behaviour is in FGAM_ToxicArea.c; this
    // config entry just makes the class spawnable (same minimal shape as the
    // vanilla contaminated-area classes).
    class FGAM_ToxicArea : ContaminatedArea_Local
    {
        scope = 2;
    };
};

// ──────────────────────────────────────────────────────────────────────────
// Coloured flare cartridges.
//
// Colour is applied ONLY through the DamageSystem healthLevels rvmat — the exact
// mechanism vanilla uses for its own Red/Green/Blue/Yellow flares — so the round
// keeps the stock flare model, ammo and chambering behaviour intact.
//
// IMPORTANT: do NOT override `ammo` here with a custom CfgAmmo class. The flare
// gun's chambering FSM (WeaponChambering.ShowBullet -> GetCartridgeAtIndex) does
// a native cartridge lookup the instant a round is chambered; pointing a flare at
// a custom Bullet_* class makes that native call read invalid memory and hard-
// crashes the client. The in-air trace therefore inherits from the closest
// vanilla flare colour.
// ──────────────────────────────────────────────────────────────────────────
class CfgMagazines
{
    class Ammo_Flare;
    class Ammo_FlareRed;
    class Ammo_FlareGreen;
    class Ammo_FlareBlue;

    // Red — vanilla red model + red trace
    class FGAM_Mag_Red : Ammo_FlareRed
    {
        scope = 2;
        displayName = "Signal Flare (Red)";
        descriptionShort = "Toxic zone + military loot drop.";
        weight = 80;
    };

    // Green — vanilla green model + green trace
    class FGAM_Mag_Green : Ammo_FlareGreen
    {
        scope = 2;
        displayName = "Signal Flare (Green)";
        descriptionShort = "Survival kit airdrop.";
        weight = 80;
    };

    // Blue — vanilla blue model + blue trace
    class FGAM_Mag_Blue : Ammo_FlareBlue
    {
        scope = 2;
        displayName = "Signal Flare (Blue)";
        descriptionShort = "Medical airdrop.";
        weight = 80;
    };

    // White — base flare recoloured white (base trace is white/yellow)
    class FGAM_Mag_White : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (White)";
        descriptionShort = "Food and water airdrop.";
        weight = 80;
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                        {0.7, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                        {0.5, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                        {0.3, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                        {0.0, {"FlareGunAirdropMod\data\flaregun_white_destruct.rvmat"}}
                    };
                };
            };
        };
    };

    // Yellow — base flare already uses the yellow rvmat; leave it as-is
    class FGAM_Mag_Yellow : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Yellow)";
        descriptionShort = "CBRN protection airdrop.";
        weight = 80;
    };

    // Orange — red model recoloured orange (trace inherits red — closest match)
    class FGAM_Mag_Orange : Ammo_FlareRed
    {
        scope = 2;
        displayName = "Signal Flare (Orange)";
        descriptionShort = "Construction and vehicle parts.";
        weight = 80;
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                        {0.7, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                        {0.5, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                        {0.3, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                        {0.0, {"FlareGunAirdropMod\data\flaregun_orange_destruct.rvmat"}}
                    };
                };
            };
        };
    };

    // Black / Dark — blue model recoloured dark (trace inherits blue)
    class FGAM_Mag_Black : Ammo_FlareBlue
    {
        scope = 2;
        displayName = "Signal Flare (Dark)";
        descriptionShort = "Top-tier weapons cache.";
        weight = 80;
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                        {0.7, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                        {0.5, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                        {0.3, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                        {0.0, {"FlareGunAirdropMod\data\flaregun_dark_destruct.rvmat"}}
                    };
                };
            };
        };
    };
};

class CfgWeapons
{
    class Flaregun_Base;
    class Flaregun : Flaregun_Base
    {
        chamberableFrom[] += {
            "FGAM_Mag_Red", "FGAM_Mag_Green", "FGAM_Mag_Blue",
            "FGAM_Mag_White", "FGAM_Mag_Yellow", "FGAM_Mag_Black",
            "FGAM_Mag_Orange"
        };
    };
};
