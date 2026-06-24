// FGAM_ToxicArea - a retail-safe toxic gas zone.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_ToxicArea.c
//
// Why this exists:
//   - ContaminatedArea_Static fills particles only in DeferredInit and has no
//     OnVariablesSynchronized, so a runtime spawn is invisible to clients.
//   - GetCEApi().SpawnGroup("ContaminatedArea_Dynamic") only works in the Diag
//     executable ("Not available in this build" on a retail server).
//
// ContaminatedArea_Local is what the vanilla chem-gas grenade spawns, so it is
// proven to work in retail: it self-initialises, re-syncs particles to clients
// via ContaminatedArea_DynamicBase.OnVariablesSynchronized, and self-deletes via
// its lifetime tick. The only drawback is its tiny 10m puff — so we subclass it
// and override SetupZoneData with a bigger radius and the full gas cloud.
//
// EffectArea members are public, so we set them directly and call InitZone()
// ourselves instead of super.SetupZoneData() (which would force Local's small
// puff defaults back).

class FGAM_ToxicArea : ContaminatedArea_Local
{
    override void SetupZoneData(EffectAreaParams params)
    {
        // Base "ContaminatedTrigger" (not the silent "_Local" one) so players get
        // the vanilla contaminated-area ambient gas hiss while inside the cloud.
        m_TriggerType      = "ContaminatedTrigger";
        m_Radius           = 40;
        m_PositiveHeight   = 12;
        m_NegativeHeight   = 6;
        m_InnerRings       = 2;
        m_InnerSpacing     = 22;
        m_OuterRingToggle  = true;
        m_OuterSpacing     = 18;
        m_OuterRingOffset  = 0;
        m_ParticleID       = ParticleList.CONTAMINATED_AREA_GAS_BIGASS;
        m_AroundParticleID = ParticleList.CONTAMINATED_AREA_GAS_AROUND;
        m_TinyParticleID   = ParticleList.CONTAMINATED_AREA_GAS_TINY;
        m_PPERequesterType = "PPERequester_ContaminatedAreaTint";
        m_PPERequesterIdx  = GetRequesterIndex(m_PPERequesterType);

        SetSynchDirty();
        InitZone();
    }

    // ContaminatedArea_Local runs this once a second on the server. The inherited
    // trigger already links players to the vanilla contamination modifier, but its
    // effect is gradual and easy to miss, so we ALSO transmit airborne chemical
    // agents directly to unprotected players in the zone. This uses the exact same
    // pathway/agent (AGT_AIRBOURNE_CHEMICAL / CHEMICAL_POISON) the vanilla area
    // modifier uses, so gas masks + NBC gear still protect the player.
    override void Tick()
    {
        m_Lifetime -= TICK_RATE;
        if (m_Lifetime <= 0)
        {
            Delete();
            return;
        }

        if (!GetGame().IsServer())
            return;

        PluginTransmissionAgents plugin = PluginTransmissionAgents.Cast(GetPlugin(PluginTransmissionAgents));
        if (!plugin)
            return;

        float radiusSq = m_Radius * m_Radius;
        vector center  = GetPosition();

        array<Man> players = new array<Man>();
        GetGame().GetPlayers(players);
        foreach (Man man : players)
        {
            PlayerBase player = PlayerBase.Cast(man);
            if (!player || !player.IsAlive())
                continue;
            if (vector.DistanceSq(player.GetPosition(), center) > radiusSq)
                continue;

            plugin.TransmitAgentsEx(null, player, AGT_AIRBOURNE_CHEMICAL, 50, eAgents.CHEMICAL_POISON);
        }
    }
}
