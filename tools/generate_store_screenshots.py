#!/usr/bin/env python3
"""Render Rebble App Store screenshots at exact platform resolutions.

No watch bezel — store requirement. Showcases themes + constellation time.
"""
from __future__ import annotations

import os
import random
import shutil
from pathlib import Path

from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "store" / "screenshots"
IMG = ROOT / "resources" / "images"

PLATFORMS = {
    "basalt": (144, 168, True),
    "diorite": (144, 168, False),
    "flint": (144, 168, False),
    "emery": (200, 228, True),
}

# Digit graphs on a 5×7 grid (matches src/c/constellation.c).
DIGITS: dict[int, tuple[list[tuple[int, int]], list[tuple[int, int]]]] = {
    0: (
        [(1, 0), (3, 0), (4, 2), (4, 4), (3, 6), (1, 6), (0, 4), (0, 2)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 0)],
    ),
    1: (
        [(1, 1), (2, 0), (2, 6), (1, 6), (3, 6)],
        [(0, 1), (1, 2), (2, 3), (2, 4)],
    ),
    2: (
        [(0, 1), (1, 0), (3, 0), (4, 1), (4, 2), (0, 5), (0, 6), (4, 6)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7)],
    ),
    3: (
        [(0, 1), (1, 0), (3, 0), (4, 1), (4, 2), (2, 3), (4, 4), (4, 5), (3, 6), (1, 6), (0, 5)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9), (9, 10)],
    ),
    4: (
        [(0, 0), (0, 3), (4, 3), (3, 0), (3, 6)],
        [(0, 1), (1, 2), (3, 4), (2, 3)],
    ),
    5: (
        [(4, 0), (0, 0), (0, 3), (3, 3), (4, 4), (4, 5), (3, 6), (1, 6), (0, 5)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8)],
    ),
    6: (
        [(3, 0), (1, 0), (0, 2), (0, 4), (1, 6), (3, 6), (4, 5), (4, 4), (3, 3), (0, 3)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 8), (8, 9)],
    ),
    7: (
        [(0, 0), (4, 0), (2, 3), (1, 6)],
        [(0, 1), (1, 2), (2, 3)],
    ),
    8: (
        [(1, 0), (3, 0), (4, 2), (3, 3), (1, 3), (0, 2), (4, 4), (3, 6), (1, 6), (0, 4)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 0), (3, 6), (6, 7), (7, 8), (8, 9), (9, 4)],
    ),
    9: (
        [(1, 3), (3, 3), (4, 1), (3, 0), (1, 0), (0, 1), (0, 2), (4, 3), (3, 6), (1, 6)],
        [(0, 1), (1, 2), (2, 3), (3, 4), (4, 5), (5, 6), (6, 0), (1, 7), (7, 8), (8, 9)],
    ),
}

THEMES = {
    "classic": {
        "bg": (0x0A, 0x0A, 0x1E),
        "star": (255, 255, 255),
        "star_dim": (0x60, 0x60, 0x80),
        "line": (0xA0, 0xC0, 0xE0),
        "accent": (0xFF, 0xE8, 0xA0),
        "date": (0x80, 0x80, 0xA0),
    },
    "gold": {
        "bg": (0x12, 0x0C, 0x08),
        "star": (0xFF, 0xE8, 0xA0),
        "star_dim": (0xA0, 0x80, 0x40),
        "line": (0xD0, 0xA0, 0x50),
        "accent": (0xFF, 0xD0, 0x70),
        "date": (0xC0, 0xA0, 0x60),
    },
    "cyan": {
        "bg": (0x04, 0x10, 0x18),
        "star": (0xE0, 0xFF, 0xFF),
        "star_dim": (0x40, 0x80, 0xA0),
        "line": (0x40, 0xC0, 0xE0),
        "accent": (0x80, 0xE8, 0xFF),
        "date": (0x70, 0xA0, 0xB0),
    },
    "magenta": {
        "bg": (0x10, 0x08, 0x18),
        "star": (0xFF, 0xE0, 0xF0),
        "star_dim": (0x80, 0x40, 0x60),
        "line": (0xE0, 0x60, 0xC0),
        "accent": (0xFF, 0x90, 0xD0),
        "date": (0xB0, 0x70, 0x90),
    },
    "aurora": {
        "bg": (0x06, 0x12, 0x10),
        "star": (0xE8, 0xFF, 0xF0),
        "star_dim": (0x40, 0x80, 0x60),
        "line": (0x40, 0xE0, 0xA0),
        "accent": (0x70, 0xA0, 0xE0),
        "date": (0x70, 0xB0, 0x90),
    },
}

