// FGAM_Magazines
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Magazines.c
//
// Color detection: when a chamberableFrom mag is chambered, the engine briefly moves it
// to an ATTACHMENT slot (InventoryLocation type 2) on the weapon. We set ColorTracker
// at that moment (newLoc.GetType() == 2) so OnFire can read the correct color.

class FGAM_ColorTracker
{
    private static string s_Color = "";

    static void Set(string color) { s_Color = color; Print("[FGAM] ColorTracker set: " + color); }
    static string Consume() { string c = s_Color; s_Color = ""; return c; }
}

class FGAM_Mag_Red : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] RED LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("RED");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_Orange : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] ORANGE LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("ORANGE");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_Green : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] GREEN LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("GREEN");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_Blue : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] BLUE LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("BLUE");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_Black : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] BLACK LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("BLACK");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_White : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] WHITE LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("WHITE");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}

class FGAM_Mag_Yellow : Ammo_Flare
{
    override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
    {
        Print("[FGAM] YELLOW LocationChanged old=" + oldLoc.GetType() + " new=" + newLoc.GetType() + " srv=" + GetGame().IsServer());
        if (GetGame().IsServer() && newLoc.GetType() == 2) FGAM_ColorTracker.Set("YELLOW");
        super.EEItemLocationChanged(oldLoc, newLoc);
    }
}
