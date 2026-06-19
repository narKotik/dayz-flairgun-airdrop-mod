// FGAM_SpawnHandler - spawn context detection helpers
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_SpawnHandler.c

enum FGAM_SpawnContext
{
    NONE      = 0,
    HELICRASH = 1,
    TRAIN     = 2,
    BEACH     = 3,
}

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

FGAM_SpawnContext FGAM_DetectSpawnContext(vector pos)
{
    array<Object> nearby = new array<Object>();
    GetGame().GetObjectsAtPosition(pos, 30, nearby, null);
    foreach (Object obj : nearby)
    {
        string cn = obj.GetType();
        cn.ToLower();
        if (cn.Contains("helicrash") || cn.Contains("mi8") || cn.Contains("uh1y"))
            return FGAM_SpawnContext.HELICRASH;
    }

    string surfaceType;
    GetGame().SurfaceGetType3D(pos[0], pos[1] + 0.5, pos[2], surfaceType);
    surfaceType.ToLower();
    if (surfaceType.Contains("rail") || surfaceType.Contains("train"))
        return FGAM_SpawnContext.TRAIN;

    float waterDist = GetGame().GetWaterDepth(pos);
    if (waterDist >= 0 && waterDist < 40)
    {
        float terrH = GetGame().SurfaceY(pos[0], pos[2]);
        if (terrH < 5.0)
            return FGAM_SpawnContext.BEACH;
    }

    return FGAM_SpawnContext.NONE;
}
