// FGAM_Boxes - script-class bindings for the flare ammo boxes.
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Boxes.c
//
// Ground-spawnable boxes that unpack into a loaded flare magazine (see config.cpp,
// CfgVehicles > FGAM_Box_FlareBase, for why: the magazines themselves use vanilla's
// thin Flare_SingleRound.p3d model and can't be placed on the ground by CE).
// Box_Base (vanilla, 4_world/entities/core/inherited/inventoryitem.c) already wires
// up the native unpack action, so these bindings are intentionally empty - same
// pattern as vanilla's own 4_world/entities/itembase/ammoboxes.c.

class FGAM_Box_Red : Box_Base {};
class FGAM_Box_Green : Box_Base {};
class FGAM_Box_Blue : Box_Base {};
class FGAM_Box_White : Box_Base {};
class FGAM_Box_Yellow : Box_Base {};
class FGAM_Box_Black : Box_Base {};
class FGAM_Box_Orange : Box_Base {};
