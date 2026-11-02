#include "constellation.h"

// Digit graphs live on a 5×7 logical grid (x:0..4, y:0..6).
// Stars sit at nodes; edges are the constellation lines that spell the time.

static void shape_copy(DigitShape *out, const StarNode *nodes, int ncount,
                       const StarEdge *edges, int ecount) {
  out->node_count = (uint8_t)ncount;
  out->edge_count = (uint8_t)ecount;
  for (int i = 0; i < ncount; i++) out->nodes[i] = nodes[i];
  for (int i = 0; i < ecount; i++) out->edges[i] = edges[i];
}

void constellation_digit_shape(int digit, DigitShape *out) {
  digit = digit % 10;
  switch (digit) {
    case 0: {
      static const StarNode n[] = {
        {1,0},{3,0},{4,2},{4,4},{3,6},{1,6},{0,4},{0,2}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,0}
      };
      shape_copy(out, n, 8, e, 8);
      break;
    }
    case 1: {
      static const StarNode n[] = {{1,1},{2,0},{2,6},{1,6},{3,6}};
      static const StarEdge e[] = {{0,1},{1,2},{2,3},{2,4}};
      shape_copy(out, n, 5, e, 4);
      break;
    }
    case 2: {
      static const StarNode n[] = {
        {0,1},{1,0},{3,0},{4,1},{4,2},{0,5},{0,6},{4,6}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7}
      };
      shape_copy(out, n, 8, e, 7);
      break;
    }
    case 3: {
      static const StarNode n[] = {
        {0,1},{1,0},{3,0},{4,1},{4,2},{2,3},{4,4},{4,5},{3,6},{1,6},{0,5}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9},{9,10}
      };
      shape_copy(out, n, 11, e, 10);
      break;
    }
    case 4: {
      static const StarNode n[] = {{0,0},{0,3},{4,3},{3,0},{3,6}};
      static const StarEdge e[] = {{0,1},{1,2},{3,4},{2,3}};
      shape_copy(out, n, 5, e, 4);
      break;
    }
    case 5: {
      static const StarNode n[] = {
        {4,0},{0,0},{0,3},{3,3},{4,4},{4,5},{3,6},{1,6},{0,5}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8}
      };
      shape_copy(out, n, 9, e, 8);
      break;
    }
    case 6: {
      static const StarNode n[] = {
        {3,0},{1,0},{0,2},{0,4},{1,6},{3,6},{4,5},{4,4},{3,3},{0,3}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,7},{7,8},{8,9}
      };
      shape_copy(out, n, 10, e, 9);
      break;
    }
    case 7: {
      static const StarNode n[] = {{0,0},{4,0},{2,3},{1,6}};
      static const StarEdge e[] = {{0,1},{1,2},{2,3}};
      shape_copy(out, n, 4, e, 3);
      break;
    }
    case 8: {
      static const StarNode n[] = {
        {1,0},{3,0},{4,2},{3,3},{1,3},{0,2},
        {4,4},{3,6},{1,6},{0,4}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,0},
        {3,6},{6,7},{7,8},{8,9},{9,4}
      };
      shape_copy(out, n, 10, e, 11);
      break;
    }
    case 9: {
      static const StarNode n[] = {
        {1,3},{3,3},{4,1},{3,0},{1,0},{0,1},{0,2},
        {4,3},{3,6},{1,6}
      };
      static const StarEdge e[] = {
        {0,1},{1,2},{2,3},{3,4},{4,5},{5,6},{6,0},
        {1,7},{7,8},{8,9}
      };
      shape_copy(out, n, 10, e, 10);
      break;
    }
    default:
      out->node_count = 0;
      out->edge_count = 0;
      break;
  }
}

