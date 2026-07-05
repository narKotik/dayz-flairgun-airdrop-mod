// FGAM_Config - server-side singleton, loads settings from JSON
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Config.c

class FGAM_FlareConfig
{
    int    shotsPerState         = 1;
    bool   canBeRepaired         = false;
    float  minTriggerPitch       = 30.0;   // degrees above horizontal the gun must be aimed to trigger
    float  minTriggerAltitude    = 40.0;   // (legacy/unused) metres the burning flare must reach
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    float  airdropDescentSpeed   = 6.0;     // metres/second the crate falls
    float  airdropLifetime       = 1800.0;  // seconds before the crate despawns (30 min)
    bool   airdropDespawnEnabled = true;    // false = crate stays forever and becomes pickable
    float  airdropMaxAgeDays     = 45.0;    // only used when airdropDespawnEnabled is false; 0 = never expire
    string airdropContainerClass = "FGAM_AirdropContainer";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 1500.0;

    ref map<string, int> helicrashFlareWeights;
    ref map<string, int> trainFlareWeights;
    ref map<string, int> beachFlareWeights;

    void FGAM_FlareConfig()
    {
        helicrashFlareWeights = new map<string, int>();
        trainFlareWeights     = new map<string, int>();
        beachFlareWeights     = new map<string, int>();
    }
};

class FGAM_Config
{
    private static ref FGAM_Config s_instance;

    ref FGAM_FlareConfig FlareSettings;
    ref map<string, ref TStringArray>  LootTables;

    static FGAM_Config Get()
    {
        if (!s_instance)
        {
            s_instance = new FGAM_Config();
            s_instance.Load();
        }
        return s_instance;
    }

    void FGAM_Config()
    {
        FlareSettings = new FGAM_FlareConfig();
        LootTables    = new map<string, ref TStringArray>();
    }

    void Load()
    {
        // Always establish a full, valid default state first.
        LoadDefaults();

        // Then overlay anything the server admin set in the profile JSON.
        string path = "$profile:FlareGunAirdropMod/config.json";
        if (FileExist(path))
        {
            FGAM_JsonRoot root = new FGAM_JsonRoot();
            JsonFileLoader<FGAM_JsonRoot>.JsonLoadFile(path, root);
            ApplyJson(root);
            Print("[FGAM] Config loaded from " + path);
        }
        else
        {
            Print("[FGAM] No config.json at " + path + " - using built-in defaults");
        }
    }

    private void ApplyJson(FGAM_JsonRoot root)
    {
        if (root.flare)
        {
            FlareSettings.shotsPerState          = root.flare.shotsPerState;
            if (root.flare.minTriggerPitch > 0)
                FlareSettings.minTriggerPitch    = root.flare.minTriggerPitch;
            if (root.flare.minTriggerAltitude > 0)
                FlareSettings.minTriggerAltitude = root.flare.minTriggerAltitude;
            FlareSettings.maxTriggerRadius       = root.flare.maxTriggerRadius;
            FlareSettings.airdropSpawnHeight     = root.flare.airdropSpawnHeight;
            if (root.flare.airdropDescentSpeed > 0)
                FlareSettings.airdropDescentSpeed = root.flare.airdropDescentSpeed;
            if (root.flare.airdropLifetime > 0)
                FlareSettings.airdropLifetime    = root.flare.airdropLifetime;
            FlareSettings.airdropDespawnEnabled  = root.flare.airdropDespawnEnabled;
            FlareSettings.airdropMaxAgeDays      = root.flare.airdropMaxAgeDays;
            if (root.flare.airdropContainerClass != "")
                FlareSettings.airdropContainerClass = root.flare.airdropContainerClass;
            FlareSettings.redZoneDelay           = root.flare.redZoneDelay;
            FlareSettings.redZoneRadius          = root.flare.redZoneRadius;
            FlareSettings.redZoneDuration        = root.flare.redZoneDuration;
        }

        TStringArray colors = new TStringArray();
        colors.Insert("RED");
        colors.Insert("GREEN");
        colors.Insert("BLUE");
        colors.Insert("WHITE");
        colors.Insert("YELLOW");
        colors.Insert("BLACK");
        colors.Insert("ORANGE");
        foreach (string color : colors)
        {
            // Only override the default loot for a colour when the JSON
            // actually supplies a non-empty list; otherwise keep the defaults.
            FGAM_JsonLootTable tbl = root.GetLootTable(color);
            if (tbl && tbl.items && tbl.items.Count() > 0)
            {
                TStringArray items = new TStringArray();
                items.Copy(tbl.items);
                LootTables.Set(color, items);
            }
        }

        if (root.spawnWeights)
        {
            ApplyWeightMap(root.spawnWeights.helicrash, FlareSettings.helicrashFlareWeights);
            ApplyWeightMap(root.spawnWeights.train,     FlareSettings.trainFlareWeights);
            ApplyWeightMap(root.spawnWeights.beach,     FlareSettings.beachFlareWeights);
        }
    }

    private void ApplyWeightMap(map<string, int> src, map<string, int> dst)
    {
        if (!src) return;
        dst.Copy(src);
    }

    TStringArray GetLootForColor(string color)
    {
        TStringArray items;
        if (LootTables.Find(color, items))
            return items;
        return new TStringArray();
    }

