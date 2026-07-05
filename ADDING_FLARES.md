# Adding a new flare colour (and its loot & spawn locations)

This guide covers four things:

1. **Add a new colour of flare** — the in-air coloured flare + its trigger.
2. **Attach loot to the crate it spawns.**
3. **Decide where the flare itself spawns in the world.**
4. **Give it a ground-loot ammo box** — the actual pickup players find (the loose
   cartridge can't be placed on the ground by CE — see Part 4).

Throughout, the worked example adds a **Purple** flare. Replace `Purple` / `PURPLE` /
`purple` with your colour, keeping the capitalisation in each spot.

After any change you must rebuild: run `1_copy_to_P.bat` then `2_deploy.bat`, then
restart the server (and relaunch the client for visual changes). See
[Build & test](#build--test) at the end.

---

## Part 1 — Add the flare colour

There are 8 small edits. The recipe is also summarised in the header of
`FlareGunAirdropMod/scripts/4_World/FGAM_FlareVisuals.c`.

### 1.1 Make the particle (the coloured flame + smoke)
Particles live in `FlareGunAirdropMod/Graphics/Particles/`.

**Easy way — use the generator.** Run `make_flare.bat` with a name and an R G B colour
(channels 0–1, dots):
```
make_flare.bat purple 0.6 0.1 1
```
It writes `FlareGunAirdropMod/Graphics/Particles/fgam_flare_purple.ptc`, recoloured from
an existing flare particle (all timing/smoke/emitter structure preserved, decimals
forced to dots). It also prints the exact `RegisterParticle` line you need for step 1.2.
More examples:
```
make_flare.bat cyan 0.1 0.9 1
make_flare.bat pink 1 0.4 0.7
```

**Manual way (optional).** Copy `fgam_flare_red.ptc` → `fgam_flare_<name>.ptc` and edit
every `Color { ... }` block by hand: the numbers are repeating groups of four —
**`time R G B`** — keep each `time`, change the `R G B` (0–1, **dots not commas**).
Example line: `Color { 0 0.6 0.1 1   0.0506 0.6 0.1 1   ... }`

**Tweak the smoke** (either way): in the `smoke` emitter of the `.ptc`,
`SizeMultiplier` = how big, `BirthRate` = how dense, `LifeTime` = how long the trail lingers.

> A flame can only be as "white"/"dark"/etc. as the colour you put here — this file
> *is* the flare's colour. (The engine has no runtime particle tinting, which is why we
> ship our own `.ptc` per colour.)

### 1.2 Register the particle — `scripts/3_Game/FGAM_Particles.c`
```c
static const int FGAM_FLARE_PURPLE = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_purple");
```

### 1.3 Light + simulation — `scripts/4_World/FGAM_FlareVisuals.c`
Add a light colour (tints the surroundings) and a simulation that ties the light to
the particle:
```c
class FGAM_FlareLightPurple : FGAM_FlareLightBase
{ void FGAM_FlareLightPurple() { SetAmbientColor(0.6, 0.2, 1.0); SetDiffuseColor(0.55, 0.15, 1.0); } }

class FGAM_FlareSimulation_Purple : FlareSimulation
{ void FGAM_FlareSimulation_Purple() { m_ScriptedLight = FGAM_FlareLightPurple; m_ParticleId = ParticleList.FGAM_FLARE_PURPLE; } }
```

### 1.4 Ammo (the fired round) — `config.cpp`, `class CfgAmmo`
```cpp
class FGAM_Bullet_FlarePurple : FGAM_Bullet_FlareBase { SimulationScriptClass = "FGAM_FlareSimulation_Purple"; };
```

### 1.5 Register the ammo — `config.cpp`, `class cfgAmmoTypes`  ⚠️ MANDATORY
```cpp
class AType_FGAM_Bullet_FlarePurple { name = "FGAM_Bullet_FlarePurple"; };
```
**If you skip this the game CRASHES when the flare is chambered** (engine access
violation in `WeaponChambering.GetCartridgeAtIndex`). Every flare round must be here.

### 1.6 Magazine (the cartridge item) — `config.cpp`, `class CfgMagazines`
```cpp
class FGAM_Mag_Purple : FGAM_Mag_FlareBase
{
    scope = 2;
    displayName = "Signal Flare (Purple)";
    descriptionShort = "Whatever this drop does.";
    ammo = "FGAM_Bullet_FlarePurple";
};
```
The inventory model stays the white cartridge (baked into the vanilla model); the
colour is shown by the name + the fired flare.

### 1.7 Let the flare gun chamber it — `config.cpp`, `class Flaregun_Base`
Add `"FGAM_Mag_Purple"` to `chamberableFrom[] += { ... }`.

### 1.8 Bind the magazine script class — `scripts/4_World/FGAM_Magazines.c`
```c
class FGAM_Mag_Purple : Ammunition_Base {};
```

### 1.9 Map the colour for the trigger — `scripts/4_World/FGAM_FlareGun.c`
In `FGAM_ColorFromAmmo()` add:
```c
if (ammoName.Contains("purple")) return "PURPLE";
```
(The substring must appear in the ammo class name `FGAM_Bullet_FlarePurple` → "purple".)

At this point the flare fires in colour. Next, make it *do* something.

---

## Part 2 — Attach loot to the crate it spawns

When a flare is fired into the sky, `FGAM_AirdropManager.OnFlareEvent` runs on the
server. Every colour **except Red** spawns a falling crate filled from that colour's
loot table. (Red instead spawns the toxic gas zone + a military crate.)

### 2.1 Make the colour spawn a crate — `scripts/4_World/FGAM_AirdropManager.c`
In `OnFlareEvent`, add your colour to the airdrop `case` list:
```c
case "GREEN":
case "BLUE":
case "PURPLE":      // <-- add
case "ORANGE":
    SpawnAirdrop(groundPos, color, cfg, cfg.FlareSettings.airdropLifetime);
    break;
```
(If you want it to do something custom instead — like Red's toxic zone — add its own
`case` with your own handler.)

### 2.2 Loot list (editable at runtime) — `ServerProfile/FlareGunAirdropMod/config.json`
Add a `loot_PURPLE` block. Items are exact DayZ class names; anything that doesn't fit
the crate drops on the ground beside it.
```json
"loot_PURPLE": {
    "items": [ "Rangefinder", "NVGoggles", "BatteryD", "BatteryD" ]
},
```
This file lives in the **server profile folder** (`<server>/Profiles/FlareGunAirdropMod/
config.json`); `2_deploy.bat` copies it there. Editing loot needs no rebuild — just
restart the server.

### 2.3 Make the JSON loot load — `scripts/4_World/FGAM_Config.c`
Three small edits so the new `loot_PURPLE` is read:

- In `class FGAM_JsonRoot`, add the field and a `GetLootTable` case:
  ```c
  ref FGAM_JsonLootTable loot_PURPLE;
  // ...
  case "PURPLE": return loot_PURPLE;
  ```
- In `ApplyJson()`, add your colour to the `colors` list:
  ```c
  colors.Insert("PURPLE");
  ```
- (Optional but recommended) In `LoadDefaults()`, add a built-in fallback so the crate
  isn't empty if the JSON is missing:
  ```c
  TStringArray purple = new TStringArray();
  purple.Insert("Rangefinder");
  LootTables.Set("PURPLE", purple);
  ```

That's the crate done: fire Purple in the sky → crate falls nearby → filled with
`loot_PURPLE`, despawns after `airdropLifetime`.

---

## Part 3 — Where the flare spawns in the world

Flares (and the flare gun) spawn through DayZ's **central economy** (`types.xml`), not
through the mod's code. Add an entry in `db/FGAM_types.xml` (this is merged into the
mission as a separate CE file by `cfgeconomycore.xml`).

