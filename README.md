# Flare Gun Airdrop Mod (FGAM)

Fire a colored flare into the sky — trigger a unique airdrop or zone event.

## Installation

See [INSTALL.md](INSTALL.md).

## Flares & colors

Each FGAM flare fires a **colored burning flare** — slow rise/fall, bright, with a
colored smoke trail — and triggers its airdrop when it is **fired up into the open
sky** (aiming flat or into a ceiling/roof does nothing). The *inventory cartridge*
keeps the vanilla (white) flare model — that color is baked into the binarized model
and can't be changed — so flares are told apart by name + airdrop.

How it's built:

- **Light colour / brightness** → `FGAM_FlareLight*` in `FGAM_FlareVisuals.c` (client visuals)
- **Flame + smoke colour** → our own recoloured particles in `FlareGunAirdropMod/Graphics/Particles/*.ptc`, registered in `FGAM_Particles.c` and used as each flare's core particle (engine keeps it glued to the flare)
- **Rise / fall / burn time** → `FGAM_Bullet_FlareBase` ballistics in `config.cpp`
- **Airdrop trigger** → `Weapon_Base.OnFire` in `FGAM_FlareGun.c` (server-side; reads
  the fired ammo's color, gated by aim pitch + roof check)
- **Ammo registry** → `cfgAmmoTypes` in `config.cpp` (mandatory — every flare cartridge
  must be registered or chambering crashes the game)
- **Aim gate** → `minTriggerPitch` in `config.json`

> On a **dedicated** server the engine never runs the flare's visual simulation, so the
> trigger lives in the weapon's server-side `OnFire`, not in the flare simulation.

**Adding a new flare colour** (plus its crate loot, where it spawns, and its
ground-loot ammo box) is documented step-by-step in
[ADDING_FLARES.md](ADDING_FLARES.md). A short version of the colour recipe is also in
the header of `FGAM_FlareVisuals.c`.

FGAM extends `Flaregun_Base.chamberableFrom` (rather than the derived `Flaregun`)
so it stays compatible with other flare-gun mods that do the same.

FGAM itself is free to use and modify, with credit — see [LICENSE.md](LICENSE.md).
If you just want to run it on your server (not build it from source), see
[INSTALL.md](INSTALL.md). Publishing your own build to the Steam Workshop is covered
in [PUBLISHING.md](PUBLISHING.md); switching your local server/client between the
Workshop build and your local dev build for testing is covered in
[TESTING.md](TESTING.md).

## Ammo box label textures

Each colour's ground-loot box (`FGAM_Box_<Colour>`) has its own label texture,
`FlareGunAirdropMod/data/fgam_box_<colour>_co.paa`, referenced by
`hiddenSelectionsTextures[]` on that class in `config.cpp`. These are generated —
not hand-painted — by `make_box.bat`, which draws an all-new "SIGNAL FLARE" label
(no leftover vanilla shotgun-shell text/barcode) with Pillow and converts it to
`.paa` via DayZ Tools' `ImageToPAA.exe`.

```
make_box.bat <name> <R> <G> <B>
```

- `name` — lowercase colour name, must match the `_co.paa` filename the box class
  expects (e.g. `red` → `fgam_box_red_co.paa`)
- `R G B` — colour channels **0..255** (not 0..1 like `make_flare.bat`)

Examples:
```
make_box.bat purple 155 30 220
make_box.bat cyan   20  220 230
```

Writes `FlareGunAirdropMod\data\fgam_box_<name>_co.paa` directly — rerun it any
time to regenerate a colour's label (e.g. after tweaking the layout in
`make_box.py`). Requires **Python + Pillow** (`pip install pillow`) and **DayZ
Tools** installed (for `ImageToPAA.exe`); it auto-detects common install paths,
or set `FGAM_IMAGETOPAA` to the exe's full path if it can't find it. Pass
`--png-only` to `make_box.py` directly to skip the `.paa` step and inspect the
PNG first.

