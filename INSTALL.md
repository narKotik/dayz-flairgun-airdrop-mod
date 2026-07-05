# Installing FGAM on your server (Steam Workshop)

For server owners who want to run this mod — not for people building it from source.
If you're building/modifying the mod itself, see [README.md](README.md) and
[ADDING_FLARES.md](ADDING_FLARES.md) instead.

**No mod dependencies.** FGAM only needs vanilla DayZ.

## 1. Subscribe & load the mod

1. Subscribe to the mod on the Steam Workshop (both **server** and every **client**
   need it — like any DayZ mod).
2. Add it to your server's start parameters:
   ```
   -mod=@FlareGunAirdropMod
   ```
   (Combine with your other mods with semicolons, e.g. `-mod=@CF;@FlareGunAirdropMod`.
   Load order relative to other mods does not matter.)
3. If your server verifies mod signatures, copy the mod's public key into your
   server's `keys/` folder:
   ```
   @FlareGunAirdropMod/keys/FGAM_v1.bikey  ->  <server>/keys/FGAM_v1.bikey
   ```
   (Steam Workshop subscriptions usually place this automatically — check `<server>/keys/`
   if the mod fails to load with a signature error.)

## 2. Make the gun & flares actually spawn (mission CE files)

This step is **not automatic** — it's true for any mod that adds loot/spawns, not
specific to FGAM. You're editing your **mission** folder (e.g.
`mpmissions/dayzOffline.chernarusplus`), not the mod itself.

1. Copy `db/FGAM_types.xml` from this mod into your mission's `db/` folder.
2. In your mission's `cfgeconomycore.xml`, add inside `<economycore>`:
   ```xml
   <ce folder="db">
       <file name="FGAM_types.xml" type="types"/>
   </ce>
   ```
   Get the filename exactly right — a typo fails silently (CE logs
   `Failed to read types file 'db/FGAM_types.xml'` and the type is never registered).
3. **Make the flare gun spawn**: edit the existing `Flaregun` entry in your mission's
   main `db/types.xml` (it's a vanilla item, usually `nominal="0"` = never spawns) —
   e.g. set `<nominal>12</nominal>`, `<min>3</min>`.
4. **Check `db/cfgignorelist.xml`.** If `Flaregun` or `Ammo_Flare*` appear there, the
   CE ignores them entirely regardless of `nominal` — remove them from that file if
   present.
5. Restart the server. New spawns fill in gradually via restock, not instantly — if
   you want them live immediately on an already-populated map, wipe that map's CE
   storage (`storage_1/data/*`) so it repopulates from scratch.

## 3. Configure loot & timers (optional, no rebuild needed)

Copy `ServerProfile/FlareGunAirdropMod/config.json` from this mod to:
```
<ServerProfileFolder>/FlareGunAirdropMod/config.json
```
Edit loot tables, airdrop timers, and trigger sensitivity there — it's plain JSON,
documented inline. Changes need only a server restart, no PBO rebuild. Full key
reference is in [README.md](README.md#3-tune-behaviour--loot--server-profile-configjson).

## 4. Verify it works

- Fire a flare gun **aimed up into open sky** (not flat, not indoors) → a crate should
  descend nearby within a few seconds.
- Fire **Red** → a toxic gas zone should appear ~5 minutes later (default
  `redZoneDelay`) with a loot crate inside it.
- If nothing happens, check your server RPT log for CE loading errors (see step 2) —
  that's the most common cause of "the gun doesn't spawn" or "flares don't drop loot."

## License

Free to use on your server, and free to modify/reskin for your own server, as long as
you credit the original author — see [LICENSE.md](LICENSE.md).