> **The magazine itself does NOT ground-spawn.** `Flare_SingleRound.p3d` (the vanilla
> cartridge model every `FGAM_Mag_<Colour>` uses) is a thin rod CE can't reliably place
> on a surface, so in this mod every `FGAM_Mag_<Colour>` type ships at
> `nominal="0"`/`count_in_map="0"` — unpack-only, never a direct pickup (see the real
> entries in `db/FGAM_types.xml`, not the illustrative one below). The thing a player
> actually finds while looting the ground is the **ammo box** — see **Part 4**.

```xml
<type name="FGAM_Mag_Purple">
    <nominal>4</nominal>      <!-- target number on the map at once -->
    <lifetime>7200</lifetime> <!-- seconds before an un-touched one despawns -->
    <restock>1200</restock>   <!-- seconds before depleted stock is topped up -->
    <min>2</min>              <!-- CE forces a respawn if count drops below this -->
    <quantmin>-1</quantmin>
    <quantmax>-1</quantmax>
    <cost>100</cost>
    <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
    <category name="weapons"/>
    <usage name="Military"/>   <!-- WHERE: building type (see below) -->
    <value name="Tier3"/>      <!-- WHERE: map area richness/tier -->
    <value name="Tier4"/>
</type>
```

**`usage` decides which building types it spawns in.** Common ones used in this mod:
`Military`, `Police`, `Medic`, `Hunting`, `Civilian`, `Industrial`. (Full list is in
the vanilla `cfglimitsdefinition.xml`.)

**`value` (Tier1–4) decides which map regions** — higher tiers = more remote/dangerous
areas. Omit `value` entirely to allow all regions.

**`nominal`/`min`/`restock`/`lifetime`** control how many exist and how fast they come
back. Rarer drop → lower `nominal`, higher `restock` (see how `FGAM_Mag_Black` is set
rarer than `FGAM_Mag_Green`).

