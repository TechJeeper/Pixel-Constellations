#include "face.h"
#include "settings.h"
#include "constellation.h"

#define FRAME_MS 50

typedef struct {
  int16_t x;
  int16_t y;
  uint8_t phase;
  uint8_t bright;
} AmbientStar;

static Window *s_window;
static Layer *s_layer;
static AppTimer *s_anim_timer;

static GBitmap *s_bmp_star;
static GBitmap *s_bmp_star_node;
static GBitmap *s_bmp_moon;
static GBitmap *s_bmp_nebula;

static int s_hour;
static int s_minute;
static int s_second;
static int s_wday;
static int s_mday;
static int s_mon;

static int s_edges_done;       // fully drawn edges across all digits
static int s_edge_progress;    // 0..1000 within current edge
static int s_total_edges;
static bool s_drawing_complete;

static AmbientStar s_ambient[AMBIENT_STAR_MAX];
static int s_ambient_count;
static int s_twinkle_tick;

static void layout_metrics(GRect bounds, bool blank_leading,
                           int *digit_w, int *gap, int *colon_w, int *start_x, int *y) {
  *digit_w = (bounds.size.w >= 180) ? 36 : 28;
  *gap = (bounds.size.w >= 180) ? 6 : 4;
  *colon_w = (bounds.size.w >= 180) ? 10 : 8;
  const int digits = blank_leading ? 3 : 4;
  const int gaps = blank_leading ? 2 : 3;
  const int total = (*digit_w) * digits + (*gap) * gaps + (*colon_w);
  *start_x = (bounds.size.w - total) / 2;
  *y = (bounds.size.h >= 200) ? 52 : 40;
}

static GPoint digit_origin(GRect bounds, int index, bool blank_leading) {
  // Digits: [H] H : M M  — leading H may be omitted when blank.
  int digit_w, gap, colon_w, start_x, y;
  layout_metrics(bounds, blank_leading, &digit_w, &gap, &colon_w, &start_x, &y);

  int x = start_x;
  if (blank_leading) {
    if (index == 1) {
      // first visible hour digit
    } else if (index == 2) {
      x += digit_w + gap + colon_w + gap;
    } else if (index == 3) {
      x += digit_w + gap + colon_w + gap + digit_w + gap;
    }
  } else if (index >= 2) {
    x += digit_w * 2 + gap + colon_w + gap;
    x += (index - 2) * (digit_w + gap);
  } else {
    x += index * (digit_w + gap);
  }
  return GPoint(x, y);
}

static int digit_cell(GRect bounds) {
  return (bounds.size.w >= 180) ? 7 : 5;
}

static GPoint map_node(GPoint origin, StarNode n, int cell) {
  return GPoint(origin.x + n.x * cell, origin.y + n.y * cell);
}

static void time_digits(int *out) {
  int hour = s_hour;
  const FaceSettings *cfg = settings_get();
  bool is24 = true;
  if (cfg->time_format == TIME_FMT_12H) is24 = false;
  else if (cfg->time_format == TIME_FMT_SYSTEM) is24 = clock_is_24h_style();

  if (!is24) {
    hour = hour % 12;
    if (hour == 0) hour = 12;
  }

  if (cfg->leading_zero || is24 || hour >= 10) {
    out[0] = hour / 10;
  } else {
    out[0] = -1; // blank leading hour
  }
  out[1] = hour % 10;
  out[2] = s_minute / 10;
  out[3] = s_minute % 10;
}

static int count_edges_for_digits(const int digits[4]) {
  int total = 0;
  for (int i = 0; i < TIME_DIGIT_COUNT; i++) {
    if (digits[i] < 0) continue;
    DigitShape shape;
    constellation_digit_shape(digits[i], &shape);
    total += shape.edge_count;
  }
  // Colon is two short edges (two dots linked optionally — count as 0 edges, just dots)
  return total;
}

