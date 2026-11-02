#pragma once

#include <pebble.h>
#include <stdbool.h>

typedef enum {
  TIME_FMT_SYSTEM = 0,
  TIME_FMT_12H,
  TIME_FMT_24H
} TimeFormat;

typedef enum {
  THEME_CLASSIC = 0,
  THEME_GOLD,
  THEME_CYAN,
  THEME_MAGENTA,
  THEME_AURORA,
  THEME_COUNT
} ColorTheme;

typedef enum {
  ANIM_SLOW = 0,
  ANIM_NORMAL,
  ANIM_FAST,
  ANIM_INSTANT,
  ANIM_COUNT
} AnimSpeed;

typedef struct {
  TimeFormat time_format;
  bool leading_zero;
  bool show_date;
  bool show_seconds;
  bool show_moon;
  bool show_nebula;
  ColorTheme color_theme;
  AnimSpeed anim_speed;
  int twinkle;       // 0..2 soft / medium / strong
  int ambient_stars; // 0..2 sparse / medium / dense
} FaceSettings;

typedef void (*SettingsChangedFn)(void);

void settings_init(void);
void settings_deinit(void);
void settings_set_changed_callback(SettingsChangedFn cb);
const FaceSettings *settings_get(void);