**Adding a new box colour** still needs the matching `FGAM_Box_<Colour>` class in
`config.cpp` (copy an existing one, point `hiddenSelectionsTextures[]` at the new
`.paa`), a `db/FGAM_types.xml` entry so CE spawns it, and a rebuild — this script only
regenerates the texture file itself. Full step-by-step: [ADDING_FLARES.md, Part
4](ADDING_FLARES.md#part-4--add-the-ground-loot-ammo-box).

## Color → Event

| Color  | Event |
|--------|-------|
| 🔴 Red    | Toxic zone (ContaminatedArea_Dynamic) spawns after 5 min + military weapon crate (AKM, plate carrier, tactical gear) inside the gas |
| 🟡 Yellow | CBRN/NBC protection kit (suit, mask, filters, anti-chem injectors) |
| 🟢 Green  | Survival kit (hunting knife/hatchet/bag, UMP45, MKII, basic clothing, bandages) |
| 🔵 Blue   | Medical drop (saline, morphine, antibiotics, epinephrine, bandages, medical clothing) |
| ⚪ White  | Provisions (canned/preserved food, drinks, water purification tabs) |
| ⚫ Black  | Top-tier weapons & ammo (M4A1, Glock19) — no light at night, thick black smoke only |
| 🟠 Orange | Construction tools & vehicle parts (car/truck batteries, spark/glow plugs, repair kits) |

## How it works

1. Player fires the flare gun **aimed up into the open sky** (firing flat, or into a
   ceiling/roof, does nothing — gated by `minTriggerPitch` + a roof check). Each colour
   is its own cartridge (`FGAM_Mag_<Colour>`) that fires its own coloured flame + smoke
   (our own particles).
2. Server-side, `Weapon_Base.EEFired` reads the fired round's colour.
3. A big-cargo crate (`FGAM_AirdropContainer`) spawns `airdropSpawnHeight` m above the
   shooter and descends gently (server-driven, visible to everyone). Loot rides down
   **inside the crate's cargo**; anything that doesn't fit drops beside it.
4. The crate's loot comes from the `loot_<COLOUR>` list in `config.json`.
5. The landed crate is removed after `airdropLifetime` seconds.
6. **Red** instead spawns a toxic gas zone (`FGAM_ToxicArea`) after `redZoneDelay`, with
   a military crate dropped into the gas. Both clear after `redZoneDuration`.

## Server setup & configuration

Everything an admin touches, grouped by **where it lives**. Only the `@FlareGunAirdropMod`
PBO is "the mod"; the rest are ordinary server/mission files you edit. The two helper
scripts `1_copy_to_P.bat` (build) and `2_deploy.bat` (deploy) automate all of this for a
local server + client.

### 1. Install the mod (server + every client)
- Copy the built `@FlareGunAirdropMod/` folder onto the server (and each client, or
  publish it to the Workshop).
- Add it to the server start parameters — load order relative to other mods doesn't
  matter:
  ```
  -mod=@CF;@VPPAdminTools;@FlareGunAirdropMod
  ```
  **FGAM has no mod dependencies.** `@CF` / `@VPPAdminTools` above are just this server's
  other mods; FGAM only needs itself.
- If the server verifies signatures, copy the mod key into the server's `keys/` folder:
  ```
  @FlareGunAirdropMod/keys/FGAM_v1.bikey  ->  <server>/keys/FGAM_v1.bikey
  ```

### 2. Make flares & the gun spawn in the world (mission CE files — not the mod)
Central-economy setup, in your **mission** folder. Two files:

**a) `mpmissions/<your.mission>/db/FGAM_types.xml`** — copy it from this repo's `db/`, and
make sure `cfgeconomycore.xml` references it under `<ce folder="db"><file name="FGAM_types.xml" type="types"/></ce>`
**with that exact filename** — a typo'd/renamed file fails silently (CE logs
`Failed to read types file 'db/FGAM_types.xml'` and the type is simply never registered).

The **magazines themselves never spawn** (`count_in_map="0" count_in_cargo="0"`, all
zero) — they use vanilla's `Flare_SingleRound.p3d`, a thin rod-shaped model that CE
can't reliably place on the ground (confirmed via RPT `exceeded max tests` / `is hard
to place`; the same reason Bohemia ships `Ammo_Flare`/`Ammo_FlareRed`/`Blue`/`Green`
at `nominal=0`). Instead, ground loot is an **ammo box per colour**
(`FGAM_Box_<Colour>`, `count_in_map="1"`), built on the vanilla `00Buck_10RoundBox`
model (proven ground-spawnable — see `AmmoBox_12gaSlug_10Rnd`). Right-click → **Unbox**
on the box to get one `FGAM_Mag_<Colour>` pre-loaded with 3 flares — that's the vanilla
`Box_Base` unpack action (native engine feature, see `config.cpp` `FGAM_Box_FlareBase`
for how), no custom script involved:
```xml
<type name="FGAM_Box_Green">
    <nominal>8</nominal>
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
> **Why multiple `<usage>` tags matter:** `usage` tags are OR'd — a type spawns at any
> point tagged with ANY of its listed usages. `category="weapons" usage="Coast"` alone
> only matches **~10 ground points on all of Chernarus** (mostly already claimed by the
> vanilla `Flaregun` itself), so CE spams `Item exceeded max tests` and nothing ever
> appears. Stacking usages (mirrors vanilla `Ammo_FlareRed/Blue/Green`, which combine
> Military+Police+Medic+Firefighter+Coast) opens up the pool — e.g. `Coast+Village+Town`
> covers ~100 points. Also do **not** use `usage name="Civilian"` — it's not a valid tag
> in `cfglimitsdefinition.xml` and silently matches nothing.
>
> Rarity tiers used here: **common** (Green, Blue, White, Yellow, Orange) →
> `Coast + Village + Town`. **Rare/military-grade** (Red, Black) → `Military + Industrial`,
> which also makes them turn up in heli crash / convoy / train wreck loot — those events
> pull from the same tagged CE pool by proximity, no `events.xml` edits needed.

