// ──────────────────────────────────────────────────────────────────────────────
//  FGAM_AirdropManager — server-side, handles all events triggered by flares
//  Path: FlareGunAirdropMod/scripts/4_World/FGAM_AirdropManager.c
// ──────────────────────────────────────────────────────────────────────────────

class FGAM_AirdropManager
{
    private static ref FGAM_AirdropManager s_instance;

    static FGAM_AirdropManager Get()
    {
        if (!s_instance)
            s_instance = new FGAM_AirdropManager();
        return s_instance;
    }

    // ── Entry point called from flare projectile ──────────────────────────────
    //  color      : "RED" / "GREEN" / "BLUE" / "WHITE" / "YELLOW" / "BLACK" / "ORANGE"
    //  impactPos  : world position where flare landed / peaked
    //  shooterPos : position of the player who fired
    void OnFlareEvent(string color, vector impactPos, vector shooterPos)
    {
        if (!GetGame().IsServer()) return;

        FGAM_Config cfg = FGAM_Config.Get();

        // ── Height check ──────────────────────────────────────────────────────
        float terrainH = GetGame().SurfaceY(impactPos[0], impactPos[2]);
        float altitude = impactPos[1] - terrainH;

        if (altitude < cfg.Flare.minTriggerAltitude)
        {
            Print("[FGAM] Flare too low (" + altitude + "m) — event suppressed");
            return;
        }

        Print("[FGAM] Flare event: " + color + " at " + impactPos + " (alt=" + altitude + "m)");

        // ── Dispatch ──────────────────────────────────────────────────────────
        switch (color)
        {
            case "RED":
                // Crate spawns with the zone after delay, not immediately
                ScheduleToxicZoneWithLoot(impactPos, cfg);
                break;

            case "GREEN":
            case "BLUE":
            case "WHITE":
            case "YELLOW":
            case "BLACK":
            case "ORANGE":
                SpawnAirdropCrate(impactPos, color, cfg);
                break;

            default:
                Print("[FGAM] Unknown flare color: " + color);
        }
    }

    // ── AIRDROP CRATE ─────────────────────────────────────────────────────────
    private void SpawnAirdropCrate(vector impactPos, string color, FGAM_Config cfg)
    {
        // Spawn container above impact, let physics carry it down
        vector spawnPos = impactPos;
        spawnPos[1] = impactPos[1] + cfg.Flare.airdropSpawnHeight;

        Object obj = GetGame().CreateObjectEx(
            cfg.Flare.airdropContainerClass,
            spawnPos,
            ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH
        );

        if (!obj)
        {
            Print("[FGAM] Failed to create airdrop container for " + color);
            return;
        }

        // Enable physics / gravity
        obj.SetPosition(spawnPos);
        obj.PlaceOnSurface();   // overridden by physics immediately

        ItemBase crate = ItemBase.Cast(obj);
        if (crate)
            FillCrate(crate, color, cfg);

        Print("[FGAM] Airdrop crate spawned for " + color + " at " + spawnPos);
    }

    // ── Fill crate with loot from config ──────────────────────────────────────
    private void FillCrate(ItemBase crate, string color, FGAM_Config cfg)
    {
        TStringArray items = cfg.GetLootForColor(color);
        EntityAI crateEntity = EntityAI.Cast(crate);

        foreach (string itemClass : items)
        {
            if (itemClass == "") continue;
            EntityAI item = crateEntity.GetInventory().CreateInInventory(itemClass);
            if (!item)
            {
                // Try spawning nearby if no space
                vector nearby = crate.GetPosition();
                nearby[0] += Math.RandomFloat(-0.5, 0.5);
                nearby[2] += Math.RandomFloat(-0.5, 0.5);
                GetGame().CreateObjectEx(itemClass, nearby, ECE_CREATEPHYSICS);
            }
        }
    }

