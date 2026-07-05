#!/usr/bin/env python
"""
make_box.py - generate an FGAM ammo-box label texture (.paa) for one flare colour.

Called by make_box.bat. Unlike the old fgam_box_<colour>_co.paa files (which were
just the VANILLA 00Buck_10RoundBox shotgun-shell texture with a colour tint - hence
the leftover "SHOTGUN SHELLS" / "12 GA" / barcode text that has nothing to do with
flares), this draws an all-new label from scratch with Pillow, laid out on the exact
same UV islands as the vanilla texture (so it still fits the 00Buck_10RoundBox.p3d
model FGAM_Box_FlareBase uses - see config.cpp) but with our own wording:

    - Front face   : "SIGNAL FLARE" headline + a drawn flare icon + "<COLOR> FLARE"
                      flap band (upside-down, matching how the vanilla flap band is
                      oriented so it reads right-side-up once the flap folds over)
    - Back face    : plain colour fill, no safety text / barcode
    - Top band     : plain colour fill
    - Both end caps: FULLY coloured (not just a stripe like the vanilla-derived
                      textures) with "FLARE / <COLOR>" - end cap 1 text left-aligned,
                      end cap 2 text right-aligned, mirrored on purpose

UV island coordinates were measured from the existing fgam_box_*_co.paa (via
ImageToPAA.exe round-trip) - if you retexture a different model, re-measure them.

Output: a 1024x1024 PNG, then converted to .paa via ImageToPAA.exe, written to
FlareGunAirdropMod/data/fgam_box_<name>_co.paa
"""
import argparse
import os
import subprocess
import sys

from PIL import Image, ImageDraw, ImageFont, ImageFilter

SIZE = 1024
ROOT = os.path.dirname(os.path.abspath(__file__))
DATA_DIR = os.path.join(ROOT, "FlareGunAirdropMod", "data")
FONT_DIR = r"C:\Windows\Fonts"

# UV islands, measured on the 1024x1024 texture (see file header).
REGION_A = (25, 10, 649, 302)      # back panel - solid fill
REGION_B = (15, 331, 659, 604)     # top band - solid fill
REGION_C = (7, 610, 659, 997)      # front face (headline + flap band)
REGION_C_FLAP_Y = 850              # y where the front face's flap band starts
REGION_D = (700, 0, 1024, 430)     # rear/side panel - solid fill, no text
REGION_E = (652, 430, 1024, 690)   # end cap 1 - text LEFT
REGION_F = (630, 715, 1024, 1024)  # end cap 2 - text RIGHT

CARDBOARD = (225, 219, 205)
BG = (236, 236, 236)  # unused UV space, matches vanilla texture's padding colour


def font(name, size):
    return ImageFont.truetype(os.path.join(FONT_DIR, name), size)


def luminance(rgb):
    r, g, b = rgb
    return 0.299 * r + 0.587 * g + 0.114 * b


def ink_color(bg_rgb):
    """Black or white text, whichever contrasts with bg_rgb."""
    return (20, 20, 20) if luminance(bg_rgb) > 140 else (245, 245, 245)


def add_grime(img, seed_region, strength=14):
    """Cheap procedural wear: subtle noise + blur, kept mild so text stays legible."""
    import random
    random.seed(hash(seed_region) & 0xFFFFFFFF)
    noise = Image.effect_noise(img.size, strength).convert("L")
    noisy = Image.merge("RGB", [noise, noise, noise])
    return Image.blend(img, noisy, 0.05)


def fill_rect(draw, box, color):
    draw.rectangle(box, fill=color)


def draw_flare_icon(draw, cx, cy, r, color):
    """Simple flame/starburst icon standing in for the removed deer/crosshair art."""
    import math
    pts = []
    spikes = 8
    for i in range(spikes * 2):
        ang = math.pi * i / spikes
        rad = r if i % 2 == 0 else r * 0.45
        pts.append((cx + rad * math.sin(ang), cy - rad * math.cos(ang)))
    draw.polygon(pts, fill=color)
    draw.ellipse((cx - r * 0.28, cy - r * 0.28, cx + r * 0.28, cy + r * 0.28), fill=(255, 255, 255))


def centered_text(draw, cx, cy, text, fnt, fill, anchor="mm"):
    draw.text((cx, cy), text, font=fnt, fill=fill, anchor=anchor)


