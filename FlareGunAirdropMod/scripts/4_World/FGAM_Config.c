// FGAM_Config - server-side singleton, loads settings from JSON
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_Config.c

class FGAM_FlareConfig
{
    int    shotsPerState         = 1;
    bool   canBeRepaired         = false;
    float  minTriggerPitch       = 70.0;   // degrees above horizontal the gun must be aimed to trigger
    float  minTriggerAltitude    = 40.0;   // (legacy/unused) metres the burning flare must reach
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    float  airdropDescentSpeed   = 6.0;     // metres/second the crate falls
    float  airdropLifetime       = 300.0;   // seconds before the crate despawns (5 min)
    bool   airdropDespawnEnabled = true;    // false = crate stays forever and becomes pickable
    float  airdropMaxAgeDays     = 5.0;     // only used when airdropDespawnEnabled is false; 0 = never expire
    string airdropContainerClass = "FGAM_AirdropContainer";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 300.0;

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
        red.Insert("PSO6Optic");
        red.Insert("AK_Suppressor");
        red.Insert("AK_woodbttstck_black");
        red.Insert("AK_railhndgrd_black");
        red.Insert("Mag_AKM_Drum75Rnd");
        red.Insert("Mag_AKM_Drum75Rnd");
        red.Insert("BDUJacket");
        red.Insert("BDUPants");
        red.Insert("PlateCarrierVest_Camo");
        red.Insert("MilitaryBoots_Black");
        red.Insert("TacticalGoggles");
        red.Insert("MilitaryBelt");
        red.Insert("NylonKnifeSheath");
        red.Insert("PlateCarrierHolster_Camo");
        red.Insert("Attack2Bag_Ttsko");
        red.Insert("TacticalGloves_Black");
        red.Insert("Mich2001Helmet");
        LootTables.Set("RED", red);

        TStringArray green = new TStringArray();
        green.Insert("KnifeHunting");
        green.Insert("HipPack_Green");
        green.Insert("Hatchet");
        green.Insert("CivilianBelt");
        green.Insert("NylonKnifeSheath");
        green.Insert("PlateCarrierHolster_Camo");
        green.Insert("BallisticHelmet_Blue");
        green.Insert("PressVest_Blue");
        green.Insert("Matchbox");
        green.Insert("HuntingKnife");
        green.Insert("HuntingBag");
        green.Insert("HuntingVest");
        green.Insert("HuntingJacket_Brown");
        green.Insert("HunterPants_Brown");
        green.Insert("WorkingGloves_Brown");
        green.Insert("MKII");
        green.Insert("Mag_MKII_10Rnd");
        green.Insert("UMP45");
        green.Insert("Mag_UMP_25Rnd");
        green.Insert("Mag_UMP_25Rnd");
        green.Insert("PistolSuppressor");
        green.Insert("BandageDressing");
        green.Insert("BandageDressing");
        green.Insert("Epinephrine");
        LootTables.Set("GREEN", green);

        TStringArray blue = new TStringArray();
        blue.Insert("PurificationTablets");
        blue.Insert("PainkillerTablets");
        blue.Insert("VitaminBottle");
        blue.Insert("IodineTincture");
        blue.Insert("CharcoalTablets");
        blue.Insert("SalineBagIV");
        blue.Insert("SalineBagIV");
        blue.Insert("Epinephrine");
        blue.Insert("Epinephrine");
        blue.Insert("WaterPurificationTablets");
        blue.Insert("WaterPurificationTablets");
        blue.Insert("Morphine");
        blue.Insert("Morphine");
        blue.Insert("TetracyclineAntibiotics");
        blue.Insert("TetracyclineAntibiotics");
        blue.Insert("BandageDressing");
        blue.Insert("BandageDressing");
        blue.Insert("BandageDressing");
        blue.Insert("AntiChemInjector");
        blue.Insert("AntiChemInjector");
        blue.Insert("ParamedicPants_Green");
        blue.Insert("ParamedicJacket_Green");
        blue.Insert("MedicalScrubsHat_Green");
        blue.Insert("HipPack_Medical");
        blue.Insert("PressVest_Blue");
        blue.Insert("BallisticHelmet_Blue");
        blue.Insert("CanvasBag_Medical");
        blue.Insert("SurgicalGloves_Green");
        LootTables.Set("BLUE", blue);

