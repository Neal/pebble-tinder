#include <pebble.h>
#include "win-myprofile.h"
#include "libs/pebble-assist.h"
#include "tinder.h"

// photo?
// name
// age
// location
// bio

static void window_load(Window *window);
static void window_unload(Window *window);

static Window* window = NULL;
// static Window* scroll_layer = NULL;

void win_myprofile_init(void) {
	window = window_create();
	window_set_fullscreen(window, true);
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
}

void win_myprofile_push(void) {
	window_stack_push(window, true);
}

void win_myprofile_deinit(void) {
	window_destroy_safe(window);
}

void win_myprofile_reload_data_and_mark_dirty(void) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void window_load(Window *window) {
}

static void window_unload(Window *window) {
}