void constellation_theme_colors(ColorTheme theme, ThemeColors *out) {
#ifdef PBL_COLOR
  switch (theme) {
    case THEME_GOLD:
      out->bg = GColorFromHEX(0x120C08);
      out->star = GColorFromHEX(0xFFE8A0);
      out->star_dim = GColorFromHEX(0xA08040);
      out->line = GColorFromHEX(0xD0A050);
      out->line_dim = GColorFromHEX(0x705828);
      out->accent = GColorFromHEX(0xFFD070);
      out->date = GColorFromHEX(0xC0A060);
      break;
    case THEME_CYAN:
      out->bg = GColorFromHEX(0x041018);
      out->star = GColorFromHEX(0xE0FFFF);
      out->star_dim = GColorFromHEX(0x4080A0);
      out->line = GColorFromHEX(0x40C0E0);
      out->line_dim = GColorFromHEX(0x206070);
      out->accent = GColorFromHEX(0x80E8FF);
      out->date = GColorFromHEX(0x70A0B0);
      break;
    case THEME_MAGENTA:
      out->bg = GColorFromHEX(0x100818);
      out->star = GColorFromHEX(0xFFE0F0);
      out->star_dim = GColorFromHEX(0x804060);
      out->line = GColorFromHEX(0xE060C0);
      out->line_dim = GColorFromHEX(0x703060);
      out->accent = GColorFromHEX(0xFF90D0);
      out->date = GColorFromHEX(0xB07090);
      break;
    case THEME_AURORA:
      out->bg = GColorFromHEX(0x061210);
      out->star = GColorFromHEX(0xE8FFF0);
      out->star_dim = GColorFromHEX(0x408060);
      out->line = GColorFromHEX(0x40E0A0);
      out->line_dim = GColorFromHEX(0x206048);
      out->accent = GColorFromHEX(0x70A0E0);
      out->date = GColorFromHEX(0x70B090);
      break;
    case THEME_CLASSIC:
    default:
      out->bg = GColorFromHEX(0x0A0A1E);
      out->star = GColorWhite;
      out->star_dim = GColorFromHEX(0x606080);
      out->line = GColorFromHEX(0xA0C0E0);
      out->line_dim = GColorFromHEX(0x405070);
      out->accent = GColorFromHEX(0xFFE8A0);
      out->date = GColorFromHEX(0x8080A0);
      break;
  }
#else
  (void)theme;
  out->bg = GColorBlack;
  out->star = GColorWhite;
  out->star_dim = GColorLightGray;
  out->line = GColorWhite;
  out->line_dim = GColorLightGray;
  out->accent = GColorWhite;
  out->date = GColorLightGray;
#endif
}

int constellation_anim_ms_per_edge(AnimSpeed speed) {
  switch (speed) {
    case ANIM_SLOW: return 420;
    case ANIM_FAST: return 90;
    case ANIM_INSTANT: return 0;
    case ANIM_NORMAL:
    default: return 180;
  }
}

void constellation_draw_line_progress(GContext *ctx, GPoint a, GPoint b,
                                      int progress_permille, GColor color) {
  if (progress_permille <= 0) return;
  if (progress_permille > 1000) progress_permille = 1000;

  const int dx = b.x - a.x;
  const int dy = b.y - a.y;
  GPoint end = {
    a.x + (dx * progress_permille) / 1000,
    a.y + (dy * progress_permille) / 1000
  };

  graphics_context_set_stroke_color(ctx, color);
  graphics_context_set_stroke_width(ctx, 1);
  graphics_draw_line(ctx, a, end);
}

void constellation_draw_star_pixel(GContext *ctx, GPoint p, GColor color, bool bright) {
  graphics_context_set_fill_color(ctx, color);
  graphics_fill_rect(ctx, GRect(p.x, p.y, 2, 2), 0, GCornerNone);
  if (bright) {
    graphics_fill_rect(ctx, GRect(p.x - 1, p.y, 1, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(p.x + 2, p.y, 1, 2), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(p.x, p.y - 1, 2, 1), 0, GCornerNone);
    graphics_fill_rect(ctx, GRect(p.x, p.y + 2, 2, 1), 0, GCornerNone);
  }
}