        TStringArray white = new TStringArray();
        white.Insert("DryBag_Green");
        white.Insert("Canteen");
        white.Insert("Honey");
        white.Insert("Honey");
        white.Insert("Honey");
        white.Insert("Honey");
        white.Insert("SodaCan_Cola");
        white.Insert("SodaCan_Sprite");
        white.Insert("SodaCan_Pipsi");
        white.Insert("SodaCan_Kvass");
        white.Insert("Matchbox");
        white.Insert("WaterPurificationTablets");
        white.Insert("WaterPurificationTablets");
        white.Insert("VitaminBottle");
        LootTables.Set("WHITE", white);

        TStringArray yellow = new TStringArray();
        yellow.Insert("NBCJacketGray");
        yellow.Insert("NBCHoodGray");
        yellow.Insert("NBCGlovesGray");
        yellow.Insert("NBCBootsGray");
        yellow.Insert("NBCPantsGray");
        yellow.Insert("AirborneMask");
        yellow.Insert("Attack2Bag_Black");
        yellow.Insert("AntiChemInjector");
        yellow.Insert("AntiChemInjector");
        yellow.Insert("GasMask_Filter");
        yellow.Insert("GasMask_Filter");
        yellow.Insert("GasMask_Filter");
        yellow.Insert("Epinephrine");
        yellow.Insert("TireRepairKit");
        yellow.Insert("IodineTincture");
        LootTables.Set("YELLOW", yellow);

        TStringArray black = new TStringArray();
        black.Insert("AliceBag_Camo");
        black.Insert("M4A1");
        black.Insert("M4_MPBttstck");
        black.Insert("M4_RISHndgrd");
        black.Insert("ACOGOptic_6x");
        black.Insert("M4_Suppressor");
        black.Insert("Mag_STANAG_60Rnd");
        black.Insert("Mag_STANAG_60Rnd");
        black.Insert("PistolSuppressor");
        black.Insert("RifleButtstockM4");
        black.Insert("RailgunGrip");
        black.Insert("ACOG4xScopeOptic");
        black.Insert("Glock19");
        black.Insert("Mag_Glock_15Rnd");
        black.Insert("Mag_Glock_15Rnd");
        black.Insert("BulletproofVest_Press");
        black.Insert("TacticalHelmet_Black");
        LootTables.Set("BLACK", black);

        TStringArray orange = new TStringArray();
        orange.Insert("CarRadiator");
        orange.Insert("CoyoteBag_Green");
        orange.Insert("CarBattery");
        orange.Insert("SparkPlug");
        orange.Insert("TruckBattery");
        orange.Insert("GlowPlug");
        orange.Insert("ElectronicRepairKit");
        orange.Insert("TireRepairKit");
        orange.Insert("Hammer");
        orange.Insert("BarbedWire");
        orange.Insert("BarbedWire");
        orange.Insert("Wire");
        orange.Insert("Screwdriver");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("Nails");
        orange.Insert("EpoxyPutty");
        orange.Insert("EpoxyPutty");
        orange.Insert("EpoxyPutty");
        orange.Insert("Pliers");
        orange.Insert("PickAxe");
        orange.Insert("WoodAxe");
        orange.Insert("Hatchet");
        orange.Insert("HandSaw");
        orange.Insert("Shovel");
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
    float  minTriggerPitch       = 70.0;
    float  minTriggerAltitude    = 40.0;
    float  maxTriggerRadius      = 1500.0;
    float  airdropSpawnHeight    = 100.0;
    float  airdropDescentSpeed   = 6.0;
    float  airdropLifetime       = 300.0;
    bool   airdropDespawnEnabled = true;
    float  airdropMaxAgeDays     = 5.0;
    string airdropContainerClass = "FGAM_AirdropContainer";
    float  redZoneDelay          = 300.0;
    float  redZoneRadius         = 50.0;
    float  redZoneDuration       = 300.0;
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
