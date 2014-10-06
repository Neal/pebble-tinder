#include <pebble.h>
#include "win-error.h"
#include "libs/pebble-assist.h"
#include "tinder.h"

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context);
static void click_config_provider(void *context);
static void window_load(Window *window);
static void window_unload(Window *window);

static Window* window = NULL;
static GBitmap* error_bitmap = NULL;
static BitmapLayer* error_bitmap_layer = NULL;
static TextLayer* text_layer;

void win_error_init(void) {
	error_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_ERROR);

	window = window_create();
	window_set_fullscreen(window, true);
	window_set_background_color(window, GColorBlack);
	window_set_click_config_provider(window, click_config_provider);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
}

void win_error_push(void) {
	if (window && window_stack_contains_window(window)) {
		return;
	}
	window_stack_push(window, true);
}

void win_error_deinit(void) {
	gbitmap_destroy(error_bitmap);
	window_destroy_safe(window);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void select_single_click_handler(ClickRecognizerRef recognizer, void *context) {
	window_stack_pop(true);
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_single_click_handler);
}

static void window_load(Window *window) {
	error_bitmap_layer = bitmap_layer_create(GRect(61, 40, 21, 19));
	bitmap_layer_set_bitmap(error_bitmap_layer, error_bitmap);
	bitmap_layer_set_compositing_mode(error_bitmap_layer, GCompOpAssignInverted);
	bitmap_layer_add_to_window(error_bitmap_layer, window);

	text_layer = text_layer_create(GRect(4, 66, 136, 96));
	text_layer_set_system_font(text_layer, FONT_KEY_GOTHIC_18_BOLD);
	text_layer_set_text(text_layer, tinder_get_error());
	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	text_layer_set_colors(text_layer, GColorWhite, GColorClear);
	text_layer_add_to_window(text_layer, window);
}

static void window_unload(Window *window) {
	text_layer_destroy(text_layer);
	bitmap_layer_destroy(error_bitmap_layer);
}
