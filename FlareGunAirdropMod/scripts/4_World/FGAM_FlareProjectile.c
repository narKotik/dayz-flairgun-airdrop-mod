// FGAM_FlareProjectile - tracks flare arc via a per-tick timer
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareProjectile.c

class FGAM_FlareTracker
{
    private string  m_Color;
    private vector  m_LastPos;
    private vector  m_PeakPos;
    private float   m_PeakY;
    private int     m_TicksStill;
    private bool    m_Done;
    private float   m_TimeAlive;

    static const float TICK_INTERVAL = 0.5;
    static const float MAX_LIFETIME  = 30.0;
    static const int   STILL_TICKS   = 4;

    void FGAM_FlareTracker(string color, vector startPos)
    {
        m_Color      = color;
        m_LastPos    = startPos;
        m_PeakPos    = startPos;
        m_PeakY      = startPos[1];
        m_TicksStill = 0;
        m_Done       = false;
        m_TimeAlive  = 0;

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, (int)(TICK_INTERVAL * 1000), false);
    }

    void Tick()
    {
        if (m_Done) return;

        m_TimeAlive = m_TimeAlive + TICK_INTERVAL;
        if (m_TimeAlive >= MAX_LIFETIME)
        {
            Print("[FGAM] FlareTracker timeout - firing event at peak");
            FireEvent();
            return;
        }

        array<Object> objects = new array<Object>;
        GetGame().GetObjectsAtPosition3D(m_LastPos, 60, objects, null);

        Object bestFlare;
        float  bestDist = 9999;

        foreach (Object obj : objects)
        {
            string cls = obj.GetType();
            cls.ToLower();
            if (cls.Contains("flare_projectile") || cls.Contains("flare_singleround"))
            {
                float d = vector.Distance(obj.GetPosition(), m_LastPos);
                if (d < bestDist)
                {
                    bestDist  = d;
                    bestFlare = obj;
                }
            }
        }

        if (!bestFlare)
        {
            m_TicksStill = m_TicksStill + 1;
        }
        else
        {
            vector pos = bestFlare.GetPosition();

            if (pos[1] > m_PeakY)
            {
                m_PeakY   = pos[1];
                m_PeakPos = pos;
            }

            float moved = vector.Distance(pos, m_LastPos);
            m_LastPos   = pos;

            if (moved < 0.5)
                m_TicksStill = m_TicksStill + 1;
            else
                m_TicksStill = 0;
        }

        if (m_TicksStill >= STILL_TICKS)
        {
            Print("[FGAM] FlareTracker detected landing - firing event");
            FireEvent();
            return;
        }

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(Tick, (int)(TICK_INTERVAL * 1000), false);
    }

    private void FireEvent()
    {
        m_Done = true;
        FGAM_AirdropManager.Get().OnFlareEvent(m_Color, m_PeakPos, m_LastPos);
    }
}