# make_flare.ps1 - generate a recoloured flare particle (.ptc) for FGAM.
# Called by make_flare.bat. Recolours an existing FGAM flare particle by replacing
# every "Color { time R G B ... }" keyframe's RGB with the colour you pass, keeping
# all timing/alpha/size/emitter structure intact (so it stays a valid flare particle).
#
# Output: FlareGunAirdropMod/Graphics/Particles/fgam_flare_<name>.ptc
param(
    [Parameter(Mandatory=$true)][string]$Name,
    [Parameter(Mandatory=$true)][string]$R,
    [Parameter(Mandatory=$true)][string]$G,
    [Parameter(Mandatory=$true)][string]$B
)

$ErrorActionPreference = "Stop"
$inv = [System.Globalization.CultureInfo]::InvariantCulture

# Parse colour channels invariantly (so a comma-locale machine still reads "0.6" right)
function ParseChannel([string]$v, [string]$label) {
    $out = 0.0
    if (-not [double]::TryParse($v, [Globalization.NumberStyles]::Float, $inv, [ref]$out)) {
        throw "$label must be a number 0..1 (got '$v')"
    }
    if ($out -lt 0 -or $out -gt 1) { Write-Host "WARNING: $label = $out is outside 0..1 (will be clamped by the engine)." }
    return $out
}
$rv = ParseChannel $R "R"; $gv = ParseChannel $G "G"; $bv = ParseChannel $B "B"

# Sanitise the colour name -> file-safe, lowercase
$safe = ($Name.ToLower() -replace '[^a-z0-9_]', '')
if ($safe -eq "") { throw "Name must contain letters/numbers (got '$Name')" }

$particleDir = Join-Path $PSScriptRoot "FlareGunAirdropMod\Graphics\Particles"
# Use an existing FGAM particle as the structural template (white = neutral base).
$template = Join-Path $particleDir "fgam_flare_white.ptc"
if (-not (Test-Path $template)) {
    # fall back to any existing fgam_flare_*.ptc
    $template = Get-ChildItem $particleDir -Filter "fgam_flare_*.ptc" -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
}
if (-not $template -or -not (Test-Path $template)) {
    throw "No template particle found in $particleDir (expected fgam_flare_white.ptc)."
}

$rs = $rv.ToString('0.####', $inv); $gs = $gv.ToString('0.####', $inv); $bs = $bv.ToString('0.####', $inv)
$src = Get-Content $template -Raw

# Replace each "Color { ... }" block: keyframes are groups of 4 (time R G B);
# keep each time, overwrite the RGB.
$out = [regex]::Replace($src, 'Color\s*\{([^}]*)\}', {
    param($m)
    $nums = ($m.Groups[1].Value -split '\s+') | Where-Object { $_ -ne '' }
    $sb = New-Object System.Collections.Generic.List[string]
    for ($i = 0; $i -lt $nums.Count; $i += 4) {
        $sb.Add($nums[$i]); $sb.Add($rs); $sb.Add($gs); $sb.Add($bs)
    }
    "Color {`n    " + ($sb -join ' ') + "`n   }"
})

$outPath = Join-Path $particleDir "fgam_flare_$safe.ptc"
[System.IO.File]::WriteAllText($outPath, $out)

Write-Host ""
Write-Host "Created: $outPath" -ForegroundColor Green
Write-Host "Colour : R=$rs G=$gs B=$bs"
Write-Host ""
Write-Host "Next steps (see ADDING_FLARES.md):"
Write-Host "  - Register it in FGAM_Particles.c:"
Write-Host "      static const int FGAM_FLARE_$($safe.ToUpper()) = RegisterParticle(`"FlareGunAirdropMod/Graphics/Particles/`", `"fgam_flare_$safe`");"
Write-Host "  - Add the light + simulation in FGAM_FlareVisuals.c, then the ammo/magazine"
Write-Host "    entries in config.cpp (including the cfgAmmoTypes line!)."
Write-Host "  - Rebuild: 1_copy_to_P.bat  then  2_deploy.bat"