> **Note:** there is leftover scaffolding (`spawnWeights` in `config.json` and
> `FGAM_SpawnHandler.c`) for spawning the gun pre-loaded with a weighted colour at
> heli-crashes / trains / beaches. It is **not currently wired up** — editing
> `spawnWeights` does nothing today. Real-world spawning is controlled entirely by
> `db/FGAM_types.xml` above.

---

## Part 4 — Add the ground-loot ammo box

Players don't find the loose cartridge on the ground (Part 3's note above explains
why) — they find a **box** (`FGAM_Box_<Colour>`) that unpacks (right-click "Unbox")
into one `FGAM_Mag_<Colour>`. It's built on the vanilla `00Buck_10RoundBox.p3d` model
(proven ground-spawnable), the same trick real ammo boxes use — see
`FGAM_Box_FlareBase` in `config.cpp` for the full mechanism.

### 4.1 Generate the label texture
Textures live in `FlareGunAirdropMod/data/`. Use the generator — it draws a clean
"SIGNAL FLARE" label from scratch (no reused vanilla shotgun-shell art) and converts it
straight to `.paa`:
```
make_box.bat purple 155 30 220
```
- `name` — lowercase, must match the `_co.paa` filename the box class below expects
- `R G B` — **0–255** (not 0–1 like `make_flare.bat`)

Writes `FlareGunAirdropMod\data\fgam_box_purple_co.paa`. Needs **Python + Pillow**
(`pip install pillow`) and **DayZ Tools** installed (for `ImageToPAA.exe` — the script
auto-detects common install paths, or set `FGAM_IMAGETOPAA` to its full path). Pass
`--png-only` to `make_box.py` directly to preview the PNG before converting.

### 4.2 Add the box class — `config.cpp`, `class CfgVehicles`
Copy an existing `FGAM_Box_<Colour>` (e.g. `FGAM_Box_Orange`) right after it:
```cpp
class FGAM_Box_Purple : FGAM_Box_FlareBase
{
    scope = 2;
    displayName = "Ammo Box (Signal Flares - Purple)";
    descriptionShort = "Unpack for a purple signal flare.";
    hiddenSelectionsTextures[] = { "FlareGunAirdropMod\data\fgam_box_purple_co.paa" };
    class Resources { class FGAM_Mag_Purple { value = 1; variable = "quantity"; }; };
};
```
`value` is how many rounds unpacking gives you (currently `1` for every colour).

Also add `"FGAM_Box_Purple"` to `CfgPatches > FlareGunAirdropMod > units[]` near the
top of `config.cpp`, alongside the other `FGAM_Box_*` entries — classes missing from
`units[]` can still be referenced, but won't show up correctly in some mod-listing/
Workshop tooling.

### 4.3 Where the box spawns — `db/FGAM_types.xml`
This is the entry CE actually uses (unlike the illustrative `FGAM_Mag_Purple` one in
Part 3). Copy an existing `FGAM_Box_<Colour>` block:
```xml
<type name="FGAM_Box_Purple">
    <nominal>5</nominal>
    <lifetime>7200</lifetime>
    <restock>900</restock>
    <min>2</min>
    <quantmin>-1</quantmin>
    <quantmax>-1</quantmax>
    <cost>100</cost>
    <flags count_in_cargo="0" count_in_hoarder="0" count_in_map="1" count_in_player="0" crafted="0" deloot="0"/>
    <category name="weapons"/>
    <usage name="Coast"/>
    <usage name="Village"/>
    <usage name="Town"/>
</type>
```
Stack multiple `usage` tags (they're OR'd) to avoid CE "exceeded max tests" starvation
on a single narrow tag — see the README's *Color → Event* section for which usage
combo each existing colour uses (common vs. rare/military-grade tiers).

---

## Build & test

1. `1_copy_to_P.bat` — syncs source, binarises config, packs + signs the PBO (CLI, no
   GUI). It already includes `scripts`, `data`, and `Graphics`.
2. `2_deploy.bat` — copies the PBO + key to the server and client, and copies
   `config.json` / `FGAM_types.xml` into the mission.
3. Restart the server; relaunch the client (needed to see new particles).

Test checklist for the new colour:
- Chamber it → **no crash** (means step 1.5 cfgAmmoTypes is correct).
- Fire it **up, outdoors** → flame + smoke in your colour, staying attached.
- ~A crate descends nearby with your `loot_PURPLE` items.
- Server log (`<server>/Profiles/DayZServer_x64_*.RPT`) shows
  `[FGAM] PURPLE flare launched - airdrop at ...`.
- `FGAM_Box_Purple` unpacks (right-click "Unbox") into one loaded `FGAM_Mag_Purple`.
- Over time, the box appears as ground loot in the `usage` locations you set in Part 4
  (CE fills sparse `nominal` counts gradually, not instantly on server start — give it
  real playtime near matching points before assuming it's broken).

If the flare fires but shows **light only, no flame/smoke**, the `.ptc` didn't load —
re-check the path/name in steps 1.1–1.2 (`FlareGunAirdropMod/Graphics/Particles/<name>.ptc`).