    // ── RED — schedule toxic zone + loot drop together after delay ───────────
    private void ScheduleToxicZoneWithLoot(vector pos, FGAM_Config cfg)
    {
        FGAM_ToxicZoneTimer timer = new FGAM_ToxicZoneTimer();
        timer.m_Position      = pos;
        timer.m_Radius        = cfg.Flare.redZoneRadius;
        timer.m_Duration      = cfg.Flare.redZoneDuration;
        timer.m_SpawnHeight   = cfg.Flare.airdropSpawnHeight;
        timer.m_ContainerClass = cfg.Flare.airdropContainerClass;
        timer.m_LootColor     = "RED";
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(
            timer,
            "Activate",
            (int)(cfg.Flare.redZoneDelay * 1000),
            false
        );
        Print("[FGAM] Toxic zone + loot scheduled in " + cfg.Flare.redZoneDelay + "s at " + pos);
    }
}

// ──────────────────────────────────────────────────────────────────────────────
//  Timer — spawns toxic zone AND loot crate together, removes both on expiry
// ──────────────────────────────────────────────────────────────────────────────
class FGAM_ToxicZoneTimer
{
    vector m_Position;
    float  m_Radius;
    float  m_Duration;
    float  m_SpawnHeight;
    string m_ContainerClass;
    string m_LootColor;

    void Activate()
    {
        if (!GetGame().IsServer()) return;

        // ── Spawn toxic zone ─────────────────────────────────────────────────
        ContaminatedArea_Dynamic zone = ContaminatedArea_Dynamic.Cast(
            GetGame().CreateObjectEx(
                "ContaminatedArea_Dynamic",
                m_Position,
                ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH
            )
        );

        if (!zone)
        {
            Print("[FGAM] Failed to create ContaminatedArea_Dynamic");
            return;
        }

        zone.SetPosition(m_Position);
        zone.SetInnerRadius(m_Radius * 0.6);
        zone.SetOuterRadius(m_Radius);

        // ── Spawn loot crate above zone centre ───────────────────────────────
        ItemBase crate;
        vector spawnPos    = m_Position;
        spawnPos[1]        = m_Position[1] + m_SpawnHeight;
        Object crateObj    = GetGame().CreateObjectEx(
            m_ContainerClass,
            spawnPos,
            ECE_CREATEPHYSICS | ECE_UPDATEPATHGRAPH
        );

        if (crateObj)
        {
            crate = ItemBase.Cast(crateObj);
            if (crate)
            {
                FGAM_Config cfg = FGAM_Config.Get();
                TStringArray items = cfg.GetLootForColor(m_LootColor);
                EntityAI crateEntity = EntityAI.Cast(crate);
                foreach (string itemClass : items)
                {
                    if (itemClass == "") continue;
                    if (!crateEntity.GetInventory().CreateInInventory(itemClass))
                    {
                        vector nearby = crate.GetPosition();
                        nearby[0] += Math.RandomFloat(-0.5, 0.5);
                        nearby[2] += Math.RandomFloat(-0.5, 0.5);
                        GetGame().CreateObjectEx(itemClass, nearby, ECE_CREATEPHYSICS);
                    }
                }
            }
            Print("[FGAM] Red loot crate spawned inside zone at " + spawnPos);
        }
        else
        {
            Print("[FGAM] Warning: failed to spawn loot crate for RED zone");
        }

        // ── Schedule zone + loot removal ────────────────────────────────────
        FGAM_ToxicZoneRemover remover = new FGAM_ToxicZoneRemover();
        remover.m_Zone  = zone;
        remover.m_Crate = crate;
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(
            remover,
            "Remove",
            (int)(m_Duration * 1000),
            false
        );

        Print("[FGAM] Toxic zone activated at " + m_Position
            + " — lasts " + m_Duration + "s, loot will be removed with zone");
    }
}

class FGAM_ToxicZoneRemover
{
    ContaminatedArea_Dynamic m_Zone;
    ItemBase                 m_Crate;

    void Remove()
    {
        if (m_Zone)
        {
            GetGame().ObjectDelete(m_Zone);
            Print("[FGAM] Toxic zone removed");
        }

        if (m_Crate)
        {
            // Delete crate and everything in it
            GetGame().ObjectDelete(m_Crate);
            Print("[FGAM] Red zone loot crate removed");
        }
    }
}
