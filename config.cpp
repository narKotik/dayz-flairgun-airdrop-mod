class CfgPatches
{
    class FlareGunAirdropMod
    {
        units[] = {
            "FGAM_AirdropContainer",
            "FGAM_Box_Red", "FGAM_Box_Green", "FGAM_Box_Blue", "FGAM_Box_White",
            "FGAM_Box_Yellow", "FGAM_Box_Black", "FGAM_Box_Orange"
        };
        weapons[] = {
            "FGAM_Flaregun_Coast", "FGAM_Flaregun_Mid", "FGAM_Flaregun_Mil"
        };
        requiredVersion = 0.1;
        requiredAddons[] =
        {
            "DZ_Data",
            "DZ_Weapons_Firearms",
            "DZ_Weapons_Magazines",
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
        author = "FanaticalWarlock";
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
    class Box_Base;

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

    // ── Flare ammo boxes ────────────────────────────────────────────────────
    // The flare magazines themselves (below, CfgMagazines) can't be placed on the
    // ground by CE - they share vanilla's Flare_SingleRound.p3d, a thin rod-shaped
    // model that fails CE's surface-placement test almost 100% of the time
    // (confirmed via RPT "exceeded max tests" / "is hard to place"; this is why
    // Bohemia themselves ship Ammo_Flare/Ammo_FlareRed/Blue/Green at nominal=0).
    // Fix: ground loot is this BOX instead, using the vanilla 00Buck_10RoundBox
    // model (proven to place fine - see AmmoBox_12gaSlug_10Rnd/AmmoBox_12gaRubberSlug_10Rnd
    // in weapons_ammunition.pbo, both CfgVehicles : Box_Base with this same model).
    // Box_Base already wires up the native "Unpack" action (ActionUnpackBox, in
    // 4_world/classes/useractionscomponent/actions/continuous/actionunpackbox.c):
    // on use it reads this class's CfgVehicles > Resources child, and if that
    // child's name matches a CfgMagazines class it replaces the box in-hand with
    // ONE of that magazine, pre-loaded to `value` rounds via ServerSetAmmoCount.
    // No custom script action needed - this is the same vanilla mechanic real
    // ammo boxes use to unpack into loose rounds.
    class FGAM_Box_FlareBase : Box_Base
    {
        scope = 0;
        model = "\dz\weapons\ammunition\00Buck_10RoundBox.p3d";
        rotationFlags = 17;
        weight = 355;
        hiddenSelections[] = { "zbytek" };
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0, {"DZ\weapons\ammunition\data\00buck_box.rvmat"}},
                        {0.7, {"DZ\weapons\ammunition\data\00buck_box.rvmat"}},
                        {0.5, {"DZ\weapons\ammunition\data\00buck_box_damage.rvmat"}},
                        {0.3, {"DZ\weapons\ammunition\data\00buck_box_damage.rvmat"}},
                        {0.0, {"DZ\weapons\ammunition\data\00buck_box_destruct.rvmat"}}
                    };
                };
            };
        };
        class AnimEvents
        {
            class SoundWeapon
            {
                class interact
                {
                    soundset = "ammoboxUnpack_SoundSet";
                    id = 70;
                };
            };
        };
    };

    // 1 round per box (magazine capacity is 5, see FGAM_Mag_FlareBase.count) -
    // hardcoded; bump `value` below to change it.
    class FGAM_Box_Red : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Red)";
        descriptionShort = "Unpack for a red signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_red_co.paa" };
        class Resources { class FGAM_Mag_Red { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_Green : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Green)";
        descriptionShort = "Unpack for a green signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_green_co.paa" };
        class Resources { class FGAM_Mag_Green { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_Blue : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Blue)";
        descriptionShort = "Unpack for a blue signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_blue_co.paa" };
        class Resources { class FGAM_Mag_Blue { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_White : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - White)";
        descriptionShort = "Unpack for a white signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_white_co.paa" };
        class Resources { class FGAM_Mag_White { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_Yellow : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Yellow)";
        descriptionShort = "Unpack for a yellow signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_yellow_co.paa" };
        class Resources { class FGAM_Mag_Yellow { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_Black : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Dark)";
        descriptionShort = "Unpack for a dark signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_black_co.paa" };
        class Resources { class FGAM_Mag_Black { value = 1; variable = "quantity"; }; };
    };
    class FGAM_Box_Orange : FGAM_Box_FlareBase
    {
        scope = 2;
        displayName = "Ammo Box (Signal Flares - Orange)";
        descriptionShort = "Unpack for an orange signal flare.";
        hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_orange_co.paa" };
        class Resources { class FGAM_Mag_Orange { value = 1; variable = "quantity"; }; };
    };
};

