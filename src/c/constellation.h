#pragma once

#include <pebble.h>
#include "settings.h"

#define DIGIT_NODE_MAX 12
#define DIGIT_EDGE_MAX 14
#define TIME_DIGIT_COUNT 4
#define AMBIENT_STAR_MAX 48

typedef struct {
  int8_t x;
  int8_t y;
} StarNode;

typedef struct {
  uint8_t a;
  uint8_t b;
} StarEdge;

typedef struct {
  uint8_t node_count;
  uint8_t edge_count;
  StarNode nodes[DIGIT_NODE_MAX];
  StarEdge edges[DIGIT_EDGE_MAX];
} DigitShape;

typedef struct {
  GColor bg;
  GColor star;
  GColor star_dim;
  GColor line;
  GColor line_dim;
  GColor accent;
  GColor date;
} ThemeColors;

void constellation_digit_shape(int digit, DigitShape *out);
void constellation_theme_colors(ColorTheme theme, ThemeColors *out);
int constellation_anim_ms_per_edge(AnimSpeed speed);

// Draw a partially completed line from a→b (progress 0..1000).
void constellation_draw_line_progress(GContext *ctx, GPoint a, GPoint b,
                                      int progress_permille, GColor color);

void constellation_draw_star_pixel(GContext *ctx, GPoint p, GColor color, bool bright);
