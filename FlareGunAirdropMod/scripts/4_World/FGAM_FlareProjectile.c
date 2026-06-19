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
    private vector m_PeakPos;

    static const int   FIRE_DELAY_MS   = 12000;
    static const float PEAK_HEIGHT_EST = 80.0;

    void FGAM_FlareTracker(string color, vector startPos)
    {
        m_Color      = color;
        m_PeakPos    = startPos;
        m_PeakPos[1] = startPos[1] + PEAK_HEIGHT_EST;

        s_Active.Insert(this);

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FireEvent, FIRE_DELAY_MS, false);
        Print("[FGAM] FlareTracker will fire in " + (FIRE_DELAY_MS / 1000) + "s, estimated peak=" + m_PeakPos);
    }

    private void FireEvent()
    {
        Print("[FGAM] FlareTracker firing event: color=" + m_Color + " peak=" + m_PeakPos);
        FGAM_AirdropManager.Get().OnFlareEvent(m_Color, m_PeakPos, m_PeakPos);
        s_Active.RemoveItem(this);
    }
}
