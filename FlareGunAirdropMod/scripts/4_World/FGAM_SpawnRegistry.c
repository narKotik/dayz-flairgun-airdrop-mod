// FGAM_SpawnRegistry - tracks positions of live airdrop crates / toxic zones
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_SpawnRegistry.c
//
// Why this exists:
//   Despawn timers (FGAM_AirdropRemover, FGAM_ToxicArea.Tick's lifetime countdown)
//   live only in memory. A server restart wipes them, but the crate / gas-zone
//   entities themselves are dynamic world objects and get persisted and reloaded
//   from storage - so they'd otherwise sit on the map forever with nothing left
//   to remove them.
//
// For a permanent, pickable crate (airdropDespawnEnabled = false, see
// FGAM_AirdropContainer.c) the intent is: it should behave exactly like a
// normal SeaChest once claimed - a player who drags it into their base is
// meant to be able to store loot in it indefinitely, with its fate then tied
// to their base (abandon the base, and vanilla's own persistence/decay takes
// it down along with everything else - not us). So Sweep() only ever expires
// an entry that is STILL sitting exactly where it landed:
//   - moved at all (even a few metres)  -> treated as claimed. We stop
//     tracking it immediately and never touch it again, forever.
//   - still untouched, younger than maxAgeDays -> left alone, still watched.
//   - still untouched, older than maxAgeDays    -> deleted as abandoned litter.
//
// Known limitation: an entry is only searched for near its ORIGINAL drop
// position (within searchRadius). If it's moved beyond that radius before the
// first Sweep() sees it, we can't tell "claimed" from "gone" - we just stop
// finding it and drop it from tracking either way, which lands on the same
// "leave it alone" outcome, so this is harmless.

class FGAM_SpawnRecord
{
    string cls;
    float  x, y, z;
    float  searchRadius;
    int    spawnDaySerial;
    float  maxAgeDays; // 0 = no age-based expiry for this entry
};

class FGAM_SpawnRegistryData
{
    ref array<ref FGAM_SpawnRecord> entries;

    void FGAM_SpawnRegistryData()
    {
        entries = new array<ref FGAM_SpawnRecord>();
    }
};

class FGAM_SpawnRegistry
{
    private static const string PATH = "$profile:FlareGunAirdropMod/active_spawns.json";

    // How far (horizontally) an object can be from its recorded drop point and
    // still count as "untouched" for the stationary check in Sweep().
    private static const float STATIONARY_TOLERANCE = 3.0;

    private static ref FGAM_SpawnRegistryData Load()
    {
        FGAM_SpawnRegistryData data = new FGAM_SpawnRegistryData();
        if (FileExist(PATH))
            JsonFileLoader<FGAM_SpawnRegistryData>.JsonLoadFile(PATH, data);
        if (!data.entries)
            data.entries = new array<ref FGAM_SpawnRecord>();
        return data;
    }

    private static void Save(FGAM_SpawnRegistryData data)
    {
        JsonFileLoader<FGAM_SpawnRegistryData>.JsonSaveFile(PATH, data);
    }

    // Coarse but monotonic day counter based on the server's in-game calendar
    // date (not real wall-clock time - fine for a rough "X days old" cutoff).
    private static int CurrentDaySerial()
    {
        int year, month, day, hour, minute;
        GetGame().GetWorld().GetDate(year, month, day, hour, minute);
        return year * 372 + month * 31 + day;
    }

    // searchRadius should comfortably cover any distance the object can be
    // from the recorded position (e.g. a falling crate's spawn height) so
    // Sweep() can still find it if the server restarts mid-fall.
    // maxAgeDays for an entry still found stationary at its drop point:
    //   < 0  : delete immediately, no grace period (the normal despawn-timer
    //          path already handles removal in the ordinary case, so finding
    //          this entry stationary at all means a restart interrupted that
    //          timer - e.g. a non-takeable temporary crate, or a toxic zone).
    //     0  : never expire (permanent crate, admin wants no litter cleanup).
    //   > 0  : grace period in days before an untouched entry is deleted.
    static void Track(string cls, vector pos, float searchRadius, float maxAgeDays = -1)
    {
        FGAM_SpawnRegistryData data = Load();
        FGAM_SpawnRecord rec = new FGAM_SpawnRecord();
        rec.cls = cls;
        rec.x = pos[0];
        rec.y = pos[1];
        rec.z = pos[2];
        rec.searchRadius = searchRadius;
        rec.spawnDaySerial = CurrentDaySerial();
        rec.maxAgeDays = maxAgeDays;
        data.entries.Insert(rec);
        Save(data);
    }