static void rebuild_ambient(GRect bounds) {
  const FaceSettings *cfg = settings_get();
  int density = 12;
  if (cfg->ambient_stars == 1) density = 24;
  if (cfg->ambient_stars == 2) density = 40;
  if (density > AMBIENT_STAR_MAX) density = AMBIENT_STAR_MAX;

  s_ambient_count = density;
  // Deterministic scatter so the sky doesn't jump on redraw.
  uint32_t seed = 0xC0FFEEU ^ (uint32_t)bounds.size.w ^ ((uint32_t)bounds.size.h << 8);
  for (int i = 0; i < s_ambient_count; i++) {
    seed = seed * 1664525u + 1013904223u;
    s_ambient[i].x = (int16_t)(seed % (uint32_t)bounds.size.w);
    seed = seed * 1664525u + 1013904223u;
    s_ambient[i].y = (int16_t)(seed % (uint32_t)bounds.size.h);
    s_ambient[i].phase = (uint8_t)(seed & 0xFF);
    s_ambient[i].bright = (uint8_t)((seed >> 8) & 1);
  }
}

static void reset_draw_animation(void) {
  int digits[4];
  time_digits(digits);
  s_total_edges = count_edges_for_digits(digits);
  s_edges_done = 0;
  s_edge_progress = 0;
  s_drawing_complete = (s_total_edges == 0) ||
                       (settings_get()->anim_speed == ANIM_INSTANT);
  if (s_drawing_complete) {
    s_edges_done = s_total_edges;
    s_edge_progress = 1000;
  }
}

static void advance_animation(void) {
  if (s_drawing_complete) return;

  const FaceSettings *cfg = settings_get();
  const int ms_per_edge = constellation_anim_ms_per_edge(cfg->anim_speed);
  if (ms_per_edge <= 0) {
    s_edges_done = s_total_edges;
    s_edge_progress = 1000;
    s_drawing_complete = true;
    return;
  }

  // FRAME_MS worth of progress.
  const int delta = (FRAME_MS * 1000) / ms_per_edge;
  s_edge_progress += delta;
  while (s_edge_progress >= 1000 && s_edges_done < s_total_edges) {
    s_edge_progress -= 1000;
    s_edges_done++;
  }
  if (s_edges_done >= s_total_edges) {
    s_edges_done = s_total_edges;
    s_edge_progress = 1000;
    s_drawing_complete = true;
  }
}

static void draw_ambient(GContext *ctx, const ThemeColors *theme) {
  const FaceSettings *cfg = settings_get();
  for (int i = 0; i < s_ambient_count; i++) {
    int phase = (s_ambient[i].phase + s_twinkle_tick) & 0xFF;
    bool on = true;
    if (cfg->twinkle == 0) {
      on = (phase & 0xE0) != 0xE0; // rare blink
    } else if (cfg->twinkle == 1) {
      on = (phase & 0xC0) != 0xC0;
    } else {
      on = (phase & 0x80) != 0;
    }
    if (!on) continue;

    GColor c = s_ambient[i].bright ? theme->star : theme->star_dim;
    graphics_context_set_fill_color(ctx, c);
    graphics_fill_rect(ctx, GRect(s_ambient[i].x, s_ambient[i].y, 1, 1), 0, GCornerNone);
  }
}

static void draw_colon(GContext *ctx, GRect bounds, const ThemeColors *theme,
                       bool blink_on, bool blank_leading) {
  int digit_w, gap, colon_w, start_x, y;
  layout_metrics(bounds, blank_leading, &digit_w, &gap, &colon_w, &start_x, &y);
  const int cell = digit_cell(bounds);
  const int hour_digits = blank_leading ? 1 : 2;

  GPoint c0 = GPoint(start_x + digit_w * hour_digits + gap + colon_w / 2, y + 2 * cell);
  GPoint c1 = GPoint(c0.x, y + 4 * cell);

  if (!blink_on) return;
  constellation_draw_star_pixel(ctx, c0, theme->accent, true);
  constellation_draw_star_pixel(ctx, c1, theme->accent, true);
}

