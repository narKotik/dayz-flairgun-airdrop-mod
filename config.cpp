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

class CfgAmmo
{
    class Ammo_Flare;
    class Ammo_FlareRed;
    class Ammo_FlareGreen;
    class Ammo_FlareBlue;

    class FGAM_Bullet_Red : Ammo_FlareRed
    {
        flareColor[] = {1.0, 0.05, 0.05, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
    };
    class FGAM_Bullet_Green : Ammo_FlareGreen
    {
        flareColor[] = {0.05, 1.0, 0.05, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
    };
    class FGAM_Bullet_Blue : Ammo_FlareBlue
    {
        flareColor[] = {0.05, 0.3, 1.0, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
    };
    class FGAM_Bullet_White : Ammo_Flare
    {
        flareColor[] = {1.0, 1.0, 1.0, 1.0};
        flareIntensity = 10000;
        flareDistance = 1500;
        flareFadeTime = 10;
    };
    class FGAM_Bullet_Yellow : Ammo_Flare
    {
        flareColor[] = {1.0, 0.85, 0.0, 1.0};
        flareIntensity = 9000;
        flareDistance = 1300;
        flareFadeTime = 8;
    };
    class FGAM_Bullet_Black : Ammo_FlareBlue
    {
        flareColor[] = {0.3, 0.0, 0.5, 0.6};
        flareIntensity = 1500;
        flareDistance = 300;
        flareFadeTime = 3;
    };
    class FGAM_Bullet_Orange : Ammo_FlareRed
    {
        flareColor[] = {1.0, 0.45, 0.0, 1.0};
        flareIntensity = 8500;
        flareDistance = 1200;
        flareFadeTime = 8;
    };
};

class CfgMagazines
{
    class Ammo_Flare;

    class FGAM_Mag_Red : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Red)";
        descriptionShort = "Toxic zone + military loot drop.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_Red";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun.rvmat"}},
                {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun.rvmat"}},
                {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_damage.rvmat"}},
                {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_damage.rvmat"}},
                {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_Green : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Green)";
        descriptionShort = "Survival kit airdrop.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Green.p3d";
        ammo = "FGAM_Bullet_Green";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_green.rvmat"}},
                {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_green.rvmat"}},
                {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_green_damage.rvmat"}},
                {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_green_damage.rvmat"}},
                {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_green_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_Blue : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Blue)";
        descriptionShort = "Medical airdrop.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Blue.p3d";
        ammo = "FGAM_Bullet_Blue";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_blue.rvmat"}},
                {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_blue.rvmat"}},
                {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_damage.rvmat"}},
                {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_damage.rvmat"}},
                {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_White : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (White)";
        descriptionShort = "Food and water airdrop.";
        model = "\dz\weapons\ammunition\Flare_SingleRound.p3d";
        ammo = "FGAM_Bullet_White";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                {0.7, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                {0.5, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                {0.3, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                {0.0, {"FlareGunAirdropMod\data\flaregun_white_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_Yellow : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Yellow)";
        descriptionShort = "CBRN protection airdrop.";
        model = "\dz\weapons\ammunition\Flare_SingleRound.p3d";
        ammo = "FGAM_Bullet_Yellow";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_Black : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Dark)";
        descriptionShort = "Top-tier weapons cache.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_Black";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                {0.7, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                {0.5, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                {0.3, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                {0.0, {"FlareGunAirdropMod\data\flaregun_dark_destruct.rvmat"}}
            };
        }; }; };
    };

    class FGAM_Mag_Orange : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Orange)";
        descriptionShort = "Construction and vehicle parts.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_Orange";
        count = 1;
        mass = 30;
        class DamageSystem { class GlobalHealth { class Health { hitpoints = 100;
            healthLevels[] = {
                {1.0, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                {0.7, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                {0.5, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                {0.3, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                {0.0, {"FlareGunAirdropMod\data\flaregun_orange_destruct.rvmat"}}
            };
        }; }; };
    };
};

class CfgWeapons
{
    class FlareGun_Base;
    class FlareGun : FlareGun_Base
    {
        canBeRepaired = 0;
        magazines[] += {
            "FGAM_Mag_Red", "FGAM_Mag_Green", "FGAM_Mag_Blue",
            "FGAM_Mag_White", "FGAM_Mag_Yellow", "FGAM_Mag_Black",
            "FGAM_Mag_Orange"
        };
    };
};