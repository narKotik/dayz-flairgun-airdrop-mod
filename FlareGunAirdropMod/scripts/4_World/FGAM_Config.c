// FGAM_Config - server-side singleton, loads settings from JSON
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Config.c

class FGAM_FlareConfig
{
    int    shotsPerState         = 1;
    bool   canBeRepaired         = false;
    float  minTriggerAltitude    = 50.0;
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    string airdropContainerClass = "SeaChest";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 1500.0;

    ref TStringIntMap helicrashFlareWeights;
    ref TStringIntMap trainFlareWeights;
    ref TStringIntMap beachFlareWeights;

    void FGAM_FlareConfig()
    {
        helicrashFlareWeights = new TStringIntMap();
        trainFlareWeights     = new TStringIntMap();
        beachFlareWeights     = new TStringIntMap();
    }
};

class FGAM_Config
{
    private static ref FGAM_Config s_instance;

    ref FGAM_FlareConfig Flare;
    ref TStringArrayMap  LootTables;

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
        Flare      = new FGAM_FlareConfig();
        LootTables = new TStringArrayMap();
    }

    void Load()
    {
        string cfgPath = "$profile:FlareGunAirdropMod\\config.json";

        if (!FileExist(cfgPath))
        {
            Print("[FGAM] config.json not found at: " + cfgPath + " - using defaults");
            LoadDefaults();
            return;
        }

        JsonFileLoader<FGAM_JsonRoot> loader = new JsonFileLoader<FGAM_JsonRoot>();
        FGAM_JsonRoot root;
        string err;
        if (!loader.LoadFile(cfgPath, root, err))
        {
            Print("[FGAM] Failed to parse config.json: " + err + " - using defaults");
            LoadDefaults();
            return;
        }

        ApplyJson(root);
        Print("[FGAM] Config loaded from: " + cfgPath);
    }

    private void ApplyJson(FGAM_JsonRoot root)
    {
        if (root.flare)
        {
            Flare.shotsPerState          = root.flare.shotsPerState;
            Flare.minTriggerAltitude     = root.flare.minTriggerAltitude;
            Flare.maxTriggerRadius       = root.flare.maxTriggerRadius;
            Flare.airdropSpawnHeight     = root.flare.airdropSpawnHeight;
            Flare.airdropContainerClass  = root.flare.airdropContainerClass;
            Flare.redZoneDelay           = root.flare.redZoneDelay;
            Flare.redZoneRadius          = root.flare.redZoneRadius;
            Flare.redZoneDuration        = root.flare.redZoneDuration;
        }

        TStringArray colors = new TStringArray();
        colors.Insert("RED"); colors.Insert("GREEN"); colors.Insert("BLUE");
        colors.Insert("WHITE"); colors.Insert("YELLOW"); colors.Insert("BLACK"); colors.Insert("ORANGE");
        foreach (string color : colors)
        {
            TStringArray items = new TStringArray();
            FGAM_JsonLootTable tbl = root.GetLootTable(color);
            if (tbl && tbl.items)
                items.Copy(tbl.items);
            LootTables.Set(color, items);
        }

        if (root.spawnWeights)
        {
            ApplyWeightMap(root.spawnWeights.helicrash, Flare.helicrashFlareWeights);
            ApplyWeightMap(root.spawnWeights.train,     Flare.trainFlareWeights);
            ApplyWeightMap(root.spawnWeights.beach,     Flare.beachFlareWeights);
        }
    }

    private void ApplyWeightMap(TStringIntMap src, TStringIntMap dst)
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
        TStringArray red = {"AKM","AKM","Mag_AKM_30Rnd","Mag_AKM_30Rnd",
                            "HighCapacityVest","BattleHelmet","MilitaryBoots_Black"};
        LootTables.Set("RED", red);

        TStringArray green = {"KnifeHunting","Hatchet","Matchbox","MedicalSupplies",
                               "TentDome","Bandage","Splint"};
        LootTables.Set("GREEN", green);

        TStringArray blue = {"BloodBagKit_0Pos","BloodBagKit_ABPos","Epinephrine",
                              "SurgicalKit","Tetracycline","Morphine","Saline_500"};
        LootTables.Set("BLUE", blue);

        TStringArray white = {"ArmyRation","Can_SardinesOpened","Can_TunafishOpened",
                               "Canteen","WaterPurificationTablets","Disinfectant_Spray"};
        LootTables.Set("WHITE", white);

        TStringArray yellow = {"NBC_Suit","GasMask","GasMaskFilter","GasMaskFilter",
                                "Antidote","Epinephrine","Iodine"};
        LootTables.Set("YELLOW", yellow);

        TStringArray black = {"M4A1","M4A1","Mag_STANAG_30Rnd","Mag_STANAG_30Rnd",
                               "Mag_STANAG_30Rnd","PistolSuppressor","RifleButtstockM4"};
        LootTables.Set("BLACK", black);

        TStringArray orange = {"Hammer","Screwdriver","Nails","Nails","Nails",
                                "BarbedWire","SparkPlug","CarBattery"};
        LootTables.Set("ORANGE", orange);

        Flare.helicrashFlareWeights.Set("RED", 20);
        Flare.helicrashFlareWeights.Set("GREEN", 15);
        Flare.helicrashFlareWeights.Set("BLUE", 15);
        Flare.helicrashFlareWeights.Set("WHITE", 15);
        Flare.helicrashFlareWeights.Set("YELLOW", 10);
        Flare.helicrashFlareWeights.Set("BLACK", 15);
        Flare.helicrashFlareWeights.Set("ORANGE", 10);

        Flare.trainFlareWeights.Set("RED", 25);
        Flare.trainFlareWeights.Set("GREEN", 20);
        Flare.trainFlareWeights.Set("BLUE", 15);
        Flare.trainFlareWeights.Set("WHITE", 20);
        Flare.trainFlareWeights.Set("BLACK", 10);
        Flare.trainFlareWeights.Set("ORANGE", 10);

        Flare.beachFlareWeights.Set("GREEN", 40);
        Flare.beachFlareWeights.Set("BLUE", 25);
        Flare.beachFlareWeights.Set("WHITE", 25);
        Flare.beachFlareWeights.Set("ORANGE", 10);
    }
}

// JSON data classes
class FGAM_JsonFlareSettings
{
    int    shotsPerState         = 1;
    float  minTriggerAltitude    = 50.0;
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    string airdropContainerClass = "SeaChest";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 600.0;
};

class FGAM_JsonLootTable
{
    ref TStringArray items;
};

class FGAM_JsonSpawnWeights
{
    ref TStringIntMap helicrash;
    ref TStringIntMap train;
    ref TStringIntMap beach;
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
