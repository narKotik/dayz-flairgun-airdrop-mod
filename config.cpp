class CfgPatches
{
    class FlareGunAirdropMod
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = {"DZ_Weapons_Projectiles", "DZ_Weapons_Firearms", "DZ_Weapons_Pistol", "DZ_Data"};
    };
};

// ──────────────────────────────────────────────────────────────────────────────
//  GLOBLAL MOD SETTINGS
// ──────────────────────────────────────────────────────────────────────────────
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

// ──────────────────────────────────────────────────────────────────────────────
//  AMMUNITION — projectile behaviour (light colour, smoke)
//  Model + inventory appearance is defined in CfgMagazines below.
// ──────────────────────────────────────────────────────────────────────────────
class CfgAmmo
{
    class Ammo_Flare;       // vanilla base
    class Ammo_FlareRed;
    class Ammo_FlareGreen;
    class Ammo_FlareBlue;

    // ── RED ────────────────────────────────────────────────────────────────
    class FGAM_Bullet_Red : Ammo_FlareRed
    {
        flareColor[] = {1.0, 0.05, 0.05, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
        smokeColor[] = {0.8, 0.1, 0.1};
        FGAM_EventType = "RED";
    };

    // ── GREEN ──────────────────────────────────────────────────────────────
    class FGAM_Bullet_Green : Ammo_FlareGreen
    {
        flareColor[] = {0.05, 1.0, 0.05, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
        smokeColor[] = {0.1, 0.8, 0.1};
        FGAM_EventType = "GREEN";
    };

    // ── BLUE ───────────────────────────────────────────────────────────────
    class FGAM_Bullet_Blue : Ammo_FlareBlue
    {
        flareColor[] = {0.05, 0.3, 1.0, 1.0};
        flareIntensity = 8000;
        flareDistance = 1200;
        flareFadeTime = 8;
        smokeColor[] = {0.1, 0.3, 0.9};
        FGAM_EventType = "BLUE";
    };

    // ── WHITE ──────────────────────────────────────────────────────────────
    class FGAM_Bullet_White : Ammo_Flare
    {
        flareColor[] = {1.0, 1.0, 1.0, 1.0};
        flareIntensity = 10000;
        flareDistance = 1500;
        flareFadeTime = 10;
        smokeColor[] = {0.9, 0.9, 0.9};
        FGAM_EventType = "WHITE";
    };

    // ── YELLOW — inherits Ammo_Flare which is already yellow ───────────────
    class FGAM_Bullet_Yellow : Ammo_Flare
    {
        flareColor[] = {1.0, 0.85, 0.0, 1.0};
        flareIntensity = 9000;
        flareDistance = 1300;
        flareFadeTime = 8;
        smokeColor[] = {0.9, 0.8, 0.0};
        FGAM_EventType = "YELLOW";
    };

    // ── DARK PURPLE (visually distinct, no blinding light) ─────────────────
    class FGAM_Bullet_DarkPurple : Ammo_FlareBlue
    {
        flareColor[] = {0.3, 0.0, 0.5, 0.6};   // dim purple glow
        flareIntensity = 1500;
        flareDistance = 300;
        flareFadeTime = 3;
        smokeColor[] = {0.1, 0.0, 0.15};       // dark purple smoke
        smokeDensity = 3.0;
        FGAM_EventType = "BLACK";               // script still calls it BLACK internally
    };

    // ── ORANGE ─────────────────────────────────────────────────────────────
    class FGAM_Bullet_Orange : Ammo_FlareRed
    {
        flareColor[] = {1.0, 0.45, 0.0, 1.0};
        flareIntensity = 8500;
        flareDistance = 1200;
        flareFadeTime = 8;
        smokeColor[] = {0.9, 0.4, 0.0};
        FGAM_EventType = "ORANGE";
    };
};

// ──────────────────────────────────────────────────────────────────────────────
//  MAGAZINES — inventory item (model + rvmat + ammo link)
//
//  Models reused from vanilla (no client download for existing colours).
//  New colours use vanilla models + custom rvmat packed inside this mod
//  → mod requires client install (Workshop) for Yellow, DarkPurple, Orange.
//
//  Vanilla model paths:
//    White/Yellow base : \dz\weapons\ammunition\Flare_SingleRound.p3d
//    Red               : \dz\weapons\ammunition\Flare_SingleRound_Red.p3d
//    Green             : \dz\weapons\ammunition\Flare_SingleRound_Green.p3d
//    Blue              : \dz\weapons\ammunition\Flare_SingleRound_Blue.p3d
// ──────────────────────────────────────────────────────────────────────────────
class CfgMagazines
{
    class Ammo_Flare;   // vanilla base magazine class

    // ── Helper macro for DamageSystem — same for all flare rounds ───────────
    // (copied inline per class since config.cpp has no macros at this stage)