BW = {
    "bg": (0, 0, 0),
    "star": (255, 255, 255),
    "star_dim": (180, 180, 180),
    "line": (255, 255, 255),
    "accent": (255, 255, 255),
    "date": (200, 200, 200),
}

SCENES = [
    {
        "file": "01-classic-1042.png",
        "theme": "classic",
        "time": (10, 42),
        "show_date": False,
        "show_moon": True,
        "show_nebula": True,
        "progress": 1.0,
    },
    {
        "file": "02-gold-drawing.png",
        "theme": "gold",
        "time": (22, 15),
        "show_date": True,
        "show_moon": True,
        "show_nebula": False,
        "progress": 0.55,
    },
    {
        "file": "03-cyan-night.png",
        "theme": "cyan",
        "time": (3, 8),
        "show_date": False,
        "show_moon": False,
        "show_nebula": True,
        "progress": 1.0,
        "leading_blank": True,
    },
    {
        "file": "04-magenta-full.png",
        "theme": "magenta",
        "time": (18, 36),
        "show_date": True,
        "show_moon": True,
        "show_nebula": True,
        "progress": 1.0,
    },
    {
        "file": "05-aurora-dawn.png",
        "theme": "aurora",
        "time": (7, 59),
        "show_date": False,
        "show_moon": True,
        "show_nebula": True,
        "progress": 0.82,
    },
]


def font(size: int) -> ImageFont.ImageFont:
    for path in (
        "C:/Windows/Fonts/segoeui.ttf",
        "C:/Windows/Fonts/arial.ttf",
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
    ):
        if os.path.isfile(path):
            return ImageFont.truetype(path, size)
    return ImageFont.load_default()


def load_sprite(name: str) -> Image.Image | None:
    path = IMG / name
    if not path.exists():
        return None
    return Image.open(path).convert("RGBA")


def to_bw(src: Image.Image) -> Image.Image:
    rgba = src.convert("RGBA")
    out = Image.new("RGBA", rgba.size, (0, 0, 0, 0))
    px, opx = rgba.load(), out.load()
    w, h = rgba.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a < 32:
                continue
            lum = 0.299 * r + 0.587 * g + 0.114 * b
            opx[x, y] = (255, 255, 255, a) if lum >= 70 else (0, 0, 0, a)
    return out


def draw_star(draw: ImageDraw.ImageDraw, x: int, y: int, color, bright: bool = False) -> None:
    draw.rectangle((x, y, x + 1, y + 1), fill=color)
    if bright:
        draw.point((x - 1, y), fill=color)
        draw.point((x + 2, y), fill=color)
        draw.point((x, y - 1), fill=color)
        draw.point((x, y + 2), fill=color)


def ambient_stars(draw: ImageDraw.ImageDraw, w: int, h: int, theme: dict, seed: int, dense: int = 28) -> None:
    rng = random.Random(seed)
    for _ in range(dense):
        x = rng.randrange(w)
        y = rng.randrange(h)
        bright = rng.random() < 0.25
        c = theme["star"] if bright else theme["star_dim"]
        draw.point((x, y), fill=c)


def digit_list(hour: int, minute: int, leading_blank: bool) -> list[int]:
    h10, h1 = hour // 10, hour % 10
    m10, m1 = minute // 10, minute % 10
    first = -1 if leading_blank and h10 == 0 else h10
    return [first, h1, m10, m1]


def layout(w: int, h: int, blank_leading: bool):
    digit_w = 36 if w >= 180 else 28
    gap = 6 if w >= 180 else 4
    colon_w = 10 if w >= 180 else 8
    digits = 3 if blank_leading else 4
    gaps = 2 if blank_leading else 3
    total = digit_w * digits + gap * gaps + colon_w
    start_x = (w - total) // 2
    y = 52 if h >= 200 else 40
    cell = 7 if w >= 180 else 5
    return digit_w, gap, colon_w, start_x, y, cell


def digit_origin(index: int, blank_leading: bool, digit_w: int, gap: int, colon_w: int, start_x: int, y: int):
    x = start_x
    if blank_leading:
        if index == 2:
            x += digit_w + gap + colon_w + gap
        elif index == 3:
            x += digit_w + gap + colon_w + gap + digit_w + gap
    elif index >= 2:
        x += digit_w * 2 + gap + colon_w + gap
        x += (index - 2) * (digit_w + gap)
    else:
        x += index * (digit_w + gap)
    return x, y


