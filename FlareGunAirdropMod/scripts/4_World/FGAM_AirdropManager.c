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

    void OnFlareEvent(string color, vector peakPos, vector impactPos)
    {
        if (!GetGame().IsServer()) return;

        FGAM_Config cfg = FGAM_Config.Get();

        float terrainH = GetGame().SurfaceY(peakPos[0], peakPos[2]);
        float altitude = peakPos[1] - terrainH;

        if (altitude < cfg.FlareSettings.minTriggerAltitude)
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

    private void SpawnAirdropCrate(vector pos, string color, FGAM_Config cfg)
    {
        vector spawnPos = pos;
        spawnPos[1] = pos[1] + cfg.FlareSettings.airdropSpawnHeight;

        Object obj = GetGame().CreateObjectEx(cfg.FlareSettings.airdropContainerClass, spawnPos, ECE_CREATEPHYSICS);

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

    private void FillCrate(ItemBase crate, string color, FGAM_Config cfg)
    {
        TStringArray items = cfg.GetLootForColor(color);

        foreach (string itemClass : items)
        {
            if (itemClass == "") continue;
            if (!crate.GetInventory().CreateInInventory(itemClass))
            {
                vector nearby = crate.GetPosition();
                nearby[0] = nearby[0] + Math.RandomFloat(-0.5, 0.5);
                nearby[2] = nearby[2] + Math.RandomFloat(-0.5, 0.5);
                GetGame().CreateObjectEx(itemClass, nearby, ECE_CREATEPHYSICS);
            }
        }
    }

    private void ScheduleToxicZoneWithLoot(vector pos, FGAM_Config cfg)
    {
        FGAM_ToxicZoneTimer timer = new FGAM_ToxicZoneTimer();
        timer.m_Position       = pos;
        timer.m_Radius         = cfg.FlareSettings.redZoneRadius;
        timer.m_Duration       = cfg.FlareSettings.redZoneDuration;
        timer.m_SpawnHeight    = cfg.FlareSettings.airdropSpawnHeight;
        timer.m_ContainerClass = cfg.FlareSettings.airdropContainerClass;
        timer.m_LootColor      = "RED";
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(timer.Activate, (int)(cfg.FlareSettings.redZoneDelay * 1000), false);
        Print("[FGAM] Toxic zone + loot scheduled in " + cfg.FlareSettings.redZoneDelay + "s at " + pos);
    }
}

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

        Object zoneObj = GetGame().CreateObjectEx("ContaminatedArea_Dynamic", m_Position, ECE_CREATEPHYSICS);

        if (!zoneObj)
        {
            Print("[FGAM] Failed to create ContaminatedArea_Dynamic");
            return;
        }

        zoneObj.SetPosition(m_Position);

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
                foreach (string itemClass : items)
                {
                    if (itemClass == "") continue;
                    if (!crate.GetInventory().CreateInInventory(itemClass))
                    {
                        vector nearby = crate.GetPosition();
                        nearby[0] = nearby[0] + Math.RandomFloat(-0.5, 0.5);
                        nearby[2] = nearby[2] + Math.RandomFloat(-0.5, 0.5);
                        GetGame().CreateObjectEx(itemClass, nearby, ECE_CREATEPHYSICS);
                    }
                }
            }
            Print("[FGAM] Red loot crate spawned inside zone at " + spawnPos);
        }

        FGAM_ToxicZoneRemover remover = new FGAM_ToxicZoneRemover();
        remover.m_Zone  = zoneObj;
        remover.m_Crate = crate;
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(remover.Remove, (int)(m_Duration * 1000), false);

        Print("[FGAM] Toxic zone activated at " + m_Position + " - lasts " + m_Duration + "s");
    }
}

class FGAM_ToxicZoneRemover
{
    Object   m_Zone;
    ItemBase m_Crate;

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