static void draw_time_constellations(GContext *ctx, GRect bounds, const ThemeColors *theme) {
  int digits[4];
  time_digits(digits);
  const bool blank_leading = digits[0] < 0;
  const int cell = digit_cell(bounds);

  int edge_index = 0;
  for (int d = 0; d < TIME_DIGIT_COUNT; d++) {
    if (digits[d] < 0) continue;

    DigitShape shape;
    constellation_digit_shape(digits[d], &shape);
    GPoint origin = digit_origin(bounds, d, blank_leading);

    const int base = edge_index;

    // Edges grow left-to-right across HH:MM.
    for (int e = 0; e < shape.edge_count; e++) {
      StarNode na = shape.nodes[shape.edges[e].a];
      StarNode nb = shape.nodes[shape.edges[e].b];
      GPoint a = map_node(origin, na, cell);
      GPoint b = map_node(origin, nb, cell);

      if (edge_index < s_edges_done || s_drawing_complete) {
        constellation_draw_line_progress(ctx, a, b, 1000, theme->line);
      } else if (edge_index == s_edges_done) {
        constellation_draw_line_progress(ctx, a, b, s_edge_progress, theme->line);
      }
      edge_index++;
    }

    // Stars appear once an incident edge has begun (or at the start of a digit).
    for (int n = 0; n < shape.node_count; n++) {
      bool visible = s_drawing_complete || settings_get()->anim_speed == ANIM_INSTANT;
      if (!visible) {
        if (s_edges_done >= base + shape.edge_count) {
          visible = true;
        } else if (s_edges_done >= base) {
          visible = true; // digit currently being drawn
        } else if (base == 0 && s_edges_done == 0) {
          visible = true;
        } else {
          for (int e = 0; e < shape.edge_count; e++) {
            if (shape.edges[e].a != n && shape.edges[e].b != n) continue;
            if (base + e < s_edges_done ||
                (base + e == s_edges_done && s_edge_progress > 0)) {
              visible = true;
              break;
            }
          }
        }
      }
      if (!visible) continue;

      GPoint p = map_node(origin, shape.nodes[n], cell);
      bool bright = ((n + s_twinkle_tick / 4) & 3) != 0;
      constellation_draw_star_pixel(ctx, p, bright ? theme->star : theme->accent, bright);

      if (s_bmp_star_node && bright && (n % 3 == 0)) {
        GSize sz = gbitmap_get_bounds(s_bmp_star_node).size;
        graphics_context_set_compositing_mode(ctx, GCompOpSet);
        graphics_draw_bitmap_in_rect(
            ctx, s_bmp_star_node,
            GRect(p.x - sz.w / 2 + 1, p.y - sz.h / 2 + 1, sz.w, sz.h));
      }
    }
  }
}

