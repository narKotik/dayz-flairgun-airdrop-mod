// ──────────────────────────────────────────────────────────────────────────────
//  FGAM_FlareProjectile — tracks flare arc via a per-tick timer started from
//  FlareGun.OnFire(). FlareSimulation is an engine type string, not a script
//  class, so we cannot modded it. Instead FGAM_FlareTracker is a timer object
//  spawned on the server that polls the nearest flare entity position until it
//  stops moving, then fires the airdrop event.
//
//  Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareProjectile.c
// ──────────────────────────────────────────────────────────────────────────────

class FGAM_FlareTracker
{
    private string  m_Color;
    private vector  m_LastPos;
    private vector  m_PeakPos;
    private float   m_PeakY;
    private int     m_TicksStill;   // consecutive ticks with no movement
    private bool    m_Done;
    private float   m_TimeAlive;

    static const float TICK_INTERVAL  = 0.5;   // seconds between polls
    static const float MAX_LIFETIME   = 30.0;  // give up after 30s
    static const int   STILL_TICKS    = 4;     // 4 ticks still = landed

    void FGAM_FlareTracker(string color, vector startPos)
    {
        m_Color      = color;
        m_LastPos    = startPos;
        m_PeakPos    = startPos;
        m_PeakY      = startPos[1];
        m_TicksStill = 0;
        m_Done       = false;
        m_TimeAlive  = 0;

        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this, "Tick", TICK_INTERVAL * 1000, false);
    }

    void Tick()
    {
        if (m_Done) return;

        m_TimeAlive += TICK_INTERVAL;
        if (m_TimeAlive >= MAX_LIFETIME)
        {
            Print("[FGAM] FlareTracker timeout — firing event at peak");
            FireEvent();
            return;
        }

        // Find the nearest flare entity by searching around the last known position
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
            // Can't find it — it may have landed and been cleaned up
            m_TicksStill++;
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
                m_TicksStill++;
            else
                m_TicksStill = 0;
        }

        if (m_TicksStill >= STILL_TICKS)
        {
            Print("[FGAM] FlareTracker detected landing — firing event");
            FireEvent();
            return;
        }

        // Schedule next tick
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(this, "Tick", TICK_INTERVAL * 1000, false);
    }

    private void FireEvent()
    {
        m_Done = true;
        FGAM_AirdropManager.Get().OnFlareEvent(m_Color, m_PeakPos, m_LastPos);
    }
}
