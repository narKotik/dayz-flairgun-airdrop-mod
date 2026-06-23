// FGAM_AirdropContainer - the big-cargo airdrop crate.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_AirdropContainer.c
//
// Behaviour class for the container defined in config.cpp (: SeaChest). It is
// deliberately NOT takeable: players can open it and loot its cargo, but cannot
// pick the whole crate up and carry it away.

class FGAM_AirdropContainer : SeaChest
{
    override bool IsTakeable()
    {
        return false;
    }

    override bool CanPutIntoHands(EntityAI parent)
    {
        return false;
    }

    override bool CanPutInCargo(EntityAI parent)
    {
        return false;
    }
}
