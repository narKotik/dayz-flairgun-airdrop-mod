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
            FGAM_FlareTracker tracker = new FGAM_FlareTracker(color, GetPosition());
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
