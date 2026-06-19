# Flare Gun Airdrop Mod (FGAM)

Fire a colored flare into the sky — trigger a unique airdrop or zone event.

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

1. Player fires flare gun into the sky (must reach ≥50m above terrain).
2. Server-side script reads the ammo color from the projectile class.
3. Airdrop container (`SeaChest` by default) spawns 100m above the flare's peak position and falls via DayZ physics.
4. Loot is filled from `config.json` inside the server profile folder.

## Installation

### 1. Workshop / @mod folder
Copy `FlareGunAirdropMod/` into your DayZ server's `@FlareGunAirdropMod/` add-on folder.  
Add `-mod=@FlareGunAirdropMod` to your server start parameters.

### 2. Economy (types.xml)
Either:
- Copy the contents of `db/types.xml` into your mission's `db/types.xml`, or
- Copy `db/cfgeconomycore.xml` entries to tell CE to load `FGAM_types.xml` as a separate file.

### 3. Server config (config.json)
Copy `ServerProfile/FlareGunAirdropMod/config.json` to:
```
<ServerProfileFolder>/FlareGunAirdropMod/config.json
```
Edit loot tables, weights, and timing to taste.

## Configuration reference (`config.json`)

### `flare` section
| Key | Default | Description |
|-----|---------|-------------|
| `shotsPerState` | `1` | Shots per health tier before degrading |
| `minTriggerAltitude` | `50.0` | Metres above terrain required to trigger event |
| `airdropSpawnHeight` | `100.0` | Metres above impact the crate spawns |
| `airdropContainerClass` | `"SeaChest"` | DayZ class of airdrop container |
| `redZoneDelay` | `300.0` | Seconds before toxic zone activates (Red flare) |
| `redZoneRadius` | `50.0` | Radius of toxic zone in metres |
| `redZoneDuration` | `600.0` | How long (seconds) the zone lasts |

### `loot_<COLOR>` sections
Array of DayZ class name strings. Items are placed into the container; extras drop nearby.

### `spawnWeights`
Weighted probability maps for each spawn location type (`helicrash`, `train`, `beach`).
Beach spawns only produce nearly-destroyed guns (1 shot left).

## Flare gun spawn locations

| Location | Condition | Notes |
|----------|-----------|-------|
| Helicrash | Normal CE condition | Spawns with random colored magazine (weighted) |
| Train / railway | Normal CE condition | More tactical/supply-oriented weights |
| Wrecked boats (beach) | Badly Damaged (1 shot) | Survival-focused colors only |
| Standalone magazine | Normal | Any color can spawn separately as loot |

## File structure

```
FlareGunAirdropMod/
├── config.cpp                          — mod definition, ammo/magazine/weapon classes
├── mod.cpp                             — Steam Workshop metadata
├── FlareGunAirdropMod/
│   └── scripts/
│       └── 4_World/
│           ├── FGAM_Config.c           — JSON config loader + defaults
│           ├── FGAM_AirdropManager.c   — event dispatcher (drop + toxic zone)
│           ├── FGAM_FlareProjectile.c  — modded FlareSimulation, fires event on impact
│           ├── FGAM_FlareGun.c         — repair block + condition degradation per shot
│           └── FGAM_SpawnHandler.c     — post-spawn mutation (beach condition, magazine)
├── db/
│   ├── types.xml                       — CE types for gun + all 7 magazine colors
│   └── cfgeconomycore.xml              — tells CE to load FGAM_types.xml separately
└── ServerProfile/
    └── FlareGunAirdropMod/
        └── config.json                 — editable server config (loot, weights, timing)
```