    private void LoadDefaults()
    {
        TStringArray red = new TStringArray();
        red.Insert("AKM");
        red.Insert("AKM");
        red.Insert("Mag_AKM_30Rnd");
        red.Insert("Mag_AKM_30Rnd");
        red.Insert("HighCapacityVest");
        red.Insert("BattleHelmet");
        red.Insert("MilitaryBoots_Black");
        LootTables.Set("RED", red);

        TStringArray green = new TStringArray();
        green.Insert("KnifeHunting");
        green.Insert("Hatchet");
        green.Insert("Matchbox");
        green.Insert("MedicalSupplies");
        green.Insert("TentDome");
        green.Insert("Bandage");
        green.Insert("Splint");
        LootTables.Set("GREEN", green);

        TStringArray blue = new TStringArray();
        blue.Insert("BloodBagKit_0Pos");
        blue.Insert("BloodBagKit_ABPos");
        blue.Insert("Epinephrine");
        blue.Insert("SurgicalKit");
        blue.Insert("Tetracycline");
        blue.Insert("Morphine");
        blue.Insert("Saline_500");
        LootTables.Set("BLUE", blue);

        TStringArray white = new TStringArray();
        white.Insert("ArmyRation");
        white.Insert("Can_SardinesOpened");
        white.Insert("Can_TunafishOpened");
        white.Insert("Canteen");
        white.Insert("WaterPurificationTablets");
        white.Insert("Disinfectant_Spray");
        LootTables.Set("WHITE", white);

        TStringArray yellow = new TStringArray();
        yellow.Insert("NBC_Suit");
        yellow.Insert("GasMask");
        yellow.Insert("GasMaskFilter");
        yellow.Insert("GasMaskFilter");
        yellow.Insert("Antidote");
        yellow.Insert("Epinephrine");
        yellow.Insert("Iodine");
        LootTables.Set("YELLOW", yellow);

        TStringArray black = new TStringArray();
        black.Insert("M4A1");
        black.Insert("M4A1");
        black.Insert("Mag_STANAG_30Rnd");
        black.Insert("Mag_STANAG_30Rnd");
        black.Insert("Mag_STANAG_30Rnd");
        black.Insert("PistolSuppressor");
        black.Insert("RifleButtstockM4");
        LootTables.Set("BLACK", black);

        TStringArray orange = new TStringArray();
        orange.Insert("Hammer");
        orange.Insert("Screwdriver");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("BarbedWire");
        orange.Insert("SparkPlug");
        orange.Insert("CarBattery");
        LootTables.Set("ORANGE", orange);

        FlareSettings.helicrashFlareWeights.Set("RED", 20);
        FlareSettings.helicrashFlareWeights.Set("GREEN", 15);
        FlareSettings.helicrashFlareWeights.Set("BLUE", 15);
        FlareSettings.helicrashFlareWeights.Set("WHITE", 15);
        FlareSettings.helicrashFlareWeights.Set("YELLOW", 10);
        FlareSettings.helicrashFlareWeights.Set("BLACK", 15);
        FlareSettings.helicrashFlareWeights.Set("ORANGE", 10);

        FlareSettings.trainFlareWeights.Set("RED", 25);
        FlareSettings.trainFlareWeights.Set("GREEN", 20);
        FlareSettings.trainFlareWeights.Set("BLUE", 15);
        FlareSettings.trainFlareWeights.Set("WHITE", 20);
        FlareSettings.trainFlareWeights.Set("BLACK", 10);
        FlareSettings.trainFlareWeights.Set("ORANGE", 10);

        FlareSettings.beachFlareWeights.Set("GREEN", 40);
        FlareSettings.beachFlareWeights.Set("BLUE", 25);
        FlareSettings.beachFlareWeights.Set("WHITE", 25);
        FlareSettings.beachFlareWeights.Set("ORANGE", 10);
    }
}

// JSON data classes
class FGAM_JsonFlareSettings
{
    int    shotsPerState         = 1;
    float  minTriggerPitch       = 30.0;
    float  minTriggerAltitude    = 40.0;
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    float  airdropDescentSpeed   = 6.0;
    float  airdropLifetime       = 1800.0;
    bool   airdropDespawnEnabled = true;
    float  airdropMaxAgeDays     = 45.0;
    string airdropContainerClass = "FGAM_AirdropContainer";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 1500.0;
};

class FGAM_JsonLootTable
{
    ref TStringArray items;
};

class FGAM_JsonSpawnWeights
{
    ref map<string, int> helicrash;
    ref map<string, int> train;
    ref map<string, int> beach;
};

class FGAM_JsonRoot
{
    ref FGAM_JsonFlareSettings flare;
    ref FGAM_JsonLootTable loot_RED;
    ref FGAM_JsonLootTable loot_GREEN;
    ref FGAM_JsonLootTable loot_BLUE;
    ref FGAM_JsonLootTable loot_WHITE;
    ref FGAM_JsonLootTable loot_YELLOW;
    ref FGAM_JsonLootTable loot_BLACK;
    ref FGAM_JsonLootTable loot_ORANGE;
    ref FGAM_JsonSpawnWeights spawnWeights;

    FGAM_JsonLootTable GetLootTable(string color)
    {
        switch (color)
        {
            case "RED":    return loot_RED;
            case "GREEN":  return loot_GREEN;
            case "BLUE":   return loot_BLUE;
            case "WHITE":  return loot_WHITE;
            case "YELLOW": return loot_YELLOW;
            case "BLACK":  return loot_BLACK;
            case "ORANGE": return loot_ORANGE;
        }
        return null;
    }
};