    static void Untrack(string cls, vector pos)
    {
        FGAM_SpawnRegistryData data = Load();
        for (int i = data.entries.Count() - 1; i >= 0; i--)
        {
            FGAM_SpawnRecord rec = data.entries.Get(i);
            if (rec.cls == cls && vector.DistanceSq(Vector(rec.x, rec.y, rec.z), pos) < rec.searchRadius * rec.searchRadius)
            {
                data.entries.Remove(i);
                break;
            }
        }
        Save(data);
    }

    // Called once from FGAM_MissionServer.OnInit() (covers restarts) and then
    // repeatedly on a timer while the server keeps running (covers long
    // uptimes with no restart). See the class-level comment for the exact
    // "moved = claimed forever, untouched = subject to maxAgeDays" rule.
    static void Sweep()
    {
        if (!FileExist(PATH))
            return;

        FGAM_SpawnRegistryData data = Load();
        ref array<ref FGAM_SpawnRecord> kept = new array<ref FGAM_SpawnRecord>();
        int today = CurrentDaySerial();

        foreach (FGAM_SpawnRecord rec : data.entries)
        {
            vector pos = Vector(rec.x, rec.y, rec.z);
            array<Object> objs = new array<Object>();
            array<CargoBase> proxies = new array<CargoBase>();
            GetGame().GetObjectsAtPosition3D(pos, rec.searchRadius, objs, proxies);

            Object found = null;
            foreach (Object obj : objs)
            {
                if (obj && obj.ClassName() == rec.cls)
                {
                    found = obj;
                    break;
                }
            }

            if (!found)
                continue; // gone, or moved beyond searchRadius - either way, drop tracking

            vector opos = found.GetPosition();
            float dx = opos[0] - rec.x;
            float dz = opos[2] - rec.z;
            bool stationary = (dx * dx + dz * dz) <= STATIONARY_TOLERANCE * STATIONARY_TOLERANCE;

            if (!stationary)
                continue; // moved - claimed forever, drop tracking and never touch it again

            bool agedOut = rec.maxAgeDays < 0 || (rec.maxAgeDays > 0 && (today - rec.spawnDaySerial) >= rec.maxAgeDays);
            if (agedOut)
            {
                GetGame().ObjectDelete(found);
            }
            else
            {
                kept.Insert(rec); // still untouched, still within its grace period - keep watching
            }
        }

        FGAM_SpawnRegistryData newData = new FGAM_SpawnRegistryData();
        newData.entries = kept;
        Save(newData);
    }
};

// Drives the periodic Sweep() call - CallLater needs a bound instance method,
// so this tiny singleton just forwards the tick.
//
// The very first sweep is deliberately delayed (bootDelayMs) rather than run
// synchronously from MissionServer.OnInit(): persisted dynamic objects (like a
// crate reloaded from storage after a restart) aren't guaranteed to already
// exist in the world at that exact point, and Sweep() treats "not found" as
// "already gone / moved away - stop tracking it" - if it runs too early, a
// perfectly real, not-yet-loaded-in crate would get silently dropped from the
// registry forever instead of being cleaned up. Two separate bound methods are
// used (not one CallLater rescheduled twice) so the one-shot boot delay and
// the recurring interval don't collide in the engine's call queue.
class FGAM_SpawnRegistryMaintenance
{
    private static ref FGAM_SpawnRegistryMaintenance s_instance;

    static void Start(int bootDelayMs, int intervalMs)
    {
        if (!s_instance)
            s_instance = new FGAM_SpawnRegistryMaintenance();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_instance.BootTick, bootDelayMs, false);
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(s_instance.PeriodicTick, intervalMs, true);
    }

    void BootTick()
    {
        FGAM_SpawnRegistry.Sweep();
    }

    void PeriodicTick()
    {
        FGAM_SpawnRegistry.Sweep();
    }
};
