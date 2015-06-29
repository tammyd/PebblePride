#include <pebble.h>

static Window      *s_main_window;
static TextLayer   *s_time_layer;
static GFont        s_time_font;
static GFont        s_weather_font;
static GBitmap     *s_background_image;
static BitmapLayer *s_background_layer;


static void load_fonts() {
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FREDOKAONE_40));
  s_weather_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_FREDOKAONE_20));
}

static void create_background_layer() {
  s_background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PRIDE_BACKGROUND);
  s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(s_background_layer, s_background_image);

}

static void create_time_layer() {
  s_time_layer = text_layer_create(GRect(0, 56, 144, 56));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  // Write the current hours and minutes into the buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  text_layer_set_text(s_time_layer, buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void main_window_load(Window *window) {
  load_fonts();
  create_time_layer();
  create_background_layer();

  // Add each layer as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
}

static void unload_time_layer() {
  text_layer_destroy(s_time_layer);
}

static void unload_background_layer() {
  bitmap_layer_destroy(s_background_layer);
  gbitmap_destroy(s_background_image);
}

static void unload_fonts() {
  fonts_unload_custom_font(s_weather_font);
  fonts_unload_custom_font(s_time_font);
}

static void main_window_unload(Window *window) {
  unload_time_layer();
  unload_background_layer();
  unload_fonts();
}


static void init() {
  // register time handler
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);
  update_time();
}

static void deinit() {
  window_destroy(s_main_window);

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