    // ── RED ────────────────────────────────────────────────────────────────
    class FGAM_Mag_Red : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Red)";
        descriptionShort = "Fire into the sky to mark a zone. After 5 minutes a toxic cloud will form at the signal point. Military-grade equipment will be found inside — if you dare to enter.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_Red";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0,   {"DZ\weapons\pistols\flaregun\data\flaregun.rvmat"}},
                        {0.7,   {"DZ\weapons\pistols\flaregun\data\flaregun.rvmat"}},
                        {0.5,   {"DZ\weapons\pistols\flaregun\data\flaregun_damage.rvmat"}},
                        {0.3,   {"DZ\weapons\pistols\flaregun\data\flaregun_damage.rvmat"}},
                        {0.0,   {"DZ\weapons\pistols\flaregun\data\flaregun_destruct.rvmat"}}
                    };
                };
            };
        };
    };

    // ── GREEN ──────────────────────────────────────────────────────────────
    class FGAM_Mag_Green : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Green)";
        descriptionShort = "Fire into the sky to call a survival airdrop. A crate with basic tools, shelter and medical supplies will fall at the signal point. Good for getting back on your feet.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Green.p3d";
        ammo = "FGAM_Bullet_Green";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_green.rvmat"}},
                    {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_green.rvmat"}},
                    {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_green_damage.rvmat"}},
                    {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_green_damage.rvmat"}},
                    {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_green_destruct.rvmat"}}
                };
            }; };
        };
    };

    // ── BLUE ───────────────────────────────────────────────────────────────
    class FGAM_Mag_Blue : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Blue)";
        descriptionShort = "Fire into the sky to call a humanitarian medical drop. Blood bags, surgical kits and antibiotics will arrive at the signal point. Can mean the difference between life and death.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Blue.p3d";
        ammo = "FGAM_Bullet_Blue";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_blue.rvmat"}},
                    {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_blue.rvmat"}},
                    {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_damage.rvmat"}},
                    {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_damage.rvmat"}},
                    {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_blue_destruct.rvmat"}}
                };
            }; };
        };
    };

    // ── WHITE ──────────────────────────────────────────────────────────────
    class FGAM_Mag_White : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (White)";
        descriptionShort = "Fire into the sky to call a food and water drop. High-calorie rations, clean water and purification tablets will fall at the signal point. Enough to keep a group going for days.";
        model = "\dz\weapons\ammunition\Flare_SingleRound.p3d";
        ammo = "FGAM_Bullet_White";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                    {0.7, {"FlareGunAirdropMod\data\flaregun_white.rvmat"}},
                    {0.5, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                    {0.3, {"FlareGunAirdropMod\data\flaregun_white_damage.rvmat"}},
                    {0.0, {"FlareGunAirdropMod\data\flaregun_white_destruct.rvmat"}}
                };
            }; };
        };
    };

    // ── YELLOW — uses vanilla base model (already yellow-tinted) ───────────
    class FGAM_Mag_Yellow : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Yellow)";
        descriptionShort = "Fire into the sky to call a CBRN protection drop. A full NBC suit, gas mask, filters and antidotes will fall at the signal point. Essential before entering contaminated zones.";
        model = "\dz\weapons\ammunition\Flare_SingleRound.p3d";
        ammo = "FGAM_Bullet_Yellow";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                    {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                    {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                    {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                    {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_destruct.rvmat"}}
                };
            }; };
        };
    };

    // ── DARK (near-black charcoal) — uses red model base + custom dark rvmat
    class FGAM_Mag_Black : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Dark)";
        descriptionShort = "Fire into the sky to call a black-ops weapons cache. Top-tier firearms and ammunition will fall at the signal point. Produces no light — only a thick dark smoke trail. Others may not see it coming.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_DarkPurple";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                    {0.7, {"FlareGunAirdropMod\data\flaregun_dark.rvmat"}},
                    {0.5, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                    {0.3, {"FlareGunAirdropMod\data\flaregun_dark_damage.rvmat"}},
                    {0.0, {"FlareGunAirdropMod\data\flaregun_dark_destruct.rvmat"}}
                };
            }; };
        };
    };

    // ── ORANGE — uses red model + custom orange rvmat ──────────────────────
    class FGAM_Mag_Orange : Ammo_Flare
    {
        scope = 2;
        displayName = "Signal Flare (Orange)";
        descriptionShort = "Fire into the sky to call a builder's supply drop. Tools, nails, barbed wire and rare vehicle parts will fall at the signal point. Everything you need to fortify a base or get a car running.";
        model = "\dz\weapons\ammunition\Flare_SingleRound_Red.p3d";
        ammo = "FGAM_Bullet_Orange";
        count = 1;
        mass = 30;
        class DamageSystem
        {
            class GlobalHealth { class Health { hitpoints = 100;
                healthLevels[] = {
                    {1.0, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                    {0.7, {"FlareGunAirdropMod\data\flaregun_orange.rvmat"}},
                    {0.5, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                    {0.3, {"FlareGunAirdropMod\data\flaregun_orange_damage.rvmat"}},
                    {0.0, {"FlareGunAirdropMod\data\flaregun_orange_destruct.rvmat"}}
                };
            }; };
        };
    };
};

// ──────────────────────────────────────────────────────────────────────────────
//  FLARE GUN — extend vanilla, disable repair, set durability thresholds
// ──────────────────────────────────────────────────────────────────────────────
class CfgWeapons
{
    class FlareGun_Base;
    class FlareGun : FlareGun_Base
    {
        // Prevent any repair actions
        canBeRepaired = 0;

        // Compatible magazines (all mod colors + any vanilla ones we keep)
        magazines[] =
        {
            "FGAM_Mag_Red", "FGAM_Mag_Green", "FGAM_Mag_Blue",
            "FGAM_Mag_White", "FGAM_Mag_Yellow", "FGAM_Mag_Black",
            "FGAM_Mag_Orange"
        };
    };
};

// ──────────────────────────────────────────────────────────────────────────────
//  LOOT TABLES — spawn economy
// ──────────────────────────────────────────────────────────────────────────────
class CfgSpawnableTypes
{
    // Heli crash — gun + random colored mag
    class FlareGun
    {
        // Handled by types.xml & CE — see cfgeconomycore.xml
    };
};
