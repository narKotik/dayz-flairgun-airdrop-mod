// FGAM_Magazines - script-class bindings for the flare magazines.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Magazines.c
//
// These bind each FGAM flare magazine to an Ammunition_Base script class - exactly
// This is the pattern that lets custom flare ammo chamber without crashing. They are
// intentionally empty; colour handling + the airdrop trigger live in the flare's
// simulation (FGAM_FlareVisuals.c), not on the weapon or the magazine.

class FGAM_Mag_Red : Ammunition_Base {};
class FGAM_Mag_Green : Ammunition_Base {};
class FGAM_Mag_Blue : Ammunition_Base {};
class FGAM_Mag_Yellow : Ammunition_Base {};
class FGAM_Mag_White : Ammunition_Base {};
class FGAM_Mag_Orange : Ammunition_Base {};
class FGAM_Mag_Black : Ammunition_Base {};
