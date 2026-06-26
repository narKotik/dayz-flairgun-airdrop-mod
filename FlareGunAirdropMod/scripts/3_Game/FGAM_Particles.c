// FGAM_Particles - registers FGAM's own coloured flare particles.
// Path: FlareGunAirdropMod/scripts/3_Game/FGAM_Particles.c
//
// These .ptc files live in FlareGunAirdropMod/Graphics/Particles/ and are OUR OWN
// (recoloured from the vanilla flare particle - no third-party assets). Registering
// here gives each a ParticleList id we can use as a flare's core particle, which the
// engine keeps glued to the flare automatically (see FGAM_FlareVisuals.c).
modded class ParticleList
{
    static const int FGAM_FLARE_RED    = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_red");
    static const int FGAM_FLARE_GREEN  = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_green");
    static const int FGAM_FLARE_BLUE   = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_blue");
    static const int FGAM_FLARE_YELLOW = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_yellow");
    static const int FGAM_FLARE_WHITE  = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_white");
    static const int FGAM_FLARE_ORANGE = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_orange");
    static const int FGAM_FLARE_DARK   = RegisterParticle("FlareGunAirdropMod/Graphics/Particles/", "fgam_flare_dark");
}
