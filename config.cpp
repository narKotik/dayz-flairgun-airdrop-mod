class CfgPatches
{
    class FlareGunAirdropMod
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Weapons_Projectiles", "DZ_Weapons_Firearms", "DZ_Data"};
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


class CfgMagazines
{
    class Ammo_Flare;
    class Ammo_FlareRed;
    class Ammo_FlareGreen;
    class Ammo_FlareBlue;

    class FGAM_Mag_Red : Ammo_FlareRed
    {
        scope = 2;
        displayName = "Signal Flare (Red)";
        descriptionShort = "Toxic zone + military loot drop.";
        weight = 80;
    };

    class FGAM_Mag_Green : Ammo_FlareGreen
    {
        scope = 2;
        displayName = "Signal Flare (Green)";
        descriptionShort = "Survival kit airdrop.";
        weight = 80;
    };

    class FGAM_Mag_Blue : Ammo_FlareBlue
    {
        scope = 2;
        displayName = "Signal Flare (Blue)";
        descriptionShort = "Medical airdrop.";
        weight = 80;
    };

    class FGAM_Mag_White : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (White)";
        descriptionShort = "Food and water airdrop.";
        weight = 80;
    };

    class FGAM_Mag_Yellow : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Yellow)";
        descriptionShort = "CBRN protection airdrop.";
        weight = 80;
    };

    class FGAM_Mag_Black : Ammo_FlareBlue
    {
        scope = 2;
        displayName = "Signal Flare (Dark)";
        descriptionShort = "Top-tier weapons cache.";
        weight = 80;
    };

    class FGAM_Mag_Orange : Ammo_FlareRed
    {
        scope = 2;
        displayName = "Signal Flare (Orange)";
        descriptionShort = "Construction and vehicle parts.";
        weight = 80;
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
