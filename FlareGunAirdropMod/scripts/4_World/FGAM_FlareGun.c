// FGAM_FlareGun - server-side airdrop trigger.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareGun.c
//
// The coloured visuals live in FGAM_FlareVisuals.c (a client-side FlareSimulation).
// The AIRDROP, however, must be triggered on the SERVER - and on a dedicated server
// the engine never instantiates the flare's visual FlareSimulation (no lights/particles
// are needed server-side), so a trigger placed there never runs. The weapon's OnFire
// DOES run server-side, so that is where we trigger from.
//
// We read the chambered FGAM cartridge to know the colour (our own ammo classes make
// this reliable - no colour-tracking hacks needed), and only fire the event if the
// flare was actually launched into the sky: aimed up far enough AND no roof overhead,
// so shooting flat or into a ceiling indoors does nothing.
//
// This does NOT degrade the gun or touch storage hooks (those caused problems before);
// it is purely the airdrop trigger.

modded class Weapon_Base
{
    // EEFired is the engine's fire callback. It runs on the dedicated server (vanilla
    // guards only its CLIENT effects with !IsDedicatedServer) and gives us the fired
    // round's CfgAmmo class directly - unlike GetChamberAmmoTypeName, which returns the
    // colour-blind spawnPileType ("Ammo_Flare") and was why detection silently failed.
    override void EEFired(int muzzleType, int mode, string ammoType)
    {
        super.EEFired(muzzleType, mode, ammoType);

        if (!GetGame().IsServer()) return;

        // Wear the flare gun down one condition tier per shot so it is a limited-use
        // item (~4 shots Pristine -> Ruined). Applies to ANY flare fired, not just
        // ours. Health is saved by the engine automatically, so no custom store hooks
        // are needed (those broke other weapons before - see git 07a8d73). Non-repair
        // is enforced by repairableWithKits[]={} in config.cpp.
        if (FGAM_IsFlaregun())
            FGAM_DegradeCondition();

        string color = FGAM_ColorFromAmmo(ammoType);
        if (color == "") return; // not one of our flares - ignore vanilla/other mods

        if (!FGAM_FiredUpward())
            return;

        vector dropPos = FGAM_DropPosition();
        FGAM_AirdropManager.Get().OnFlareEvent(color, dropPos, dropPos);
    }

    // True for the vanilla flare gun AND our tiered spawn variants (FGAM_Flaregun_*),
    // so per-shot wear applies to all of them but not to other weapons/mods' guns.
    protected bool FGAM_IsFlaregun()
    {
        string t = GetType();
        return t == "Flaregun" || t.Contains("FGAM_Flaregun");
    }

    // Drop the gun one condition tier per call: Pristine -> Worn -> Damaged ->
    // Badly Damaged -> Ruined. A Ruined weapon can't fire, so this hard-caps the
    // number of shots. Uses real health (auto-persisted); no counter/store hooks.
    protected void FGAM_DegradeCondition()
    {
        // GetHealthLevel() is available on Weapon_Base (via EntityAI); GetItemCondition()
        // is NOT - it lives on ItemBase, a different branch. Level: 0=Pristine, 1=Worn,
        // 2=Damaged, 3=Badly Damaged, 4=Ruined. Each shot lands health in the next tier.
        float maxHP = GetMaxHealth("", "Health");
        switch (GetHealthLevel())
        {
            case 0: SetHealth("", "Health", maxHP * 0.55); break; // Pristine     -> Worn
            case 1: SetHealth("", "Health", maxHP * 0.35); break; // Worn         -> Damaged
            case 2: SetHealth("", "Health", maxHP * 0.10); break; // Damaged      -> Badly Damaged
            case 3: SetHealth("", "Health", 0.0);          break; // Badly Damaged -> Ruined
            case 4: break;                                        // already Ruined
        }
    }

    // Map our cartridge ammo class to a colour key. Only FGAM ammo matches, so
    // vanilla and other mods' flares are ignored (they won't trigger airdrops).
    protected string FGAM_ColorFromAmmo(string ammoName)
    {
        ammoName.ToLower();
        if (!ammoName.Contains("fgam_bullet_flare")) return "";
        if (ammoName.Contains("red"))    return "RED";
        if (ammoName.Contains("green"))  return "GREEN";
        if (ammoName.Contains("blue"))   return "BLUE";
        if (ammoName.Contains("yellow")) return "YELLOW";
        if (ammoName.Contains("white"))  return "WHITE";
        if (ammoName.Contains("orange")) return "ORANGE";
        if (ammoName.Contains("black"))  return "BLACK";
        return "";
    }

    // Ground point near the shooter, nudged a few metres aside so the descending
    // crate does not land on the player's head.
    protected vector FGAM_DropPosition()
    {
        vector origin = GetPosition();
        PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
        if (player) origin = player.GetPosition();

        float ang = Math.RandomFloat(0, Math.PI2);
        float r   = Math.RandomFloat(5.0, 9.0);
        origin[0] = origin[0] + Math.Cos(ang) * r;
        origin[2] = origin[2] + Math.Sin(ang) * r;
        return origin;
    }

    // True only if the flare was actually fired up into the open sky. We read the
    // shooter's real up/down aiming angle (valid server-side) and require it to be
    // above minTriggerPitch degrees, and that there is no roof/ceiling overhead.
    protected bool FGAM_FiredUpward()
    {
        PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
        if (!player) return true; // can't determine shooter - don't block

        HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
        if (!hcw) return true;

        float udAngle  = hcw.GetBaseAimingAngleUD(); // degrees, positive = up
        float minPitch = FGAM_Config.Get().FlareSettings.minTriggerPitch;
        if (udAngle < minPitch)
            return false;

        if (MiscGameplayFunctions.IsUnderRoofEx(player, GameConstants.ROOF_CHECK_RAYCAST_DIST, ObjIntersectFire))
            return false;

        return true;
    }
}
