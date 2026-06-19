// FGAM_FlareGun - hooks into Weapon_Base (FlareGun has no vanilla script class)
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareGun.c

modded class Weapon_Base
{
    private int m_FGAM_ShotsInState = 0;

    override void OnFire(int muzzle_index)
    {
        super.OnFire(muzzle_index);

        if (!GetGame().IsServer()) return;
        if (GetType() != "FlareGun") return;

        string color = FGAM_GetCurrentColor();
        if (color != "")
        {
            FGAM_FlareTracker tracker = new FGAM_FlareTracker(color, GetPosition());
            Print("[FGAM] FlareTracker started for color: " + color);
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

    private string FGAM_GetCurrentColor()
    {
        Magazine mag = GetMagazine(GetCurrentMuzzle());
        if (!mag) return "";
        string cls = mag.GetType();
        cls.ToLower();
        if (cls.Contains("_red"))    return "RED";
        if (cls.Contains("_green"))  return "GREEN";
        if (cls.Contains("_blue"))   return "BLUE";
        if (cls.Contains("_white"))  return "WHITE";
        if (cls.Contains("_yellow")) return "YELLOW";
        if (cls.Contains("_black"))  return "BLACK";
        if (cls.Contains("_orange")) return "ORANGE";
        return "";
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