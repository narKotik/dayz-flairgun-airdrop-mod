// FGAM_AirdropContainer - the big-cargo airdrop crate.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_AirdropContainer.c
//
// Behaviour class for the container defined in config.cpp (: SeaChest). Players
// can always open it and loot its cargo. Whether the crate itself can be picked
// up/carried away depends on flare.airdropDespawnEnabled in config.json: when
// despawn is on, the crate is temporary, so FGAM_AirdropManager calls
// SetTakeable(false) on it at spawn time (matches the old hardcoded behaviour);
// when despawn is off, the crate is permanent and left takeable - otherwise it
// would clutter the map forever with no way to relocate it.
//
// IsTakeable() is intentionally NOT overridden here: ItemBase.m_IsTakeable is a
// net-synced field (RegisterNetSyncVariableBool), so setting it server-side via
// SetTakeable() replicates correctly to clients. Deriving it locally from
// FGAM_Config here instead would break on clients, which never have
// config.json (only the server profile does).

class FGAM_AirdropContainer : SeaChest
{
    override bool CanPutIntoHands(EntityAI parent)
    {
        return IsTakeable();
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return IsTakeable();
    }
}
