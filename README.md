# Pixel Constellations (Pebble)

A night-sky watchface where stars slowly draw pixelated lines between each other
to form constellations that spell out the digital time.

Pixel art accents (menu icon, sparkle star, crescent moon, nebula) were generated
with [PixelLab](https://pixellab.ai). Settings use **Clay** (`@rebble/clay`).

## Features

- Constellation digits animate line-by-line each minute
- Twinkling ambient starfield
- Optional PixelLab moon + nebula decorations
- Color themes: Classic, Gold, Cyan, Magenta, Aurora (B&W on Diorite)

## Settings (phone)

Open **Pixel Constellations → Settings** in the Pebble/Rebble app:

- Hour format / leading zero / date / blinking colon
- Color theme
- Moon & nebula toggles
- Background star density & twinkle strength
- Constellation line draw speed (slow → instant)

## Build

On Windows (Podman or Docker):

```powershell
npm install
./build.ps1
```

Output: `build/PixelConstellations.pbw`

```powershell
./build.ps1 -Phone 192.168.1.23
./build.ps1 -Zip
```

## CloudPebble

Import `dist/Pixel-Constellations-cloudpebble.zip` (or rebuild with
`python build-env/make_zip.py`).

## Layout

```
src/c/              constellation drawing + settings persistence
src/pkjs/           Clay configuration
resources/images/   PixelLab sprites + menu icon
```
