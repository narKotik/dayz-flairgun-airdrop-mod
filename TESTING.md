# Testing: local build vs. Steam Workshop

Both the local DayZServer and DayZ client load the mod from the same fixed path,
`@FlareGunAirdropMod` in their install root. `3_switch_to_workshop.bat` /
`4_switch_to_local.bat` swap what that folder actually points to, so the
`-mod=` launch parameter never needs to change.

| Mode | `@FlareGunAirdropMod` is | Use it for |
|------|--------------------------|------------|
| **Local** | A real copy of `D:\Projects\DayZMods\@FlareGunAirdropMod` (your build output), refreshed by `2_deploy.bat` | Testing uncommitted / unpublished changes before pushing a Workshop update |
| **Workshop** | A directory junction to `steamapps\workshop\content\221100\3758787089` (the published item) | Confirming subscribers get a working mod — same bits they download |

## Switching

- **`3_switch_to_workshop.bat`** — points server + client at the published Workshop
  item. Errors out if you're not subscribed (item folder missing).
- **`4_switch_to_local.bat`** — points server + client back at your local build and
  redeploys it (`2_deploy.bat`). Errors out if you haven't run `1_copy_to_P.bat` yet.

Either script is safe to re-run from either state — each one removes whatever's
currently at `@FlareGunAirdropMod` (a plain folder or a junction) before replacing it.

## Protecting the private key

The signing key pair lives at `D:\Projects\DayZMods\@FlareGunAirdropMod\keys\`:

- `FGAM_v1.biprivatekey` — **never leaves this folder.** None of the build/deploy/
  switch scripts read, copy, or delete anything under `%BUILD%\keys` except to copy
  the *public* `.bikey` out — check the `copy` lines in `2_deploy.bat` if you ever
  extend it. Losing this file means you can't sign an update to the same Workshop
  listing; regenerating it breaks the mod for every server that already whitelisted
  the old key (see [PUBLISHING.md](PUBLISHING.md)).
- Switching modes only touches `@FlareGunAirdropMod` under the **server/client**
  install roots (`steamapps\common\...`), never `D:\Projects\DayZMods\...`.
- The Workshop item's own `keys\` folder only ever contains the public `.bikey`
  (that's all Steam needs to distribute) — there's no private key to lose there either.

## Everyday flow

1. Edit the mod, `1_copy_to_P.bat` to rebuild.
2. `4_switch_to_local.bat` (or just `2_deploy.bat` if you're already in local mode)
   to deploy and test.
3. Happy with it? Publish the update via DayZ Tools (see [PUBLISHING.md](PUBLISHING.md))
   using the **same** signing key.
4. `3_switch_to_workshop.bat` to confirm the live Workshop version behaves the same
   for a real subscriber.
