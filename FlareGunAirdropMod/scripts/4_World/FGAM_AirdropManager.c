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

        // Called from the flare's simulation (FGAM_FlareVisuals.c) only after the
        // flare actually rose into the sky, so we only get here for valid shots.

        // Drop point: directly under the flare, at ground level.
        vector groundPos = peakPos;
        groundPos[1] = GetGame().SurfaceY(peakPos[0], peakPos[2]);

        Print("[FGAM] Flare event: " + color + " at " + groundPos);

        switch (color)
        {
            case "RED":
                ScheduleToxicZoneWithLoot(groundPos, cfg);
                break;

            case "GREEN":
            case "BLUE":
            case "WHITE":
            case "YELLOW":
            case "BLACK":
            case "ORANGE":
                SpawnAirdrop(groundPos, color, cfg, cfg.FlareSettings.airdropLifetime);
                break;

            default:
                Print("[FGAM] Unknown flare color: " + color);
        }
    }

    // Spawns a big-cargo crate high above groundPos and lets it descend gently.
    void SpawnAirdrop(vector groundPos, string color, FGAM_Config cfg, float lifetime)
    {
        TStringArray items = cfg.GetLootForColor(color);

        FGAM_FallingAirdrop drop = new FGAM_FallingAirdrop();
        drop.Start(
            groundPos,
            cfg.FlareSettings.airdropSpawnHeight,
            cfg.FlareSettings.airdropDescentSpeed,
            lifetime,
            cfg.FlareSettings.airdropContainerClass,
            color,
            items);
    }

    private void ScheduleToxicZoneWithLoot(vector groundPos, FGAM_Config cfg)
    {
        FGAM_ToxicZoneTimer timer = new FGAM_ToxicZoneTimer();
        timer.m_Position = groundPos;
        timer.m_Radius   = cfg.FlareSettings.redZoneRadius;
        timer.m_Duration = cfg.FlareSettings.redZoneDuration;
        timer.KeepAlive();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(timer.Activate, (int)(cfg.FlareSettings.redZoneDelay * 1000), false);
        Print("[FGAM] Toxic zone + loot scheduled in " + cfg.FlareSettings.redZoneDelay + "s at " + groundPos);
    }
}

// Shared loot loader: everything that fits goes into the container's cargo;
// anything that cannot fit drops on the ground beside it.
// NOTE: ECE_SETUP is required so spawned items get full entity setup — without it
// world-spawned items come up as a bare 'ScriptedType' and throw on InitItemSounds.
void FGAM_FillContainer(ItemBase container, TStringArray items)
{
    if (!container) return;
    foreach (string itemClass : items)
    {
        if (itemClass == "") continue;
        if (!container.GetInventory().CreateInInventory(itemClass))
        {
            vector nearby = container.GetPosition();
            nearby[0] = nearby[0] + Math.RandomFloat(-0.5, 0.5);
            nearby[2] = nearby[2] + Math.RandomFloat(-0.5, 0.5);
            GetGame().CreateObjectEx(itemClass, nearby, ECE_SETUP | ECE_PLACE_ON_SURFACE);
        }
    }
}

// Spawns a working, retail-safe toxic gas zone (FGAM_ToxicArea, a subclass of
// the vanilla chem-grenade ContaminatedArea_Local — see FGAM_ToxicArea.c). It
// self-initialises, syncs gas to clients, and self-deletes after its lifetime.
Object FGAM_SpawnGasZone(vector groundPos, float lifetime)
{
    Object obj = GetGame().CreateObjectEx("FGAM_ToxicArea", groundPos, ECE_PLACE_ON_SURFACE);
    FGAM_ToxicArea area = FGAM_ToxicArea.Cast(obj);
    if (!area)
    {
        Print("[FGAM] Failed to create FGAM_ToxicArea");
        return obj;
    }

    // ContaminatedArea_Local ticks this down once a second and deletes itself at 0.
    area.m_Lifetime = lifetime;
    FGAM_SpawnRegistry.Track("FGAM_ToxicArea", groundPos, 20.0);
    Print("[FGAM] Toxic gas zone spawned at " + groundPos + " (lifetime " + lifetime + "s)");
    return obj;
}

// ── Gentle parachute-style descent for the airdrop crate ─────────────────────
// Created without physics and moved by SetPosition on the server, so its descent
// replicates to every client. Loot is loaded before the fall so it rides inside
// the cargo. On landing a despawn timer is armed.
class FGAM_FallingAirdrop
{
    private static ref array<ref FGAM_FallingAirdrop> s_Active = new array<ref FGAM_FallingAirdrop>();

    Object m_Crate;
    vector m_GroundPos;
    float  m_CurrentY;
    float  m_DescentSpeed;
    float  m_Lifetime;
    string m_ContainerClass;

    static const int TICK_MS = 100;

