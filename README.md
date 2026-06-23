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

1. Player fires flare gun into the sky (must reach ≥50m above terrain). Each colour has its own coloured cartridge (Orange/White/Dark added via rvmat recolour). The in-air burning trace uses the nearest vanilla flare colour.
2. Server-side script reads the ammo color from the projectile class.
3. A big-cargo airdrop crate (`FGAM_AirdropContainer`) spawns ~100m above the flare's peak and descends gently to the ground (server-driven, visible to all players). All loot rides down **inside the crate's cargo**; anything that can't fit drops beside it.
4. Loot is filled from `config.json` inside the server profile folder.
5. The landed crate is automatically removed after `airdropLifetime` seconds (default 30 min).
6. Red flares instead spawn a working **toxic gas zone** (`ContaminatedArea_Static`) after `redZoneDelay`, with a military crate dropped into the gas. Both clear after `redZoneDuration`.

## Installation

### 1. Workshop / @mod folder
Copy `FlareGunAirdropMod/` into your DayZ server's `@FlareGunAirdropMod/` add-on folder.  
Add `-mod=@FlareGunAirdropMod` to your server start parameters.

### 2. Economy (types.xml)
Either:
- Copy the contents of `db/FGAM_types.xml` into your mission's `db/types.xml`, or
- Copy `db/FGAM_types.xml` into your mission's `db/` folder and add the `db/cfgeconomycore.xml` `<ce>` entry so CE loads it as a separate file (keeps vanilla `types.xml` untouched). This is what `2_deploy.bat` does.

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
| `airdropSpawnHeight` | `100.0` | Metres above the ground the crate spawns before descending |
| `airdropDescentSpeed` | `6.0` | Metres/second the crate falls from the sky |
| `airdropLifetime` | `1800.0` | Seconds the landed crate stays before it despawns (30 min) |
| `airdropContainerClass` | `"FGAM_AirdropContainer"` | DayZ class of airdrop container (mod's big-cargo crate) |
| `redZoneDelay` | `300.0` | Seconds before toxic zone activates (Red flare) |
| `redZoneRadius` | `50.0` | Radius of toxic zone in metres |
| `redZoneDuration` | `1500.0` | How long (seconds) the zone + crate last |

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
│           ├── FGAM_AirdropManager.c   — event dispatcher (falling crate + toxic gas + despawn)
│           ├── FGAM_FlareProjectile.c  — fires the airdrop event after delay
│           ├── FGAM_FlareGun.c         — condition degradation per shot
│           ├── FGAM_Magazines.c        — per-colour magazines + colour detection
│           └── FGAM_SpawnHandler.c     — spawn-context helpers (heli/train/beach weights)
├── db/
│   ├── FGAM_types.xml                   — CE types for gun + all 7 magazine colors
│   └── cfgeconomycore.xml              — tells CE to load FGAM_types.xml separately
└── ServerProfile/
    └── FlareGunAirdropMod/
        └── config.json                 — editable server config (loot, weights, timing)
```
