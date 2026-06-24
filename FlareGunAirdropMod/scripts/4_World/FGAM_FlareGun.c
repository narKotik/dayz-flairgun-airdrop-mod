// FGAM_FlareGun
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareGun.c

modded class Weapon_Base
{
    private int m_FGAM_ShotsInState = 0;

    override void OnFire(int muzzle_index)
    {
        string chamberName = "";
        if (GetGame().IsServer() && GetType() == "Flaregun")
        {
            chamberName = GetChamberAmmoTypeName(GetCurrentMuzzle());
            Print("[FGAM] PRE-SUPER GetChamberAmmoTypeName='" + chamberName + "'");
        }

        super.OnFire(muzzle_index);

        if (!GetGame().IsServer()) return;
        if (GetType() != "Flaregun") return;

        // ColorTracker is set by EEItemLocationChanged when a mag leaves cargo (oldType=3)
        string trackerColor = FGAM_ColorTracker.Consume();
        Print("[FGAM] POST-SUPER chamberName='" + chamberName + "' trackerColor='" + trackerColor + "'");

        string color = trackerColor;
        if (color == "")
        {
            // Fallback: derive color from chamberName (works for RED, may be wrong for others)
            chamberName.ToLower();
            if      (chamberName.Contains("_orange")) color = "ORANGE";
            else if (chamberName.Contains("_red"))    color = "RED";
            else if (chamberName.Contains("_green"))  color = "GREEN";
            else if (chamberName.Contains("_black"))  color = "BLACK";
            else if (chamberName.Contains("_blue"))   color = "BLUE";
            else if (chamberName.Contains("_yellow")) color = "YELLOW";
            else if (chamberName.Contains("_white"))  color = "WHITE";
        }

        Print("[FGAM] OnFire final color='" + color + "'");

        if (color != "")
        {
            if (FGAM_FiredUpward())
            {
                FGAM_FlareTracker tracker = new FGAM_FlareTracker(color, GetPosition());
            }
            else
            {
                Print("[FGAM] Flare not fired upward enough - event suppressed");
            }
        }

        m_FGAM_ShotsInState = m_FGAM_ShotsInState + 1;
        int shotsNeeded = FGAM_Config.Get().FlareSettings.shotsPerState;
        if (shotsNeeded < 1) shotsNeeded = 1;
        if (m_FGAM_ShotsInState >= shotsNeeded)
        {
            m_FGAM_ShotsInState = 0;
            FGAM_DegradeCondition();
        }
    }

    // True only if the flare was actually fired up into the sky. We read the
    // player's real up/down aiming angle from the weapon command (the same value
    // the vanilla throwing/firing code uses, valid server-side) and require it to
    // exceed minTriggerPitch degrees above horizontal. This replaces the old code
    // that just ASSUMED the flare rose 80m every time, so firing parallel to the
    // ground no longer triggers an event.
    private bool FGAM_FiredUpward()
    {
        PlayerBase player = PlayerBase.Cast(GetHierarchyRootPlayer());
        if (!player) return true; // can't determine the shooter - don't block

        HumanCommandWeapons hcw = player.GetCommandModifier_Weapons();
        if (!hcw) return true;

        float udAngle  = hcw.GetBaseAimingAngleUD(); // degrees, positive = aiming up
        float minPitch = FGAM_Config.Get().FlareSettings.minTriggerPitch;

        if (udAngle < minPitch)
        {
            Print("[FGAM] aim up/down angle=" + udAngle + " deg < " + minPitch + " - too low, suppressed");
            return false;
        }

        // Aimed up is not enough if there's a roof/ceiling overhead - the flare
        // would just bump it and never reach the sky. Cast straight up and bail
        // if the shooter is under cover (indoors / under a roof).
        if (MiscGameplayFunctions.IsUnderRoofEx(player, GameConstants.ROOF_CHECK_RAYCAST_DIST, ObjIntersectFire))
        {
            Print("[FGAM] under a roof - flare blocked overhead, suppressed");
            return false;
        }

        Print("[FGAM] valid sky shot: angle=" + udAngle + " deg, overhead clear");
        return true;
    }

    private void FGAM_DegradeCondition()
    {
        float h = GetHealth01("", "Health");
        float next;
        if (h > 0.75)       next = 0.75;
        else if (h > 0.50)  next = 0.50;
        else if (h > 0.25)  next = 0.25;
        else if (h > 0.0)   next = 0.0;
        else                return;
        SetHealth("", "Health", GetMaxHealth("", "") * next);
        Print("[FGAM] FlareGun degraded, health now " + (next * 100) + "%");
    }
}