// ──────────────────────────────────────────────────────────────────────────
// FGAM coloured flares. All use the base (white) flare cartridge MODEL in the
// inventory (the cartridge colour is baked into vanilla models and can't change),
// but each fires a COLOURED burning flare - slow, bright, coloured smoke - and
// triggers its airdrop. See FGAM_FlareVisuals.c for the colour/recipe and
// FGAM_Magazines.c for the script-class bindings.
//
// TUNE FLIGHT in FGAM_Bullet_FlareBase below: initSpeed/coefGravity = rise/fall,
// timeToLive = burn seconds.
// ──────────────────────────────────────────────────────────────────────────
class CfgAmmo
{
    class Bullet_Flare;

    class FGAM_Bullet_FlareBase : Bullet_Flare
    {
        scope = 1;
        model = "\dz\weapons\projectiles\Flare_Projectile.p3d";
        proxyShape = "\dz\weapons\projectiles\Flare_SingleRound.p3d";
        casing = "FxCartridge_Flare";
        round = "FxRound_Flare";
        spawnPileType = "Ammo_Flare";
        caliber = 0;
        airFriction = -0.002;
        airFrictionChangeOnActivation = -0.029;
        typicalSpeed = 25;
        initSpeed = 25;
        weight = 0.005;
        coefGravity = 0.009;
        soundFly[] = {};
        supersonicCrackNear[] = {};
        supersonicCrackFar[] = {};
        hit = 5;
        indirectHit = 2;
        indirectHitRange = 0.2;
        simulation = "shotIlluminating";
        simulationStep = 0.05;
        soundHit[] = {"", 0, 1};
        SimulationScriptClass = "FGAM_FlareSimulation_Yellow";
        timeToLive = 180;
        explosionTime = 180;
        radius = 300;
        class DamageApplied
        {
            type = "Projectile";
            dispersion = 0;
            bleedThreshold = 0;
            defaultDamageOverride[] = {{0.5, 1}};
            class Health { damage = 10; };
            class Blood { damage = 10; };
            class Shock { damage = 50; };
        };
    };

    class FGAM_Bullet_FlareRed    : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Red"; };
    class FGAM_Bullet_FlareGreen  : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Green"; };
    class FGAM_Bullet_FlareBlue   : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Blue"; };
    class FGAM_Bullet_FlareYellow : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Yellow"; };
    class FGAM_Bullet_FlareWhite  : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_White"; };
    class FGAM_Bullet_FlareOrange : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Orange"; };
    class FGAM_Bullet_FlareBlack  : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Black"; };
};

// ──────────────────────────────────────────────────────────────────────────
// AMMO-TYPE REGISTRY — the piece whose absence crashed chambering.
// Every cartridge ammo that a magazine can hold MUST be registered here so the
// engine can map the ammo class to an internal integer type-id. When a flare is
// chambered, WeaponChambering.ShowBullet calls Magazine.GetCartridgeAtIndex,
// which reads that id; if the cartridge's ammo is unregistered the lookup is
// invalid and the engine access-violates (crash at GetCartridgeAtIndex, 0x98).
// One entry per ammo referenced by a magazine's `ammo=`.
// ──────────────────────────────────────────────────────────────────────────
class cfgAmmoTypes
{
    class AType_FGAM_Bullet_FlareRed    { name = "FGAM_Bullet_FlareRed"; };
    class AType_FGAM_Bullet_FlareGreen  { name = "FGAM_Bullet_FlareGreen"; };
    class AType_FGAM_Bullet_FlareBlue   { name = "FGAM_Bullet_FlareBlue"; };
    class AType_FGAM_Bullet_FlareYellow { name = "FGAM_Bullet_FlareYellow"; };
    class AType_FGAM_Bullet_FlareWhite  { name = "FGAM_Bullet_FlareWhite"; };
    class AType_FGAM_Bullet_FlareOrange { name = "FGAM_Bullet_FlareOrange"; };
    class AType_FGAM_Bullet_FlareBlack  { name = "FGAM_Bullet_FlareBlack"; };
};