def build_texture(color_name, rgb):
    img = Image.new("RGB", (SIZE, SIZE), BG)
    draw = ImageDraw.Draw(img)
    label = color_name.upper()

    # -- Region A: back panel, solid colour ---------------------------------
    fill_rect(draw, REGION_A, rgb)

    # -- Region B: top band, cardboard -------------------------------------
    fill_rect(draw, REGION_B, CARDBOARD)

    # -- Region D: rear/side panel, solid colour, no text -------------------
    fill_rect(draw, REGION_D, rgb)

    # -- Region C: front face -------------------------------------------------
    cx0, cy0, cx1, cy1 = REGION_C
    fill_rect(draw, REGION_C, CARDBOARD)
    cx_mid = (cx0 + cx1) // 2

    icon_color = tuple(max(0, c - 40) for c in rgb)
    draw_flare_icon(draw, cx_mid, cy0 + 95, 70, icon_color)

    f_headline = font("impact.ttf", 46)
    f_sub = font("arialbd.ttf", 24)
    centered_text(draw, cx_mid, cy0 + 190, "SIGNAL FLARE", f_headline, (30, 30, 30))
    centered_text(draw, cx_mid, cy0 + 228, "AERIAL DISTRESS CARTRIDGE", f_sub, (60, 60, 60))

    # Flap band - drawn upside down (rotated 180) to match the vanilla flap's
    # orientation, so it reads right-side-up once the model's flap folds over.
    flap_h = cy1 - REGION_C_FLAP_Y
    flap_w = cx1 - cx0
    flap = Image.new("RGB", (flap_w, flap_h), rgb)
    fdraw = ImageDraw.Draw(flap)
    f_flap_big = font("impact.ttf", 54)
    f_flap_small = font("arialbd.ttf", 30)
    ink = ink_color(rgb)
    centered_text(fdraw, flap_w // 2, int(flap_h * 0.32), "FLARE", f_flap_big, ink)
    centered_text(fdraw, flap_w // 2, int(flap_h * 0.72), label, f_flap_small, ink)
    flap = flap.rotate(180)
    img.paste(flap, (cx0, REGION_C_FLAP_Y))

    # -- Region E: end cap 1, fully coloured. Both end caps are drawn upside
    # down (rotate 180, matching the flap-band convention above), which mirrors
    # left/right - so to land text on this cap's LEFT once on the model, we
    # anchor it to the RIGHT before the rotation.
    ex0, ey0, ex1, ey1 = REGION_E
    fill_rect(draw, REGION_E, rgb)
    ew, eh = ex1 - ex0, ey1 - ey0
    cap = Image.new("RGB", (ew, eh), rgb)
    cdraw = ImageDraw.Draw(cap)
    f_cap_big = font("impact.ttf", 46)
    f_cap_small = font("arialbd.ttf", 26)
    cdraw.text((ew - 24, eh // 2 - 34), "FLARE", font=f_cap_big, fill=ink, anchor="rm")
    cdraw.text((ew - 24, eh // 2 + 22), label, font=f_cap_small, fill=ink, anchor="rm")
    cap = cap.rotate(180)
    img.paste(cap, (ex0, ey0))

    # -- Region F: end cap 2, fully coloured, text RIGHT (mirror of cap 1) ---
    fx0, fy0, fx1, fy1 = REGION_F
    fill_rect(draw, REGION_F, rgb)
    fw, fh = fx1 - fx0, fy1 - fy0
    cap2 = Image.new("RGB", (fw, fh), rgb)
    c2draw = ImageDraw.Draw(cap2)
    c2draw.text((24, fh // 2 - 34), "FLARE", font=f_cap_big, fill=ink, anchor="lm")
    c2draw.text((24, fh // 2 + 22), label, font=f_cap_small, fill=ink, anchor="lm")
    cap2 = cap2.rotate(180)
    img.paste(cap2, (fx0, fy0))

    img = add_grime(img, color_name)
    return img


def find_image_to_paa():
    env = os.environ.get("FGAM_IMAGETOPAA")
    if env and os.path.isfile(env):
        return env
    candidates = []
    for drive in "CDEFGP":
        candidates.append(rf"{drive}:\Program Files (x86)\Steam\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe")
        candidates.append(rf"{drive}:\Installs\steam\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe")
        candidates.append(rf"{drive}:\SteamLibrary\steamapps\common\DayZ Tools\Bin\ImageToPAA\ImageToPAA.exe")
    for c in candidates:
        if os.path.isfile(c):
            return c
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("name", help="lowercase colour name, e.g. purple")
    ap.add_argument("r", type=float, help="0..255")
    ap.add_argument("g", type=float, help="0..255")
    ap.add_argument("b", type=float, help="0..255")
    ap.add_argument("--png-only", action="store_true", help="skip .paa conversion")
    args = ap.parse_args()

    safe = "".join(c for c in args.name.lower() if c.isalnum() or c == "_")
    if not safe:
        sys.exit("name must contain letters/numbers")
    rgb = (int(max(0, min(255, args.r))), int(max(0, min(255, args.g))), int(max(0, min(255, args.b))))

    os.makedirs(DATA_DIR, exist_ok=True)
    img = build_texture(safe, rgb)

    png_path = os.path.join(DATA_DIR, f"fgam_box_{safe}_co.png")
    img.save(png_path)
    print(f"Wrote {png_path}")

    if args.png_only:
        return

    exe = find_image_to_paa()
    if not exe:
        print("WARNING: ImageToPAA.exe not found - set FGAM_IMAGETOPAA to its path, "
              "or run with --png-only and convert manually (TexView.exe / ImageToPAA.exe).")
        return

    paa_path = os.path.join(DATA_DIR, f"fgam_box_{safe}_co.paa")
    subprocess.run([exe, png_path, paa_path], check=True)
    print(f"Wrote {paa_path}")
    os.remove(png_path)


if __name__ == "__main__":
    main()
