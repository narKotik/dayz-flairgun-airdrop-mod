// FGAM_FlareVisuals - FGAM's own coloured flares (light + coloured smoke trail).
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareVisuals.c
//
// credit): custom CfgAmmo bullet -> FlareSimulation subclass -> FlareLight subclass.
// This file is CLIENT-side visuals ONLY. The airdrop is triggered server-side from
// the weapon's OnFire (FGAM_FlareGun.c), because on a dedicated server the engine
// never runs this visual FlareSimulation - the colour key there is read from the
// fired ammo's class name, not from m_FGAM_Color here.
//
// ── HOW TO ADD A NEW COLOUR (recipe) ─────────────────────────────────────────
//   1. FGAM_FlareLight<Name>      : set SetAmbientColor/SetDiffuseColor (the light)
//   2. FGAM_FlareSimulation_<Name>: set m_ScriptedLight, m_ParticleId (burning core),
//                                   m_FGAM_SmokeId (coloured smoke trail)
//   3. config.cpp CfgAmmo  : FGAM_Bullet_Flare<Name> { SimulationScriptClass="FGAM_FlareSimulation_<Name>"; }
//   4. config.cpp cfgAmmoTypes : class AType_FGAM_Bullet_Flare<Name> { name="FGAM_Bullet_Flare<Name>"; }
//                            ^^ MANDATORY. Every cartridge ammo a magazine holds must be
//                            registered here or chambering CRASHES the game (engine access
//                            violation in WeaponChambering.GetCartridgeAtIndex). This was
//   5. config.cpp CfgMag   : FGAM_Mag_<Name> { ammo="FGAM_Bullet_Flare<Name>"; ... }
//   6. FGAM_Magazines.c    : class FGAM_Mag_<Name> : Ammunition_Base {};
//   7. config.cpp Flaregun_Base.chamberableFrom += "FGAM_Mag_<Name>"
//   8. FGAM_FlareGun.c FGAM_ColorFromAmmo(): add the "<name>" -> "<COLOUR>" mapping
//      (the substring must be present in the ammo class name, e.g. "white").
//   9. FGAM_AirdropManager / loot: handle the colour key.
// Brightness/size: edit FGAM_FlareLightBase. Flight speed/fall: the CfgAmmo bullet.

// ── Lights (colour + brightness) ─────────────────────────────────────────────
class FGAM_FlareLightBase : FlareLight
{
    void FGAM_FlareLightBase()
    {
        SetRadiusTo(140);     // vanilla 100
        SetBrightnessTo(16);  // vanilla 10
    }
}
class FGAM_FlareLightRed : FGAM_FlareLightBase
{ void FGAM_FlareLightRed() { SetAmbientColor(1.0, 0.25, 0.25); SetDiffuseColor(1.0, 0.2, 0.2); } }
class FGAM_FlareLightGreen : FGAM_FlareLightBase
{ void FGAM_FlareLightGreen() { SetAmbientColor(0.3, 1.0, 0.3); SetDiffuseColor(0.25, 1.0, 0.25); } }
class FGAM_FlareLightBlue : FGAM_FlareLightBase
{ void FGAM_FlareLightBlue() { SetAmbientColor(0.3, 0.4, 1.0); SetDiffuseColor(0.25, 0.35, 1.0); } }
class FGAM_FlareLightYellow : FGAM_FlareLightBase
{ void FGAM_FlareLightYellow() { SetAmbientColor(1.0, 0.85, 0.2); SetDiffuseColor(1.0, 0.8, 0.15); } }
class FGAM_FlareLightWhite : FGAM_FlareLightBase
{ void FGAM_FlareLightWhite() { SetAmbientColor(0.95, 0.97, 1.0); SetDiffuseColor(0.9, 0.9, 1.0); } }
class FGAM_FlareLightOrange : FGAM_FlareLightBase
{ void FGAM_FlareLightOrange() { SetAmbientColor(1.0, 0.5, 0.1); SetDiffuseColor(1.0, 0.45, 0.08); } }
class FGAM_FlareLightBlack : FGAM_FlareLightBase
{ void FGAM_FlareLightBlack() { SetRadiusTo(20); SetBrightnessTo(2); SetAmbientColor(0.3, 0.3, 0.3); SetDiffuseColor(0.3, 0.3, 0.3); } }