class CfgMagazines
{
    class Ammo_Flare;

    // Complete base flare magazine. The script-class binding to Ammunition_Base
    // is in FGAM_Magazines.c.
    class FGAM_Mag_FlareBase : Ammo_Flare
    {
        scope = 0;
        model = "\dz\weapons\ammunition\Flare_SingleRound.p3d";
        rotationFlags = 34;
        weight = 80;
        count = 5;
        class DamageSystem
        {
            class GlobalHealth
            {
                class Health
                {
                    hitpoints = 100;
                    healthLevels[] =
                    {
                        {1.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                        {0.7, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow.rvmat"}},
                        {0.5, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                        {0.3, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_damage.rvmat"}},
                        {0.0, {"DZ\weapons\pistols\flaregun\data\flaregun_yellow_destruct.rvmat"}}
                    };
                };
            };
        };
    };

    class FGAM_Mag_Red : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Red)";
        descriptionShort = "Toxic zone + military loot drop.";
        ammo = "FGAM_Bullet_FlareRed";
    };
    class FGAM_Mag_Green : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Green)";
        descriptionShort = "Survival kit airdrop.";
        ammo = "FGAM_Bullet_FlareGreen";
    };
    class FGAM_Mag_Blue : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Blue)";
        descriptionShort = "Medical airdrop.";
        ammo = "FGAM_Bullet_FlareBlue";
    };
    class FGAM_Mag_Yellow : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Yellow)";
        descriptionShort = "CBRN protection airdrop.";
        ammo = "FGAM_Bullet_FlareYellow";
    };
    class FGAM_Mag_White : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (White)";
        descriptionShort = "Food and water airdrop.";
        ammo = "FGAM_Bullet_FlareWhite";
    };
    class FGAM_Mag_Orange : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Orange)";
        descriptionShort = "Construction and vehicle parts.";
        ammo = "FGAM_Bullet_FlareOrange";
    };
    class FGAM_Mag_Black : FGAM_Mag_FlareBase
    {
        scope = 2;
        displayName = "Signal Flare (Dark)";
        descriptionShort = "Top-tier weapons cache.";
        ammo = "FGAM_Bullet_FlareBlack";
    };
};

class CfgWeapons
{
    class Pistol_Base;
    // Modify Flaregun_BASE (not the derived Flaregun) so this merges with other
    // mods that also extend the flare gun's chamberableFrom. Adding to the derived
    // class shadows the inherited list and drops other mods' flares.
    class Flaregun_Base : Pistol_Base
    {
        chamberableFrom[] += {
            "FGAM_Mag_Red", "FGAM_Mag_Green", "FGAM_Mag_Blue",
            "FGAM_Mag_White", "FGAM_Mag_Yellow", "FGAM_Mag_Black",
            "FGAM_Mag_Orange"
        };
        // Non-repairable: empty repair-kit list means the Weapon Cleaning Kit
        // (and any other kit) cannot restore condition. Combined with the
        // per-shot wear in FGAM_FlareGun.c, the gun is a limited-use consumable -
        // once worn down it stays down, and once RUINED it's dead for good.
        repairableWithKits[] = {};
        repairCosts[] = {};
    };
    class Flaregun;  // vanilla concrete gun (inherits Flaregun_Base with our changes)

    // Tiered spawn variants. These are the SAME gun as vanilla - each inherits
    // Flaregun, so it automatically gets our coloured-flare chambering, non-repair
    // and per-shot wear. They exist ONLY so the CE can assign a different spawn
    // CONDITION per loot tier (set in db/FGAM_spawnabletypes.xml) and a different
    // usage/rate per tier (db/FGAM_types.xml). displayName is left as vanilla so
    // players can't tell them apart except by the gun's condition.
    // Set the vanilla "Flaregun" to nominal 0 in the mission's base db/types.xml so
    // only these tiered variants world-spawn.
    class FGAM_Flaregun_Coast : Flaregun { scope = 2; };  // coast/town  - Badly Damaged, 1 use
    class FGAM_Flaregun_Mid   : Flaregun { scope = 2; };  // indus/police - Worn, 3 uses
    class FGAM_Flaregun_Mil   : Flaregun { scope = 2; };  // military     - Pristine, 4 uses
};