static void draw_date(GContext *ctx, GRect bounds, const ThemeColors *theme) {
  static const char *const DOW[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
  static const char *const MON[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };
  char buf[24];
  snprintf(buf, sizeof(buf), "%s %d %s", DOW[s_wday % 7], s_mday, MON[s_mon % 12]);
  graphics_context_set_text_color(ctx, theme->date);
  graphics_draw_text(ctx, buf, fonts_get_system_font(FONT_KEY_GOTHIC_14),
                     GRect(0, bounds.size.h - 22, bounds.size.w, 18),
                     GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

static void update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  const FaceSettings *cfg = settings_get();
  ThemeColors theme;
  constellation_theme_colors(cfg->color_theme, &theme);

  graphics_context_set_fill_color(ctx, theme.bg);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  if (cfg->show_nebula && s_bmp_nebula) {
    GSize sz = gbitmap_get_bounds(s_bmp_nebula).size;
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_bmp_nebula,
                                 GRect(bounds.size.w - sz.w + 4, -4, sz.w, sz.h));
  }

  if (cfg->show_moon && s_bmp_moon) {
    GSize sz = gbitmap_get_bounds(s_bmp_moon).size;
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(ctx, s_bmp_moon, GRect(8, 8, sz.w, sz.h));
  }

  draw_ambient(ctx, &theme);

  // Occasional decorative sparkle using the PixelLab star sprite.
  if (s_bmp_star && (s_twinkle_tick % 17) < 8) {
    GSize sz = gbitmap_get_bounds(s_bmp_star).size;
    graphics_context_set_compositing_mode(ctx, GCompOpSet);
    graphics_draw_bitmap_in_rect(
        ctx, s_bmp_star,
        GRect(bounds.size.w - sz.w - 12, bounds.size.h / 3, sz.w, sz.h));
  }

  bool colon_on = true;
  if (cfg->show_seconds) {
    colon_on = (s_second % 2) == 0;
  }
  int digits[4];
  time_digits(digits);
  draw_time_constellations(ctx, bounds, &theme);
  draw_colon(ctx, bounds, &theme, colon_on, digits[0] < 0);

  if (cfg->show_date) {
    draw_date(ctx, bounds, &theme);
  }
}

static void anim_cb(void *ctx) {
  (void)ctx;
  s_anim_timer = app_timer_register(FRAME_MS, anim_cb, NULL);
  s_twinkle_tick++;
  advance_animation();
  if (s_layer) layer_mark_dirty(s_layer);
}

static void load_bitmaps(void) {
  s_bmp_star = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAR);
  s_bmp_star_node = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_STAR_NODE);
  s_bmp_moon = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MOON);
  s_bmp_nebula = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEBULA);
}

static void unload_bitmaps(void) {
  if (s_bmp_star) { gbitmap_destroy(s_bmp_star); s_bmp_star = NULL; }
  if (s_bmp_star_node) { gbitmap_destroy(s_bmp_star_node); s_bmp_star_node = NULL; }
  if (s_bmp_moon) { gbitmap_destroy(s_bmp_moon); s_bmp_moon = NULL; }
  if (s_bmp_nebula) { gbitmap_destroy(s_bmp_nebula); s_bmp_nebula = NULL; }
}

void face_init(Window *window) {
  s_window = window;
  load_bitmaps();

  Layer *root = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(root);
  s_layer = layer_create(bounds);
  layer_set_update_proc(s_layer, update_proc);
  layer_add_child(root, s_layer);

  rebuild_ambient(bounds);

  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  s_hour = t->tm_hour;
  s_minute = t->tm_min;
  s_second = t->tm_sec;
  s_wday = t->tm_wday;
  s_mday = t->tm_mday;
  s_mon = t->tm_mon;
  reset_draw_animation();

  ThemeColors theme;
  constellation_theme_colors(settings_get()->color_theme, &theme);
  window_set_background_color(window, theme.bg);

  s_anim_timer = app_timer_register(FRAME_MS, anim_cb, NULL);
}

void face_deinit(void) {
  if (s_anim_timer) {
    app_timer_cancel(s_anim_timer);
    s_anim_timer = NULL;
  }
  unload_bitmaps();
  if (s_layer) {
    layer_destroy(s_layer);
    s_layer = NULL;
  }
}

void face_tick(void) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  const bool minute_changed = (t->tm_hour != s_hour) || (t->tm_min != s_minute);

  s_hour = t->tm_hour;
  s_minute = t->tm_min;
  s_second = t->tm_sec;
  s_wday = t->tm_wday;
  s_mday = t->tm_mday;
  s_mon = t->tm_mon;

  if (minute_changed) {
    reset_draw_animation();
  }
  if (s_layer) layer_mark_dirty(s_layer);
}

void face_apply_settings(void) {
  if (!s_layer) return;
  GRect bounds = layer_get_bounds(s_layer);
  rebuild_ambient(bounds);
  reset_draw_animation();

  ThemeColors theme;
  constellation_theme_colors(settings_get()->color_theme, &theme);
  if (s_window) window_set_background_color(s_window, theme.bg);
  layer_mark_dirty(s_layer);
}
