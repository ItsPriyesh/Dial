#include <pebble.h>

static Window *s_main_window;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

#define BACKGROUND_WIDTH 1366
#define BACKGROUND_END 1350
#define SCREEN_WIDTH 144
#define SCREEN_HEIGHT 168

static void draw_clock(struct tm *tick_time) {
  const int64_t mins_in_day = 24 * 60;
  const int64_t mins_since_midnight = tick_time->tm_hour * 60 + tick_time->tm_min;
  const int64_t background_x_offset = mins_since_midnight * BACKGROUND_END / mins_in_day;

  const GRect frame = GRect(-background_x_offset + SCREEN_WIDTH / 2, 0, BACKGROUND_WIDTH, SCREEN_HEIGHT);
  layer_set_frame((Layer*) s_background_layer, frame);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  draw_clock(tick_time);
}

static void pin_layer_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorOrange);
  graphics_fill_rect(ctx, GRect(71, 0, 2, 101), 0, 0);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  Layer *pin_layer = layer_create(bounds);
  layer_set_update_proc(pin_layer, pin_layer_update_proc);

  s_background_layer = bitmap_layer_create(bounds);
  s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
  bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);

  layer_add_child(window_layer, (Layer*) s_background_layer);
  layer_add_child(window_layer, pin_layer);
}

static void main_window_unload(Window *window) {
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_bitmap);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);

  time_t current_time = time(NULL);
  draw_clock(localtime(&current_time));

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}