> **The flare gun spawns via your base `types.xml`, not this file.** `Flaregun` is a
> vanilla class already defined in the mission's `db/types.xml` at `nominal 0` (never
> world-spawned). A duplicate `Flaregun` entry in a *separate* CE file does **not**
> override it — the base definition wins. To make the gun spawn, **edit the `Flaregun`
> entry in your mission's main `db/types.xml`** (e.g. `<nominal>12</nominal>`,
> `<min>3</min>`); its vanilla `usage` already covers Military/Police/Medic/Firefighter/Coast.
> That's why `FGAM_types.xml` contains only the flare cartridges, not the gun.

> ⚠️ **Also check `cfgignorelist.xml`.** If `Flaregun` (or the vanilla `Ammo_Flare*`
> types) appear in your mission's `db`/`cfgignorelist.xml`, the CE ignores them
> **entirely** — the gun never spawns *and* is deleted from players on load
> (`CHARACTER LOAD ERROR: Item 'Flaregun' ... it is ignored, throwing away all its
> content`). The ignore list overrides `types.xml`, so a high `nominal` won't help.
> Remove `Flaregun` from that file. RPT tell: `[CE][IgnoreList] "cfgignorelist.xml"
> :: loaded N types`.

> **No rebuild needed for any of this.** These are mission CE files, not the PBO —
> edit `FGAM_types.xml`, restart the server, done. (For changes to take effect on an
> already-populated map, wipe the CE storage — `storage_1/data/*` — so it repopulates;
> otherwise new counts fill in slowly via restock.)

**b) `mpmissions/<your.mission>/cfgeconomycore.xml`** — tell CE to load that file (this
keeps the vanilla `types.xml` untouched). Add inside `<economycore>`:
```xml
<ce folder="db">
    <file name="FGAM_types.xml" type="types"/>
</ce>
```
(Alternatively, paste the `FGAM_types.xml` entries straight into the mission's
`db/types.xml` and skip this step — but the separate file is cleaner to maintain.)

### 3. Tune behaviour & loot — server profile `config.json`
The main runtime config. **Editing loot or timers here needs only a server restart — no
rebuild.** Copy `ServerProfile/FlareGunAirdropMod/config.json` to:
```
<ServerProfileFolder>/FlareGunAirdropMod/config.json
```
It's plain JSON. The shipped file documents every key with `_comment_*` entries (the game
ignores unknown keys). Readable summary below — **JSON has no real comments, so don't put
`//` in the actual file; use the `_comment_*` style the shipped file uses:**
```jsonc
{
  "flare": {
    "minTriggerPitch": 75.0,        // degrees above horizontal needed to trigger (flat / indoors = nothing)
    "airdropSpawnHeight": 100.0,    // metres above the shooter the crate appears
    "airdropDescentSpeed": 6.0,     // metres/second it descends
    "airdropLifetime": 1800.0,      // seconds the landed crate stays (1800 = 30 min)
    "airdropContainerClass": "FGAM_AirdropContainer",
    "redZoneDelay": 300.0,          // Red flare: seconds before the gas zone appears
    "redZoneDuration": 1500.0       // Red flare: seconds the zone + its crate last
  },
  "loot_GREEN": {                   // one block per colour - exact DayZ class names
    "items": [ "KnifeHunting", "Hatchet", "HipPack_Green", "BandageDressing", "BandageDressing" ]
  }
  // ...loot_RED / loot_BLUE / loot_WHITE / loot_YELLOW / loot_BLACK / loot_ORANGE
}
```
Items that fit go inside the crate's cargo; overflow drops on the ground beside it.

| Key | Default | Description |
|-----|---------|-------------|
| `minTriggerPitch` | `30.0` | Degrees above horizontal the gun must be aimed to trigger. Flat / indoors = nothing. |
| `airdropSpawnHeight` | `100.0` | Metres above the shooter the crate spawns before descending |
| `airdropDescentSpeed` | `6.0` | Metres/second the crate falls |
| `airdropLifetime` | `1800.0` | Seconds the landed crate stays before despawn |
| `airdropContainerClass` | `"FGAM_AirdropContainer"` | Container class used for airdrops |
| `redZoneDelay` | `300.0` | Seconds before the Red toxic zone activates |
| `redZoneDuration` | `1500.0` | Seconds the Red zone + its crate last |
| `loot_<COLOUR>` | — | Items spawned in that colour's crate (exact class names) |

