# Rebble Appstore listing — Pixel Constellations

Copy these fields into [dev-portal.rebble.io](https://dev-portal.rebble.io/submit).

| Field | Value |
| --- | --- |
| App name | Pixel Constellations |
| Type | Watchface |
| Category | Faces *(automatic)* |
| Website | *(optional)* |
| Source code | https://github.com/TechJeeper/Pixel-Constellations |
| `.pbw` | `build/PixelConstellations.pbw` |

## Description

Paste into each platform asset collection (max 1600 characters):

```
Pixel Constellations turns your wrist into a night sky.

Stars slowly draw pixelated lines between each other, forming constellations that spell out the digital time. Each minute the sky redraws — a quiet, animated connect-the-dots clock.

• Constellation digits that animate line-by-line
• Twinkling ambient starfield
• PixelLab moon, nebula, and sparkle accents
• Color themes: Classic, Gold, Cyan, Magenta, Aurora
• Adjustable draw speed, twinkle, and star density
• Optional date and blinking colon
• Clay settings on your phone

Color watches show the full theme palette; B&W watches keep the constellation time on a black sky.
```

*(~620 characters)*

## Release notes (v1.0.0)

```
Initial release: constellation time, PixelLab sky accents, five color themes, Clay settings.
```

## Screenshots

Exact platform sizes (no watch bezel). Regenerate with:

```powershell
python tools/generate_store_screenshots.py
```

| Platform | Size | Folder |
| --- | --- | --- |
| basalt (Pebble Time) | 144×168 | `store/screenshots/basalt/` |
| diorite (Pebble 2) | 144×168 | `store/screenshots/diorite/` |
| flint (Pebble 2 Duo) | 144×168 | `store/screenshots/flint/` |
| emery (Pebble Time 2) | 200×228 | `store/screenshots/emery/` |

Five shots per platform:

1. Classic · 10:42  
2. Gold · mid-draw (+date)  
3. Cyan · 3:08  
4. Magenta · full sky  
5. Aurora · nearly complete

## Listing checklist

- [ ] Screenshots uploaded per platform (up to 5)
- [ ] Description above on each asset collection
- [ ] Source URL set to the GitHub repo
- [ ] Publish `build/PixelConstellations.pbw` as release 1.0.0