def draw_constellation_time(
    draw: ImageDraw.ImageDraw,
    w: int,
    h: int,
    theme: dict,
    hour: int,
    minute: int,
    progress: float,
    leading_blank: bool,
) -> None:
    digits = digit_list(hour, minute, leading_blank)
    blank = digits[0] < 0
    digit_w, gap, colon_w, start_x, y, cell = layout(w, h, blank)

    edges: list[tuple[tuple[int, int], tuple[int, int]]] = []
    nodes_by_digit: list[list[tuple[int, int]]] = []

    for d_i, dig in enumerate(digits):
        if dig < 0:
            nodes_by_digit.append([])
            continue
        nodes, edge_ix = DIGITS[dig]
        ox, oy = digit_origin(d_i, blank, digit_w, gap, colon_w, start_x, y)
        mapped = [(ox + nx * cell, oy + ny * cell) for nx, ny in nodes]
        nodes_by_digit.append(mapped)
        for a, b in edge_ix:
            edges.append((mapped[a], mapped[b]))

    total = max(1, len(edges))
    drawn = progress * total
    full = int(drawn)
    frac = drawn - full

    for i, (a, b) in enumerate(edges):
        if i < full:
            draw.line([a, b], fill=theme["line"], width=1)
        elif i == full and frac > 0:
            x = int(a[0] + (b[0] - a[0]) * frac)
            y2 = int(a[1] + (b[1] - a[1]) * frac)
            draw.line([a, (x, y2)], fill=theme["line"], width=1)

    # Reveal nodes for digits that have started drawing.
    edge_base = 0
    for d_i, dig in enumerate(digits):
        if dig < 0:
            continue
        nodes, edge_ix = DIGITS[dig]
        count = len(edge_ix)
        visible = progress >= 1.0 or full >= edge_base or (full == 0 and edge_base == 0)
        if full >= edge_base or (edge_base <= full < edge_base + count) or edge_base == 0:
            visible = True
        if visible:
            for n, (px, py) in enumerate(nodes_by_digit[d_i]):
                bright = (n % 3) != 1
                draw_star(draw, px, py, theme["star"] if bright else theme["accent"], bright)
        edge_base += count

    hour_digits = 1 if blank else 2
    cx = start_x + digit_w * hour_digits + gap + colon_w // 2
    draw_star(draw, cx, y + 2 * cell, theme["accent"], True)
    draw_star(draw, cx, y + 4 * cell, theme["accent"], True)


def render(w: int, h: int, color: bool, scene: dict) -> Image.Image:
    theme = THEMES[scene["theme"]] if color else BW
    img = Image.new("RGB", (w, h), theme["bg"])
    draw = ImageDraw.Draw(img)

    ambient_stars(draw, w, h, theme, seed=hash(scene["file"]) & 0xFFFF, dense=32 if w < 180 else 44)

    moon = load_sprite("moon.png")
    nebula = load_sprite("nebula.png")
    sparkle = load_sprite("star.png")
    if not color:
        if moon:
            moon = to_bw(moon)
        if nebula:
            nebula = to_bw(nebula)
        if sparkle:
            sparkle = to_bw(sparkle)

    if scene.get("show_nebula") and nebula:
        img.paste(nebula, (w - nebula.size[0] + 4, -4), nebula)
    if scene.get("show_moon") and moon:
        img.paste(moon, (8, 8), moon)
    if sparkle:
        img.paste(sparkle, (w - sparkle.size[0] - 12, h // 3), sparkle)

    hour, minute = scene["time"]
    draw_constellation_time(
        draw,
        w,
        h,
        theme,
        hour,
        minute,
        scene.get("progress", 1.0),
        scene.get("leading_blank", False),
    )

    if scene.get("show_date"):
        fnt = font(max(11, w // 13))
        text = "Mon 2 Nov"
        bbox = draw.textbbox((0, 0), text, font=fnt)
        tw = bbox[2] - bbox[0]
        draw.text(((w - tw) // 2, h - 22), text, font=fnt, fill=theme["date"])

    return img


def main() -> None:
    if OUT.exists():
        shutil.rmtree(OUT)
    for plat, (w, h, color) in PLATFORMS.items():
        dest = OUT / plat
        dest.mkdir(parents=True, exist_ok=True)
        for scene in SCENES:
            shot = render(w, h, color, scene)
            path = dest / scene["file"]
            shot.save(path, "PNG", optimize=True)
            print(f"wrote {path.relative_to(ROOT)} ({w}x{h})")
    print("Done:", OUT)


if __name__ == "__main__":
    main()
