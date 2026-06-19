// ──────────────────────────────────────────────────────────────────────────────
//  FGAM_SpawnHandler — helper functions for spawn context detection
//  Path: FlareGunAirdropMod/scripts/4_World/FGAM_SpawnHandler.c
// ──────────────────────────────────────────────────────────────────────────────

// ── Context enum — declared first so FlareGun.c can use it ───────────────────
enum FGAM_SpawnContext
{
    NONE      = 0,
    HELICRASH = 1,
    TRAIN     = 2,
    BEACH     = 3,
}

// ── Weighted random color picker ──────────────────────────────────────────────
string FGAM_PickWeightedColor(TStringIntMap weights)
{
    int total = 0;
    foreach (string color, int w : weights)
        total += w;

    if (total == 0) return "GREEN";

    int roll = Math.RandomInt(0, total);
    int cumulative = 0;
    foreach (string c, int wt : weights)
    {
        cumulative += wt;
        if (roll < cumulative)
            return c;
    }
    return "GREEN";
}

// ── Magazine class name from color ────────────────────────────────────────────
string FGAM_MagClassFromColor(string color)
{
    switch (color)
    {
        case "RED":    return "FGAM_Mag_Red";
        case "GREEN":  return "FGAM_Mag_Green";
        case "BLUE":   return "FGAM_Mag_Blue";
        case "WHITE":  return "FGAM_Mag_White";
        case "YELLOW": return "FGAM_Mag_Yellow";
        case "BLACK":  return "FGAM_Mag_Black";
        case "ORANGE": return "FGAM_Mag_Orange";
    }
    return "FGAM_Mag_Green";
}

// ── Detect context by proximity to known world features ──────────────────────
FGAM_SpawnContext FGAM_DetectSpawnContext(vector pos)
{
    // Helicrash: look for crash debris in 30m radius
    array<Object> nearby = new array<Object>();
    GetGame().GetObjectsAtPosition(pos, 30, nearby, null);
    foreach (Object obj : nearby)
    {
        string cn = obj.GetType();
        cn.ToLower();
        if (cn.Contains("helicrash") || cn.Contains("mi8") || cn.Contains("uh1y"))
            return FGAM_SpawnContext.HELICRASH;
    }

    // Train: surface type check
    string surfaceType;
    GetGame().SurfaceGetType3D(pos[0], pos[1] + 0.5, pos[2], surfaceType);
    surfaceType.ToLower();
    if (surfaceType.Contains("rail") || surfaceType.Contains("train"))
        return FGAM_SpawnContext.TRAIN;

    // Beach: within 40m of water AND low elevation
    float waterDist = GetGame().GetWaterDepth(pos);
    if (waterDist >= 0 && waterDist < 40)
    {
        float terrH = GetGame().SurfaceY(pos[0], pos[2]);
        if (terrH < 5.0)
            return FGAM_SpawnContext.BEACH;
    }

    return FGAM_SpawnContext.NONE;
}
