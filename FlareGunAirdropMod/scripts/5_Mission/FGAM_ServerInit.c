// FGAM_ServerInit - server-side boot hook
// Path: FlareGunAirdropMod/scripts/5_Mission/FGAM_ServerInit.c
//
// Removes any airdrop crate / toxic gas zone left over from before the last
// restart, then keeps sweeping periodically so long server uptimes (no
// restart for weeks) still expire aged-out permanent crates without needing
// a reboot. See FGAM_SpawnRegistry.c for why any of this is needed, and why
// the first sweep is delayed rather than run immediately here.

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        FGAM_SpawnRegistryMaintenance.Start(30 * 1000, 6 * 60 * 60 * 1000); // first sweep after 30s, then every 6h
    }
}
