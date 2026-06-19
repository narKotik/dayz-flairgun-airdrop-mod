// FGAM_FlareGun - hooks into Weapon_Base (FlareGun has no vanilla script class)
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareGun.c

modded class Weapon_Base
{
    private int m_FGAM_ShotsInState = 0;

    override void OnFire(int muzzleIndex)
    {
        super.OnFire(muzzleIndex);

        if (!GetGame().IsServer()) return;
        if (GetType() != "FlareGun") return;

        string color = FGAM_GetCurrentColor();
        if (color != "")
        {
            FGAM_FlareTracker tracker = new FGAM_FlareTracker(color, GetPosition());
            Print("[FGAM] FlareTracker started for color: " + color);
        }

        m_FGAM_ShotsInState++;

        int shotsNeeded = FGAM_Config.Get().Flare.shotsPerState;
        if (shotsNeeded < 1) shotsNeeded = 1;

        if (m_FGAM_ShotsInState >= shotsNeeded)
        {
            m_FGAM_ShotsInState = 0;
            FGAM_DegradeCondition();
        }
    }

    override void EEInit()
    {
        super.EEInit();

        if (!GetGame().IsServer()) return;
        if (GetType() != "FlareGun") return;

        FGAM_SpawnContext ctx = FGAM_DetectSpawnContext(GetPosition());
        if (ctx == FGAM_SpawnContext.NONE) return;

        FGAM_Config cfg = FGAM_Config.Get();
        TStringIntMap weights;
        string color;

        switch (ctx)
        {
            case FGAM_SpawnContext.HELICRASH:
                color = FGAM_PickWeightedColor(cfg.Flare.helicrashFlareWeights);
                break;
            case FGAM_SpawnContext.TRAIN:
                color = FGAM_PickWeightedColor(cfg.Flare.trainFlareWeights);
                break;
            case FGAM_SpawnContext.BEACH:
                color = FGAM_PickWeightedColor(cfg.Flare.beachFlareWeights);
                SetHealth("", "Health", GetMaxHealth("", "Health") * 0.26);
                break;
            default:
                return;
        }

        string magClass = FGAM_MagClassFromColor(color);
        EntityAI magEnt = GetInventory().CreateInInventory(magClass);
        if (!magEnt)
            Print("[FGAM] Warning: could not load magazine " + magClass + " into FlareGun");
        else
            Print("[FGAM] FlareGun spawned with " + color + " magazine (ctx=" + ctx + ")");
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
        EItemCondition current = GetItemCondition();
        switch (current)
        {
            case EItemCondition.PRISTINE:
                SetHealth("", "Health", GetMaxHealth("", "Health") * 0.75);
                break;
            case EItemCondition.WORN:
                SetHealth("", "Health", GetMaxHealth("", "Health") * 0.50);
                break;
            case EItemCondition.DAMAGED:
                SetHealth("", "Health", GetMaxHealth("", "Health") * 0.25);
                break;
            case EItemCondition.BADLY_DAMAGED:
                SetHealth("", "Health", 0.0);
                break;
            case EItemCondition.RUINED:
                break;
        }
        Print("[FGAM] FlareGun degraded to condition: " + GetItemCondition());
    }

}
