// FGAM_FlareVisuals - FGAM's own coloured flares (light + flame/smoke particle).
// Path: FlareGunAirdropMod/scripts/4_World/FGAM_FlareVisuals.c
//
// CLIENT-side visuals only. Two pieces per colour, both OUR OWN / vanilla - no
// third-party assets:
//   * m_ScriptedLight = a FlareLight subclass below (the dynamic light that tints
//     the surroundings).
//   * m_ParticleId    = one of our recoloured flare particles (FGAM_Particles.c),
//     which carries the coloured flame AND its smoke. The engine plays this as the
//     flare's "main fire" particle and repositions it onto the flare every tick
//     (vanilla FlareSimulation.Simulate -> FlareParticleUpdate), so it stays glued
//     to the flare - no separate smoke emitter, no manual tracking. That is what
//     keeps the smoke from detaching.
//
// The airdrop is triggered server-side from the weapon (FGAM_FlareGun.c); nothing
// here runs on a dedicated server.
//
// -- HOW TO ADD A NEW COLOUR (recipe) -----------------------------------------
//   1. FlareGunAirdropMod/Graphics/Particles/fgam_flare_<name>.ptc
//        Recolour from an existing one: change the "Color { t R G B ... }" arrays.
//   2. FGAM_Particles.c : register it -> ParticleList.FGAM_FLARE_<NAME>
//   3. FGAM_FlareLight<Name> below : SetAmbientColor/SetDiffuseColor (the light)
//   4. FGAM_FlareSimulation_<Name> below : m_ScriptedLight + m_ParticleId
//   5. config.cpp CfgAmmo : FGAM_Bullet_Flare<Name> { SimulationScriptClass="FGAM_FlareSimulation_<Name>"; }
//   6. config.cpp cfgAmmoTypes : class AType_FGAM_Bullet_Flare<Name> { name="FGAM_Bullet_Flare<Name>"; }
//        ^^ MANDATORY or chambering CRASHES (WeaponChambering.GetCartridgeAtIndex).
//   7. config.cpp CfgMag : FGAM_Mag_<Name> { ammo="FGAM_Bullet_Flare<Name>"; ... }
//   8. FGAM_Magazines.c : class FGAM_Mag_<Name> : Ammunition_Base {};
//   9. config.cpp Flaregun_Base.chamberableFrom += "FGAM_Mag_<Name>"
//  10. FGAM_FlareGun.c FGAM_ColorFromAmmo() : add the "<name>" -> "<COLOUR>" mapping
//  11. FGAM_AirdropManager / loot : handle the colour key
// Brightness/size of the LIGHT: FGAM_FlareLightBase. Flame/smoke look & colour: the .ptc.
// Flight speed/fall/burn time: FGAM_Bullet_FlareBase ballistics in config.cpp.

// -- Lights (colour + brightness) ---------------------------------------------
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

// -- Simulations: colour = light + our recoloured flare particle ---------------
// We only set the light + particle; vanilla FlareSimulation does the rest (creates
// them, keeps the particle glued to the flare). No overrides needed.
class FGAM_FlareSimulation_Red : FlareSimulation
{ void FGAM_FlareSimulation_Red() { m_ScriptedLight = FGAM_FlareLightRed; m_ParticleId = ParticleList.FGAM_FLARE_RED; } }
class FGAM_FlareSimulation_Green : FlareSimulation
{ void FGAM_FlareSimulation_Green() { m_ScriptedLight = FGAM_FlareLightGreen; m_ParticleId = ParticleList.FGAM_FLARE_GREEN; } }
class FGAM_FlareSimulation_Blue : FlareSimulation
{ void FGAM_FlareSimulation_Blue() { m_ScriptedLight = FGAM_FlareLightBlue; m_ParticleId = ParticleList.FGAM_FLARE_BLUE; } }
class FGAM_FlareSimulation_Yellow : FlareSimulation
{ void FGAM_FlareSimulation_Yellow() { m_ScriptedLight = FGAM_FlareLightYellow; m_ParticleId = ParticleList.FGAM_FLARE_YELLOW; } }
class FGAM_FlareSimulation_White : FlareSimulation
{ void FGAM_FlareSimulation_White() { m_ScriptedLight = FGAM_FlareLightWhite; m_ParticleId = ParticleList.FGAM_FLARE_WHITE; } }
class FGAM_FlareSimulation_Orange : FlareSimulation
{ void FGAM_FlareSimulation_Orange() { m_ScriptedLight = FGAM_FlareLightOrange; m_ParticleId = ParticleList.FGAM_FLARE_ORANGE; } }
class FGAM_FlareSimulation_Black : FlareSimulation
{ void FGAM_FlareSimulation_Black() { m_ScriptedLight = FGAM_FlareLightBlack; m_ParticleId = ParticleList.FGAM_FLARE_DARK; } }
