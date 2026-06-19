// ──────────────────────────────────────────────────────────────────────────────
//  FGAM_FlareGun — modded FlareGun: block repair, degrade on each shot
//  Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareGun.c
// ──────────────────────────────────────────────────────────────────────────────

modded class FlareGun
{
    // Track how many shots have been fired in the current health state
    private int m_FGAM_ShotsInState = 0;

    // ── Block all repair actions ──────────────────────────────────────────────
    override bool CanBeRepaired(EntityAI item, float healthLevel, int damage)
    {
        return false;
    }

    // ── Called after each shot ────────────────────────────────────────────────
    override void OnFire(int muzzleIndex)
    {
        super.OnFire(muzzleIndex);

        if (!GetGame().IsServer()) return;

        // Start tracking this flare's arc and landing position
        string color = FGAM_GetCurrentColor();
        if (color != "")
        {
            vector startPos = GetPosition();
            FGAM_FlareTracker tracker = new FGAM_FlareTracker(color, startPos);
            Print("[FGAM] FlareTracker started for color: " + color);
        }

        m_FGAM_ShotsInState++;

        int shotsNeeded = FGAM_Config.Get().Flare.shotsPerState;
        if (shotsNeeded < 1) shotsNeeded = 1;

        if (m_FGAM_ShotsInState >= shotsNeeded)
        {
            m_FGAM_ShotsInState = 0;
            DegradeCondition();
        }
    }

    // ── Map current magazine class → color string ─────────────────────────────
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

    // ── Drop one condition tier ───────────────────────────────────────────────
    private void DegradeCondition()
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
                // Next shot will destroy it
                SetHealth("", "Health", 0.0);
                break;
            case EItemCondition.RUINED:
                // Already ruined — nothing more to do
                break;
        }

        Print("[FGAM] FlareGun degraded to condition: " + GetItemCondition());
    }

    // ── Called after CE places the gun in the world ──────────────────────────
    override void EEInit()
    {
        super.EEInit();

        if (!GetGame().IsServer()) return;

        FGAM_SpawnContext ctx = FGAM_DetectSpawnContext(GetPosition());
        if (ctx == FGAM_SpawnContext.NONE) return;

        FGAM_Config cfg = FGAM_Config.Get();
        string color;
        TStringIntMap weights;

        switch (ctx)
        {
            case FGAM_SpawnContext.HELICRASH:
                weights = cfg.Flare.helicrashFlareWeights;
                color   = FGAM_PickWeightedColor(weights);
                break;

            case FGAM_SpawnContext.TRAIN:
                weights = cfg.Flare.trainFlareWeights;
                color   = FGAM_PickWeightedColor(weights);
                break;

            case FGAM_SpawnContext.BEACH:
                weights = cfg.Flare.beachFlareWeights;
                color   = FGAM_PickWeightedColor(weights);
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

    // ── Serialise shot counter so it survives server restarts ─────────────────
    override void OnStoreSave(ParamsWriteContext ctx)
    {
        super.OnStoreSave(ctx);
        ctx.Write(m_FGAM_ShotsInState);
    }

    override bool OnStoreLoad(ParamsReadContext ctx, int version)
    {
        if (!super.OnStoreLoad(ctx, version)) return false;
        if (!ctx.Read(m_FGAM_ShotsInState))  return false;
        return true;
    }
}
