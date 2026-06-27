# Flare Gun Airdrop Mod (FGAM)

Fire a colored flare into the sky — trigger a unique airdrop or zone event.

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

**Adding a new flare colour** (plus its crate loot and where it spawns) is documented
step-by-step in [ADDING_FLARES.md](ADDING_FLARES.md). A short version of the colour
recipe is also in the header of `FGAM_FlareVisuals.c`.

free, with credit). FGAM extends `Flaregun_Base.chamberableFrom` so it stays

## Color → Event

| Color  | Event |
|--------|-------|
| 🔴 Red    | Toxic zone (ContaminatedArea_Dynamic) spawns after 5 min + military loot crate inside gas |
| 🟡 Yellow | NBC/CBRN protection kit (suit, mask, filters, antidotes) |
| 🟢 Green  | Survival kit (knife, axe, tent, basic meds) |
| 🔵 Blue   | Medical humanitarian drop (blood bags, surgical kits, antibiotics) |
| ⚪ White  | Provisions (food, water, purification tabs) |
| ⚫ Black  | Top-tier weapons & ammo — no light at night, thick black smoke only |
| 🟠 Orange | Construction tools, vehicle parts |

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

**a) `mpmissions/<your.mission>/db/FGAM_types.xml`** — copy it from this repo's `db/`.
It registers all 7 flare magazines. They spawn as **cargo inside containers**, not as
loose ground loot — note `count_in_map="0" count_in_cargo="1"`:
```xml
<type name="FGAM_Mag_Green">
    <nominal>8</nominal>      <!-- cap on how many exist (across all cargo) -->
    <lifetime>7200</lifetime>
    <restock>900</restock>
    <min>0</min>
    <quantmin>-1</quantmin>
    <quantmax>-1</quantmax>
    <cost>100</cost>
    <flags count_in_cargo="1" count_in_hoarder="0" count_in_map="0" count_in_player="0" crafted="0" deloot="0"/>
    <category name="weapons"/>
    <usage name="Hunting"/>
    <usage name="Civilian"/>
</type>
```
> **Why cargo, not ground loot:** the flare round model is awkward for CE to place on
> surfaces (vanilla ships flares at `nominal=0` for this reason). Forcing them onto the
> map (`count_in_map="1"`) makes CE retry placement hundreds of times per item —
> thousands of `is hard to place` / `exceeded max tests` log lines and real CE load.
> Spawning them as **cargo** (`count_in_map="0" count_in_cargo="1"`) skips surface
> placement entirely, so the spam is gone and they still appear as findable loot.

**a2) The container hosts — `cfgspawnabletypes.xml`.** Cargo items need a host container to
spawn inside. Add the `<cargo>` lines from [`db/FGAM_cfgspawnabletypes_cargo.xml`](db/FGAM_cfgspawnabletypes_cargo.xml)
into the matching `<type>` blocks of your mission's **base** `cfgspawnabletypes.xml` (a
separate file doesn't merge reliably). Default theme:
> - **AmmoBox** (Military) → Red, Yellow, Dark
> - **FirstAidKit** (Medic) → Blue
> - **DryBag_Green / ChildBag_Green / CoyoteBag_Brown** (Civilian) → Green, White
> - **Drivable car trunks** (CivilianSedan / Hatchback_02 / OffroadHatchback / Sedan_02 / Truck_01_Covered + variants) → Orange *(the vehicle-parts flare)*
>
> So you find flares by **opening those containers** at the matching locations. Tune the
> `chance=""` per `<cargo>` line for rarity. See [ADDING_FLARES.md](ADDING_FLARES.md) Part 3.

> **The flare gun spawns via your base `types.xml`, not this file.** `Flaregun` is a
> vanilla class already defined in the mission's `db/types.xml` at `nominal 0` (never
> world-spawned). A duplicate `Flaregun` entry in a *separate* CE file does **not**
> override it — the base definition wins. To make the gun spawn, **edit the `Flaregun`
> entry in your mission's main `db/types.xml`** (e.g. `<nominal>12</nominal>`,
> `<min>3</min>`); its vanilla `usage` already covers Military/Police/Medic/Firefighter/Coast.
> That's why `FGAM_types.xml` contains only the flare cartridges, not the gun.

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
    "minTriggerPitch": 30.0,        // degrees above horizontal needed to trigger (flat / indoors = nothing)
    "airdropSpawnHeight": 100.0,    // metres above the shooter the crate appears
    "airdropDescentSpeed": 6.0,     // metres/second it descends
    "airdropLifetime": 1800.0,      // seconds the landed crate stays (1800 = 30 min)
    "airdropContainerClass": "FGAM_AirdropContainer",
    "redZoneDelay": 300.0,          // Red flare: seconds before the gas zone appears
    "redZoneDuration": 1500.0       // Red flare: seconds the zone + its crate last
  },
  "loot_GREEN": {                   // one block per colour - exact DayZ class names
    "items": [ "KnifeHunting", "Hatchet", "Bandage", "Bandage", "TentDome" ]
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
`FGAM_Bullet_FlareBase`), the crate's cargo grid size, and the flame/smoke `.ptc` particles.

## File structure

```
FlareGunAirdropMod/
├── config.cpp                          — mod definition: ammo/magazine/weapon classes, cfgAmmoTypes
├── mod.cpp                             — Steam Workshop metadata
├── cfgeconomycore.xml                  — EXAMPLE mission CE file (shows the <ce> entry to copy)
├── ADDING_FLARES.md                    — guide: new colour + crate loot + spawn locations
├── 1_copy_to_P.bat / 2_deploy.bat      — build (pack + sign) / deploy to server + client
├── make_flare.bat / make_flare.ps1     — generate a recoloured flare particle (.ptc)
├── FlareGunAirdropMod/
│   ├── scripts/
│   │   ├── 3_Game/
│   │   │   └── FGAM_Particles.c        — registers the 7 coloured flare particles
│   │   ├── 4_World/
│   │   │   ├── FGAM_Config.c           — JSON config loader + defaults
│   │   │   ├── FGAM_AirdropManager.c   — event dispatcher (falling crate + toxic gas + despawn)
│   │   │   ├── FGAM_FlareGun.c         — server-side airdrop trigger (Weapon_Base.EEFired)
│   │   │   ├── FGAM_FlareVisuals.c     — coloured flare light + smoke (client visuals)
│   │   │   ├── FGAM_Magazines.c        — magazine script-class bindings
│   │   │   ├── FGAM_ToxicArea.c        — toxic gas zone for the red flare
│   │   │   ├── FGAM_AirdropContainer.c — airdrop crate class
│   │   │   └── FGAM_SpawnHandler.c     — spawn-context helpers (dormant; not wired up)
│   │   └── 5_Mission/
│   │       └── FGAM_StartingEquip.c    — placeholder (test gear lives in mission init.c)
│   └── Graphics/
│       └── Particles/                  — our own recoloured flare .ptc files (7 colours)
├── db/
│   └── FGAM_types.xml                   — CE spawn rules for the gun + all 7 flare magazines
└── ServerProfile/
    └── FlareGunAirdropMod/
        └── config.json                 — runtime config (loot, timers, trigger pitch)
```