    void Start(vector groundPos, float spawnHeight, float descentSpeed, float lifetime, string containerClass, string color, TStringArray items)
    {
        m_GroundPos    = groundPos;
        m_DescentSpeed  = descentSpeed;
        if (m_DescentSpeed <= 0) m_DescentSpeed = 6.0;
        m_Lifetime     = lifetime;
        m_ContainerClass = containerClass;

        m_CurrentY = groundPos[1] + spawnHeight;
        vector spawnPos = groundPos;
        spawnPos[1] = m_CurrentY;

        // ECE_SETUP   = full entity setup so the container's cargo works (without
        //               it CreateInInventory fails and items spawn malformed).
        // ECE_KEEPHEIGHT = keep the spawn altitude (no surface snap); no physics
        //               flag so it does not free-fall and we can animate it down.
        m_Crate = GetGame().CreateObjectEx(containerClass, spawnPos, ECE_SETUP | ECE_KEEPHEIGHT);
        if (!m_Crate)
        {
            Print("[FGAM] Failed to create airdrop container '" + containerClass + "'");
            return;
        }

        FGAM_FlareConfig fs = FGAM_Config.Get().FlareSettings;

        ItemBase crate = ItemBase.Cast(m_Crate);
        if (crate)
        {
            FGAM_FillContainer(crate, items);
            // Net-synced (RegisterNetSyncVariableBool) - set once here, server-side,
            // and it replicates to clients correctly. See FGAM_AirdropContainer.c.
            crate.SetTakeable(!fs.airdropDespawnEnabled);
        }

        // Tracked at the ground position (not the spawn/current height) with a
        // search radius wide enough to cover the whole fall, so a restart mid-
        // descent can still find and remove the crate later.
        // maxAgeDays: a despawn-enabled crate can't be moved (SetTakeable(false)
        // above), so if Sweep() ever finds it still sitting there, its normal
        // lifetime remover must have been interrupted by a restart - delete on
        // sight (-1). A despawn-disabled crate gets the configured grace period
        // instead, so a freshly-landed one isn't mistaken for abandoned litter.
        float maxAgeDays = -1;
        if (!fs.airdropDespawnEnabled)
            maxAgeDays = fs.airdropMaxAgeDays;
        FGAM_SpawnRegistry.Track(containerClass, groundPos, spawnHeight + 20.0, maxAgeDays);

        s_Active.Insert(this);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Descend, TICK_MS, true);
        Print("[FGAM] " + color + " airdrop spawned at " + spawnPos + " - descending");
    }

    void Descend()
    {
        if (!m_Crate)
        {
            Stop();
            return;
        }

        m_CurrentY = m_CurrentY - (m_DescentSpeed * (TICK_MS / 1000.0));

        if (m_CurrentY <= m_GroundPos[1])
        {
            vector land = m_GroundPos;
            m_Crate.SetPosition(land);

            ItemBase crate = ItemBase.Cast(m_Crate);
            if (crate)
                crate.PlaceOnSurface();

            Stop();

            if (FGAM_Config.Get().FlareSettings.airdropDespawnEnabled)
            {
                FGAM_AirdropRemover rem = new FGAM_AirdropRemover();
                rem.m_Crate = m_Crate;
                rem.m_ContainerClass = m_ContainerClass;
                rem.m_GroundPos = m_GroundPos;
                rem.KeepAlive();
                GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(rem.Remove, (int)(m_Lifetime * 1000), false);

                Print("[FGAM] airdrop landed at " + land + " - despawn in " + m_Lifetime + "s");
            }
            else
            {
                // Left in FGAM_SpawnRegistry with no remover scheduled: it survives
                // restarts. FGAM_SpawnRegistry.CleanupOnBoot only removes it later if
                // it's found untouched at this exact spot - if a player takes/moves
                // it first, it's treated as claimed and left alone.
                Print("[FGAM] airdrop landed at " + land + " - despawn disabled, crate is permanent and pickable");
            }
            return;
        }

        vector p = m_GroundPos;
        p[1] = m_CurrentY;
        m_Crate.SetPosition(p);
    }

    void Stop()
    {
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(Descend);
        s_Active.RemoveItem(this);
    }
}

class FGAM_AirdropRemover
{
    private static ref array<ref FGAM_AirdropRemover> s_Active = new array<ref FGAM_AirdropRemover>();

    Object m_Crate;
    string m_ContainerClass;
    vector m_GroundPos;

    void KeepAlive()
    {
        s_Active.Insert(this);
    }

    void Remove()
    {
        s_Active.RemoveItem(this);
        if (m_Crate)
        {
            FGAM_SpawnRegistry.Untrack(m_ContainerClass, m_GroundPos);
            GetGame().ObjectDelete(m_Crate);
            Print("[FGAM] airdrop crate recovered (despawned)");
        }
    }
}

// ── Red flare: delayed toxic zone + falling military crate inside it ──────────
class FGAM_ToxicZoneTimer
{
    private static ref array<ref FGAM_ToxicZoneTimer> s_Active = new array<ref FGAM_ToxicZoneTimer>();

    vector m_Position;
    float  m_Radius;
    float  m_Duration;

    void KeepAlive()
    {
        s_Active.Insert(this);
    }

    void Activate()
    {
        s_Active.RemoveItem(this);

        if (!GetGame().IsServer()) return;

        // The gas zone self-deletes after m_Duration; no separate remover needed.
        FGAM_SpawnGasZone(m_Position, m_Duration);

        // Drop the military crate into the gas; its lifetime matches the zone.
        FGAM_Config cfg = FGAM_Config.Get();
        FGAM_AirdropManager.Get().SpawnAirdrop(m_Position, "RED", cfg, m_Duration);

        Print("[FGAM] Toxic zone activated at " + m_Position + " - lasts " + m_Duration + "s");
    }
}
