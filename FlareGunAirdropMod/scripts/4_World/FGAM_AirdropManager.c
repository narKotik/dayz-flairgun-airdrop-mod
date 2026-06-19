// FGAM_AirdropManager - server-side handler for all flare events
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_AirdropManager.c

class FGAM_AirdropManager
{
    private static ref FGAM_AirdropManager s_instance;

    static FGAM_AirdropManager Get()
    {
        if (!s_instance)
            s_instance = new FGAM_AirdropManager();
        return s_instance;
    }

    // Entry point called from FGAM_FlareTracker on landing
    // color     : "RED" / "GREEN" / "BLUE" / "WHITE" / "YELLOW" / "BLACK" / "ORANGE"
    // peakPos   : highest point of the flare arc
    // impactPos : where the flare landed
    void OnFlareEvent(string color, vector peakPos, vector impactPos)
    {
        if (!GetGame().IsServer()) return;

        FGAM_Config cfg = FGAM_Config.Get();

        // Height check - peak must be at least minTriggerAltitude above terrain
        float terrainH = GetGame().SurfaceY(peakPos[0], peakPos[2]);
        float altitude = peakPos[1] - terrainH;

        if (altitude < cfg.Flare.minTriggerAltitude)
        {
            Print("[FGAM] Flare too low (" + altitude + "m) - event suppressed");
            return;
        }

        Print("[FGAM] Flare event: " + color + " at " + peakPos + " (alt=" + altitude + "m)");

        switch (color)
        {
            case "RED":
                ScheduleToxicZoneWithLoot(peakPos, cfg);
                break;

            case "GREEN":
            case "BLUE":
            case "WHITE":
            case "YELLOW":
            case "BLACK":
            case "ORANGE":
                SpawnAirdropCrate(peakPos, color, cfg);
                break;

            default:
                Print("[FGAM] Unknown flare color: " + color);
        }
    }

    // Spawn airdrop crate above impact position
    private void SpawnAirdropCrate(vector pos, string color, FGAM_Config cfg)
    {
        vector spawnPos = pos;
        spawnPos[1] = pos[1] + cfg.Flare.airdropSpawnHeight;

        Object obj = GetGame().CreateObjectEx(cfg.Flare.airdropContainerClass, spawnPos, ECE_CREATEPHYSICS);

        if (!obj)
        {
            Print("[FGAM] Failed to create airdrop container for " + color);
            return;
        }

        obj.SetPosition(spawnPos);
        obj.PlaceOnSurface();

        ItemBase crate = ItemBase.Cast(obj);
        if (crate)
            FillCrate(crate, color, cfg);

        Print("[FGAM] Airdrop crate spawned for " + color + " at " + spawnPos);
    }

    // Fill crate with loot from config
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
                vector nearby = crate.GetPosition();
                nearby[0] += Math.RandomFloat(-0.5, 0.5);
                nearby[2] += Math.RandomFloat(-0.5, 0.5);
                GetGame().CreateObjectEx(itemClass, nearby, ECE_CREATEPHYSICS);
            }
        }
    }

    // RED - schedule toxic zone + loot after delay
    private void ScheduleToxicZoneWithLoot(vector pos, FGAM_Config cfg)
    {
        FGAM_ToxicZoneTimer timer = new FGAM_ToxicZoneTimer();
        timer.m_Position       = pos;
        timer.m_Radius         = cfg.Flare.redZoneRadius;
        timer.m_Duration       = cfg.Flare.redZoneDuration;
        timer.m_SpawnHeight    = cfg.Flare.airdropSpawnHeight;
        timer.m_ContainerClass = cfg.Flare.airdropContainerClass;
        timer.m_LootColor      = "RED";
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(timer, "Activate", (int)(cfg.Flare.redZoneDelay * 1000), false);
        Print("[FGAM] Toxic zone + loot scheduled in " + cfg.Flare.redZoneDelay + "s at " + pos);
    }
}

// Spawns toxic zone AND loot crate together, removes both on expiry
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

        ContaminatedArea_Dynamic zone = ContaminatedArea_Dynamic.Cast(
            GetGame().CreateObjectEx("ContaminatedArea_Dynamic", m_Position, ECE_CREATEPHYSICS)
        );

        if (!zone)
        {
            Print("[FGAM] Failed to create ContaminatedArea_Dynamic");
            return;
        }

        zone.SetPosition(m_Position);
        zone.SetInnerRadius(m_Radius * 0.6);
        zone.SetOuterRadius(m_Radius);

        vector spawnPos = m_Position;
        spawnPos[1] = m_Position[1] + m_SpawnHeight;
        Object crateObj = GetGame().CreateObjectEx(m_ContainerClass, spawnPos, ECE_CREATEPHYSICS);

        ItemBase crate;
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

        FGAM_ToxicZoneRemover remover = new FGAM_ToxicZoneRemover();
        remover.m_Zone  = zone;
        remover.m_Crate = crate;
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(remover, "Remove", (int)(m_Duration * 1000), false);

        Print("[FGAM] Toxic zone activated at " + m_Position + " - lasts " + m_Duration + "s");
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
            GetGame().ObjectDelete(m_Crate);
            Print("[FGAM] Red zone loot crate removed");
        }
    }
}
