// FGAM_FlareProjectile - fires airdrop event after fixed delay
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareProjectile.c
//
// s_Active keeps a ref so the object survives until FireEvent() runs.
// Without it the object is GC'd immediately (local var goes out of scope)
// and CallLater callback never fires.

class FGAM_FlareTracker
{
    private static ref array<ref FGAM_FlareTracker> s_Active = new array<ref FGAM_FlareTracker>();

    private string m_Color;
    private vector m_OriginPos; // ground position under the shooter at fire time

    static const int FIRE_DELAY_MS = 12000;

    // The upward-pitch gate now happens in Weapon_Base.OnFire (FGAM_FiredUpward),
    // so by the time a tracker exists the shot already qualified. We just remember
    // where it was fired from and dispatch the event after a short delay.
    void FGAM_FlareTracker(string color, vector startPos)
    {
        m_Color     = color;
        m_OriginPos = startPos;

        s_Active.Insert(this);

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FireEvent, FIRE_DELAY_MS, false);
        Print("[FGAM] FlareTracker will fire in " + (FIRE_DELAY_MS / 1000) + "s, origin=" + m_OriginPos);
    }

    private void FireEvent()
    {
        Print("[FGAM] FlareTracker firing event: color=" + m_Color + " origin=" + m_OriginPos);
        FGAM_AirdropManager.Get().OnFlareEvent(m_Color, m_OriginPos, m_OriginPos);
        s_Active.RemoveItem(this);
    }
}