// ── Simulation base: coloured burning core + smoke trail (client visuals) ─────
class FGAM_FlareSimulationBase : FlareSimulation
{
    // This class is CLIENT-only visuals: the engine does not run it on a dedicated
    // server, so the airdrop is NOT triggered here - see FGAM_FlareGun.c (the weapon's
    // server-side OnFire). All we do here is colour + the coloured smoke trail.
    protected int      m_FGAM_SmokeId = 0;  // colour smoke particle id (subclass sets)
    protected string   m_FGAM_Color   = "";  // documentation only; trigger is weapon-side
    protected Entity   m_FGAM_Flare;
    protected Particle m_FGAM_Smoke;

    override void OnActivation(Entity flare)
    {
        super.OnActivation(flare); // light + burning core particle (client side)

        // Client only: play the coloured smoke and tie its lifetime to the flare so
        // it stops once the flare burns out instead of lingering on its own.
        if ((!g_Game.IsServer() || !g_Game.IsMultiplayer()) && m_FGAM_SmokeId != 0)
        {
            m_FGAM_Flare = flare;
            // PlayInWorld (NOT PlayOnObject): a world-space emitter we move onto the
            // flare every Simulate tick (see below). PlayOnObject attaches in the flare's
            // LOCAL space, so SetPosition(worldPos) would fling it thousands of metres away
            // and the big smoke vanishes - which is exactly what happened.
            m_FGAM_Smoke = ParticleManager.GetInstance().PlayInWorld(m_FGAM_SmokeId, flare.GetPosition());
            GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(FGAM_SmokeWatch, 1000, true);
        }
    }

    // Vanilla repositions the burning-core particle to the flare every tick (that is
    // what keeps it glued). A separate PlayOnObject smoke emitter does NOT track a fast
    // projectile - it lags at the launch point and looks detached. So we do the same as
    // vanilla: drive the smoke emitter onto the flare each tick. Puffs already emitted
    // stay put, forming a trail behind the moving flare head. Client-only (the smoke and
    // the simulation itself only exist on the client).
    override void Simulate(Entity flare)
    {
        super.Simulate(flare); // vanilla: moves the core particle + distance-light logic
        if (m_FGAM_Smoke && flare)
            m_FGAM_Smoke.SetPosition(flare.GetPosition());
    }

    // Object references null out when the engine deletes the flare; once that happens
    // we stop the smoke emitter and unschedule ourselves.
    void FGAM_SmokeWatch()
    {
        if (m_FGAM_Flare) return; // flare still alive - keep trailing
        if (m_FGAM_Smoke) m_FGAM_Smoke.Stop();
        GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).Remove(FGAM_SmokeWatch);
    }
}

class FGAM_FlareSimulation_Red : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Red() { m_ScriptedLight = FGAM_FlareLightRed; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE_RED; m_FGAM_SmokeId = ParticleList.GRENADE_M18_RED_LOOP; m_FGAM_Color = "RED"; } }
class FGAM_FlareSimulation_Green : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Green() { m_ScriptedLight = FGAM_FlareLightGreen; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE_GREEN; m_FGAM_SmokeId = ParticleList.GRENADE_M18_GREEN_LOOP; m_FGAM_Color = "GREEN"; } }
class FGAM_FlareSimulation_Blue : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Blue() { m_ScriptedLight = FGAM_FlareLightBlue; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE_BLUE; m_FGAM_SmokeId = ParticleList.GRENADE_M18_PURPLE_LOOP; m_FGAM_Color = "BLUE"; } }
class FGAM_FlareSimulation_Yellow : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Yellow() { m_ScriptedLight = FGAM_FlareLightYellow; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE; m_FGAM_SmokeId = ParticleList.GRENADE_M18_YELLOW_LOOP; m_FGAM_Color = "YELLOW"; } }
class FGAM_FlareSimulation_White : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_White() { m_ScriptedLight = FGAM_FlareLightWhite; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE; m_FGAM_SmokeId = ParticleList.GRENADE_M18_WHITE_LOOP; m_FGAM_Color = "WHITE"; } }
class FGAM_FlareSimulation_Orange : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Orange() { m_ScriptedLight = FGAM_FlareLightOrange; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE_RED; m_FGAM_SmokeId = ParticleList.GRENADE_M18_RED_LOOP; m_FGAM_Color = "ORANGE"; } }
class FGAM_FlareSimulation_Black : FGAM_FlareSimulationBase
{ void FGAM_FlareSimulation_Black() { m_ScriptedLight = FGAM_FlareLightBlack; m_ParticleId = ParticleList.FLAREPROJ_ACTIVATE; m_FGAM_SmokeId = ParticleList.GRENADE_M18_BLACK_LOOP; m_FGAM_Color = "BLACK"; } }
