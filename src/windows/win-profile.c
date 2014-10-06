#include <pebble.h>
#include "win-profile.h"
#include "libs/pebble-assist.h"
#include "tinder.h"

static void window_load(Window *window);
static void window_unload(Window *window);

static Window* window = NULL;
static ScrollLayer* scroll_layer = NULL;
static TextLayer* name_text_layer = NULL;
static TextLayer* distance_text_layer = NULL;
static TextLayer* bio_text_layer = NULL;
static TextLayer* commlikes_text_layer = NULL;
static TextLayer* commfriends_text_layer = NULL;
static GBitmap* likes_bitmap = NULL;
static GBitmap* friends_bitmap = NULL;
static BitmapLayer* likes_bitmap_layer = NULL;
static BitmapLayer* friends_bitmap_layer = NULL;

void win_profile_init(void) {
	likes_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LIKES);
	friends_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_FRIENDS);

	window = window_create();
	window_set_fullscreen(window, true);
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
}

void win_profile_push(void) {
	window_stack_push(window, true);
	scroll_layer_set_content_size(scroll_layer, GSize(144, text_layer_get_content_size(bio_text_layer).h + 10));
}

void win_profile_deinit(void) {
	gbitmap_destroy(likes_bitmap);
	gbitmap_destroy(friends_bitmap);
	window_destroy_safe(window);
}

void win_profile_reload_data_and_mark_dirty(void) {
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void window_load(Window *window) {
	scroll_layer = scroll_layer_create(GRect(0, 52, 144, 120));
	scroll_layer_set_click_config_onto_window(scroll_layer, window);
	scroll_layer_set_content_size(scroll_layer, GSize(144, 120));
	scroll_layer_add_to_window(scroll_layer, window);

	likes_bitmap_layer = bitmap_layer_create(GRect(127, 10, 16, 12));
	bitmap_layer_set_bitmap(likes_bitmap_layer, likes_bitmap);
	bitmap_layer_set_compositing_mode(likes_bitmap_layer, GCompOpAssignInverted);
	bitmap_layer_add_to_window(likes_bitmap_layer, window);

	friends_bitmap_layer = bitmap_layer_create(GRect(127, 34, 16, 10));
	bitmap_layer_set_bitmap(friends_bitmap_layer, friends_bitmap);
	bitmap_layer_set_compositing_mode(friends_bitmap_layer, GCompOpAssignInverted);
	bitmap_layer_add_to_window(friends_bitmap_layer, window);

	name_text_layer = text_layer_create(GRect(4, 0, 136, 28));
	text_layer_set_system_font(name_text_layer, FONT_KEY_GOTHIC_24_BOLD);
	text_layer_set_text(name_text_layer, recs_get()->name);
	text_layer_set_colors(name_text_layer, GColorWhite, GColorClear);
	text_layer_add_to_window(name_text_layer, window);

	distance_text_layer = text_layer_create(GRect(4, 26, 136, 22));
	text_layer_set_system_font(distance_text_layer, FONT_KEY_GOTHIC_18);
	text_layer_set_text(distance_text_layer, recs_get()->distance);
	text_layer_set_colors(distance_text_layer, GColorWhite, GColorClear);
	text_layer_add_to_window(distance_text_layer, window);

	bio_text_layer = text_layer_create(GRect(4, -2, 136, 1024));
	text_layer_set_system_font(bio_text_layer, FONT_KEY_GOTHIC_18_BOLD);
	text_layer_set_text(bio_text_layer, recs_get()->bio);
	text_layer_set_colors(bio_text_layer, GColorWhite, GColorClear);
	scroll_layer_add_child(scroll_layer, text_layer_get_layer(bio_text_layer));

	commlikes_text_layer = text_layer_create(GRect(98, 4, 26, 22));
	text_layer_set_system_font(commlikes_text_layer, FONT_KEY_GOTHIC_18_BOLD);
	text_layer_set_text(commlikes_text_layer, recs_get()->common_likes);
	text_layer_set_colors(commlikes_text_layer, GColorWhite, GColorClear);
	text_layer_set_text_alignment(commlikes_text_layer, GTextAlignmentRight);
	text_layer_add_to_window(commlikes_text_layer, window);

	commfriends_text_layer = text_layer_create(GRect(98, 26, 26, 22));
	text_layer_set_system_font(commfriends_text_layer, FONT_KEY_GOTHIC_18_BOLD);
	text_layer_set_text(commfriends_text_layer, recs_get()->common_friends);
	text_layer_set_colors(commfriends_text_layer, GColorWhite, GColorClear);
	text_layer_set_text_alignment(commfriends_text_layer, GTextAlignmentRight);
	text_layer_add_to_window(commfriends_text_layer, window);
}

static void window_unload(Window *window) {
	scroll_layer_destroy_safe(scroll_layer);
	bitmap_layer_destroy(likes_bitmap_layer);
	bitmap_layer_destroy(friends_bitmap_layer);
	text_layer_destroy_safe(name_text_layer);
	text_layer_destroy_safe(distance_text_layer);
	text_layer_destroy_safe(bio_text_layer);
	text_layer_destroy_safe(commlikes_text_layer);
	text_layer_destroy_safe(commfriends_text_layer);
}