> Some keys exist but are **currently inert**, left from earlier iterations:
> `shotsPerState`, `redZoneRadius`, `minTriggerAltitude`, `maxTriggerRadius`, and the
> whole `spawnWeights` block. Editing them has no effect today.

### 4. (Optional) Test gear — give every player flares on spawn
Handy while testing. In the mission's `init.c`, inside `StartingEquipSetup(...)`:
```c
player.GetInventory().CreateInInventory("FlareGun");
player.GetInventory().CreateInInventory("FGAM_Mag_Red");
player.GetInventory().CreateInInventory("FGAM_Mag_Green");
// ...add whichever colours you want to test
```
Remove these lines for normal play so flares are found in the world instead.

### What you can't change without rebuilding the PBO
These live in `config.cpp` / the particle files **inside** the mod, so changing them means
a rebuild (`1_copy_to_P.bat` → `2_deploy.bat`): the flare classes & names, the
`cfgAmmoTypes` registry, flight ballistics (rise / fall / burn time on
`FGAM_Bullet_FlareBase`), the crate's cargo grid size, the flame/smoke `.ptc` particles,
the ammo box classes/textures, and how many rounds a box unpacks into (`value` on each
`FGAM_Box_<Colour>`'s `Resources` block in `config.cpp`, currently 1).

## File structure

```
. (repo root)
├── config.cpp                          — mod definition: ammo/magazine/weapon classes, cfgAmmoTypes
├── mod.cpp                             — Steam Workshop metadata
├── cfgeconomycore.xml                  — EXAMPLE mission CE file (shows the <ce> entry to copy)
├── README.md / INSTALL.md / LICENSE.md — this file / server install guide / license & credit terms
├── ADDING_FLARES.md                    — guide: new colour + crate loot + spawn locations + ammo box
├── TESTING.md                          — guide: switch local build vs. Steam Workshop for testing
├── 1_copy_to_P.bat / 2_deploy.bat      — build (pack + sign) / deploy to server + client
├── 3_switch_to_workshop.bat / 4_switch_to_local.bat — swap between Workshop + local build, see TESTING.md
├── make_flare.bat / make_flare.ps1     — generate a recoloured flare particle (.ptc)
├── make_box.bat / make_box.py          — generate an ammo box label texture (.paa)
├── FlareGunAirdropMod/                 — the actual PBO source (becomes @FlareGunAirdropMod)
│   ├── scripts/
│   │   ├── 3_Game/
│   │   │   └── FGAM_Particles.c        — registers the 7 coloured flare particles
│   │   ├── 4_World/
│   │   │   ├── FGAM_Config.c           — JSON config loader + defaults
│   │   │   ├── FGAM_AirdropManager.c   — event dispatcher (falling crate + toxic gas + despawn)
│   │   │   ├── FGAM_FlareGun.c         — server-side airdrop trigger (Weapon_Base.EEFired)
│   │   │   ├── FGAM_FlareVisuals.c     — coloured flare light + smoke (client visuals)
│   │   │   ├── FGAM_Magazines.c        — magazine script-class bindings
│   │   │   ├── FGAM_Boxes.c            — ammo box script-class bindings (unpack -> magazine)
│   │   │   ├── FGAM_ToxicArea.c        — toxic gas zone for the red flare
│   │   │   ├── FGAM_AirdropContainer.c — airdrop crate class
│   │   │   ├── FGAM_SpawnRegistry.c    — tracks live crate/zone positions so restarts don't leak them forever
│   │   │   └── FGAM_SpawnHandler.c     — spawn-context helpers (dormant; not wired up)
│   │   └── 5_Mission/
│   │       ├── FGAM_ServerInit.c       — server-side mission init hook
│   │       └── FGAM_StartingEquip.c    — placeholder (test gear lives in mission init.c)
│   ├── Graphics/
│   │   └── Particles/                  — our own recoloured flare .ptc files (7 colours)
│   └── data/
│       └── fgam_box_<colour>_co.paa    — ammo box label textures, generated by make_box.bat (7 colours)
├── db/                                 — EXAMPLE mission CE files, copy into your mission's db/
│   ├── FGAM_types.xml                  — CE spawn rules for the gun + 7 ammo boxes (ground loot) + 7 flare magazines (unpack-only)
│   ├── FGAM_spawnabletypes.xml         — CE attachment/cargo spawnable-types rules for the flare items
│   └── cfgeconomycore.xml              — EXAMPLE CE index file showing the <ce> entry to copy
└── ServerProfile/
    └── FlareGunAirdropMod/
        └── config.json                 — runtime config (loot, timers, trigger pitch)
```
