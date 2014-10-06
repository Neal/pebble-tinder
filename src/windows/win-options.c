#include <pebble.h>
#include "win-options.h"
#include "libs/pebble-assist.h"
#include "win-profile.h"
#include "win-myprofile.h"
#include "win-messages.h"
#include "win-settings.h"
#include "win-about.h"

static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context);
static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context);
static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);

static Window* window = NULL;
static MenuLayer* menu_layer = NULL;

void win_options_init(void) {
	window = window_create();

	menu_layer = menu_layer_create_fullscreen(window);
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks) {
		.get_num_rows = menu_get_num_rows_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	menu_layer_add_to_window(menu_layer, window);
}

void win_options_push(void) {
	window_stack_push(window, true);
}

void win_options_deinit(void) {
	menu_layer_destroy_safe(menu_layer);
	window_destroy_safe(window);
}

void win_options_reload_data_and_mark_dirty(void) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static uint16_t menu_get_num_rows_callback(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
	return 4;
}

static int16_t menu_get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	return MENU_CELL_BASIC_CELL_HEIGHT;
}

static void menu_draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
	switch (cell_index->row) {
		case 0:
			menu_cell_basic_draw(ctx, cell_layer, "Profile", NULL, NULL);
			break;
		case 1:
			menu_cell_basic_draw(ctx, cell_layer, "Messages", NULL, NULL);
			break;
		case 2:
			menu_cell_basic_draw(ctx, cell_layer, "Settings", NULL, NULL);
			break;
		case 3:
			menu_cell_basic_draw(ctx, cell_layer, "About", NULL, NULL);
			break;
	}
}

static void menu_select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	switch (cell_index->row) {
		case 0:
			win_myprofile_push();
			break;
		case 1:
			win_messages_push();
			break;
		case 2:
			win_settings_push();
			break;
		case 3:
			win_about_push();
			break;
	}
}
