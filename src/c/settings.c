#include "settings.h"

enum {
  PERSIST_TIME_FORMAT = 1,
  PERSIST_LEADING_ZERO = 2,
  PERSIST_SHOW_DATE = 3,
  PERSIST_SHOW_SECONDS = 4,
  PERSIST_SHOW_MOON = 5,
  PERSIST_SHOW_NEBULA = 6,
  PERSIST_COLOR_THEME = 7,
  PERSIST_ANIM_SPEED = 8,
  PERSIST_TWINKLE = 9,
  PERSIST_AMBIENT = 10,
};

static FaceSettings s_settings;
static SettingsChangedFn s_changed;
static bool s_inbox_open;

static int clamp_int(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static void normalize(void) {
  s_settings.time_format = (TimeFormat)clamp_int(
      (int)s_settings.time_format, TIME_FMT_SYSTEM, TIME_FMT_24H);
  s_settings.color_theme = (ColorTheme)clamp_int(
      (int)s_settings.color_theme, THEME_CLASSIC, THEME_COUNT - 1);
  s_settings.anim_speed = (AnimSpeed)clamp_int(
      (int)s_settings.anim_speed, ANIM_SLOW, ANIM_COUNT - 1);
  s_settings.twinkle = clamp_int(s_settings.twinkle, 0, 2);
  s_settings.ambient_stars = clamp_int(s_settings.ambient_stars, 0, 2);
}

static void settings_set_defaults(void) {
  s_settings.time_format = TIME_FMT_SYSTEM;
  s_settings.leading_zero = true;
  s_settings.show_date = false;
  s_settings.show_seconds = true;
  s_settings.show_moon = true;
  s_settings.show_nebula = true;
  s_settings.color_theme = THEME_CLASSIC;
  s_settings.anim_speed = ANIM_NORMAL;
  s_settings.twinkle = 1;
  s_settings.ambient_stars = 1;
}

static void settings_load(void) {
  settings_set_defaults();
  if (persist_exists(PERSIST_TIME_FORMAT)) {
    s_settings.time_format = (TimeFormat)persist_read_int(PERSIST_TIME_FORMAT);
  }
  if (persist_exists(PERSIST_LEADING_ZERO)) {
    s_settings.leading_zero = persist_read_bool(PERSIST_LEADING_ZERO);
  }
  if (persist_exists(PERSIST_SHOW_DATE)) {
    s_settings.show_date = persist_read_bool(PERSIST_SHOW_DATE);
  }
  if (persist_exists(PERSIST_SHOW_SECONDS)) {
    s_settings.show_seconds = persist_read_bool(PERSIST_SHOW_SECONDS);
  }
  if (persist_exists(PERSIST_SHOW_MOON)) {
    s_settings.show_moon = persist_read_bool(PERSIST_SHOW_MOON);
  }
  if (persist_exists(PERSIST_SHOW_NEBULA)) {
    s_settings.show_nebula = persist_read_bool(PERSIST_SHOW_NEBULA);
  }
  if (persist_exists(PERSIST_COLOR_THEME)) {
    s_settings.color_theme = (ColorTheme)persist_read_int(PERSIST_COLOR_THEME);
  }
  if (persist_exists(PERSIST_ANIM_SPEED)) {
    s_settings.anim_speed = (AnimSpeed)persist_read_int(PERSIST_ANIM_SPEED);
  }
  if (persist_exists(PERSIST_TWINKLE)) {
    s_settings.twinkle = persist_read_int(PERSIST_TWINKLE);
  }
  if (persist_exists(PERSIST_AMBIENT)) {
    s_settings.ambient_stars = persist_read_int(PERSIST_AMBIENT);
  }
  normalize();
}

static void settings_save(void) {
  normalize();
  persist_write_int(PERSIST_TIME_FORMAT, (int)s_settings.time_format);
  persist_write_bool(PERSIST_LEADING_ZERO, s_settings.leading_zero);
  persist_write_bool(PERSIST_SHOW_DATE, s_settings.show_date);
  persist_write_bool(PERSIST_SHOW_SECONDS, s_settings.show_seconds);
  persist_write_bool(PERSIST_SHOW_MOON, s_settings.show_moon);
  persist_write_bool(PERSIST_SHOW_NEBULA, s_settings.show_nebula);
  persist_write_int(PERSIST_COLOR_THEME, (int)s_settings.color_theme);
  persist_write_int(PERSIST_ANIM_SPEED, (int)s_settings.anim_speed);
  persist_write_int(PERSIST_TWINKLE, s_settings.twinkle);
  persist_write_int(PERSIST_AMBIENT, s_settings.ambient_stars);
}

static int tuple_as_int(Tuple *t, int fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return (int)t->value->int32;
  if (t->type == TUPLE_CSTRING) return atoi(t->value->cstring);
  return fallback;
}

static bool tuple_as_bool(Tuple *t, bool fallback) {
  if (!t) return fallback;
  if (t->type == TUPLE_INT) return t->value->int32 != 0;
  if (t->type == TUPLE_CSTRING) {
    return strcmp(t->value->cstring, "true") == 0 ||
           strcmp(t->value->cstring, "1") == 0;
  }
  return fallback;
}

static void inbox_received(DictionaryIterator *iter, void *context) {
  (void)context;
  Tuple *t;

  t = dict_find(iter, MESSAGE_KEY_TimeFormat);
  if (t) s_settings.time_format = (TimeFormat)tuple_as_int(t, (int)s_settings.time_format);

  t = dict_find(iter, MESSAGE_KEY_LeadingZero);
  if (t) s_settings.leading_zero = tuple_as_bool(t, s_settings.leading_zero);

  t = dict_find(iter, MESSAGE_KEY_ShowDate);
  if (t) s_settings.show_date = tuple_as_bool(t, s_settings.show_date);

  t = dict_find(iter, MESSAGE_KEY_ShowSeconds);
  if (t) s_settings.show_seconds = tuple_as_bool(t, s_settings.show_seconds);

  t = dict_find(iter, MESSAGE_KEY_ShowMoon);
  if (t) s_settings.show_moon = tuple_as_bool(t, s_settings.show_moon);

  t = dict_find(iter, MESSAGE_KEY_ShowNebula);
  if (t) s_settings.show_nebula = tuple_as_bool(t, s_settings.show_nebula);

  t = dict_find(iter, MESSAGE_KEY_ColorTheme);
  if (t) s_settings.color_theme = (ColorTheme)tuple_as_int(t, (int)s_settings.color_theme);

  t = dict_find(iter, MESSAGE_KEY_AnimSpeed);
  if (t) s_settings.anim_speed = (AnimSpeed)tuple_as_int(t, (int)s_settings.anim_speed);

  t = dict_find(iter, MESSAGE_KEY_Twinkle);
  if (t) s_settings.twinkle = tuple_as_int(t, s_settings.twinkle);

  t = dict_find(iter, MESSAGE_KEY_AmbientStars);
  if (t) s_settings.ambient_stars = tuple_as_int(t, s_settings.ambient_stars);

  settings_save();
  if (s_changed) s_changed();
}

void settings_init(void) {
  settings_load();
  if (!s_inbox_open) {
    app_message_register_inbox_received(inbox_received);
    app_message_open(app_message_inbox_size_maximum(),
                     app_message_outbox_size_maximum());
    s_inbox_open = true;
  }
}

void settings_deinit(void) {
  settings_save();
  if (s_inbox_open) {
    app_message_deregister_callbacks();
    s_inbox_open = false;
  }
}

void settings_set_changed_callback(SettingsChangedFn cb) {
  s_changed = cb;
}

const FaceSettings *settings_get(void) {
  return &s_settings;
}
