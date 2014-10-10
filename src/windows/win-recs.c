#include <pebble.h>
#include "win-recs.h"
#include "libs/pebble-assist.h"
#include "tinder.h"
#include "win-profile.h"
#include "win-myprofile.h"
#include "win-messages.h"
#include "win-settings.h"
#include "win-about.h"
#include "win-options.h"

static void loading_layer_update_callback(Layer *layer, GContext* ctx);
static void destroy_property_animation(PropertyAnimation **loading_prop_animation);
static void animation_started(Animation *animation, void *data);
static void animation_stopped(Animation *animation, bool finished, void *data);
static void select_single_click_handler(ClickRecognizerRef recognizer, Window *window);
static void select_long_click_handler(ClickRecognizerRef recognizer, Window *window);
static void up_single_click_handler(ClickRecognizerRef recognizer, Window *window);
static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window);
static void click_config_provider(Window *window);
static void window_load(Window *window);
static void window_unload(Window *window);
static void loading_timer_callback(void *data);

static Window* window = NULL;
static TextLayer* name_text_layer = NULL;
static ActionBarLayer* action_bar_layer = NULL;
static BitmapLayer* image_bitmap_layer = NULL;
static BitmapLayer* logo_bitmap_layer = NULL;
static GBitmap* logo_bitmap = NULL;
static GBitmap* like_bitmap = NULL;
static GBitmap* info_bitmap = NULL;
static GBitmap* pass_bitmap = NULL;
static Layer* loading_layer = NULL;
static AppTimer* loading_timer = NULL;
static PropertyAnimation* loading_prop_animation = NULL;
static PropertyAnimation* actionbar_prop_animation = NULL;

static bool is_loading = false;
static bool is_image_layer_visible = false;

static uint8_t loading_current_fill = 0;

void win_recs_init(void) {
	logo_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LOGO);
	like_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_LIKE);
	info_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_INFO);
	pass_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PASS);

	window = window_create();
	window_set_fullscreen(window, true);
	window_set_background_color(window, GColorBlack);
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(window, true);

	win_recs_loading_start();

	win_profile_init();
	win_myprofile_init();
	win_messages_init();
	win_settings_init();
	win_about_init();
	win_options_init();
}

void win_recs_deinit(void) {
	gbitmap_destroy_safe(logo_bitmap);
	gbitmap_destroy_safe(like_bitmap);
	gbitmap_destroy_safe(info_bitmap);
	gbitmap_destroy_safe(pass_bitmap);
	destroy_property_animation(&actionbar_prop_animation);
	destroy_property_animation(&loading_prop_animation);
	win_options_deinit();
	win_about_deinit();
	win_settings_deinit();
	win_messages_deinit();
	win_myprofile_deinit();
	win_profile_deinit();
	window_destroy_safe(window);
}

void win_recs_reload_data_and_mark_dirty(void) {
	text_layer_mark_dirty(name_text_layer);
	bitmap_layer_mark_dirty(image_bitmap_layer);
	if (!is_image_layer_visible) {
		destroy_property_animation(&loading_prop_animation);
		loading_prop_animation = property_animation_create_layer_frame(bitmap_layer_get_layer(image_bitmap_layer), NULL, &GRect(0, 0, 144, 144));
		animation_set_duration((Animation*) loading_prop_animation, 0);
		animation_set_curve((Animation*) loading_prop_animation, AnimationCurveEaseOut);
		animation_schedule((Animation*) loading_prop_animation);
		is_image_layer_visible = true;
	}
}

void win_recs_loading_start(void) {
	if (is_loading) return;
	loading_current_fill = 0;
	loading_timer = app_timer_register(0, loading_timer_callback, NULL);
	is_loading = true;
}

void win_recs_loading_stop(void) {
	app_timer_cancel(loading_timer);
	is_loading = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - //

static void loading_layer_update_callback(Layer *layer, GContext* ctx) {
	if (!is_loading) return;
	graphics_context_set_stroke_color(ctx, GColorWhite);
	graphics_context_set_fill_color(ctx, GColorWhite);
	graphics_draw_circle(ctx, GPoint(48, 8), 7);
	graphics_draw_circle(ctx, GPoint(72, 8), 7);
	graphics_draw_circle(ctx, GPoint(96, 8), 7);
	graphics_fill_circle(ctx, GPoint(48 + loading_current_fill * 24, 8), 5);
	loading_current_fill++;
	if (loading_current_fill > 2) loading_current_fill = 0;
}

static void destroy_property_animation(PropertyAnimation **loading_prop_animation) {
	if (*loading_prop_animation == NULL) return;
	if (animation_is_scheduled((Animation*) *loading_prop_animation)) {
		animation_unschedule((Animation*) *loading_prop_animation);
	}
	property_animation_destroy(*loading_prop_animation);
	*loading_prop_animation = NULL;
}

static void animation_started(Animation *animation, void *data) {
	strncpy(recs_get()->name, "", sizeof(recs_get()->name) - 1);
}

static void animation_stopped(Animation *animation, bool finished, void *data) {
	win_recs_loading_start();
	tinder_reset_image();
	is_image_layer_visible = false;
}

static void select_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	if (strlen(recs_get()->name)) {
		win_profile_push();
	}
}

static void select_long_click_handler(ClickRecognizerRef recognizer, Window *window) {
	// win_options_push();
}

static void up_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	if (!strlen(recs_get()->name)) return;
	tinder_like();
 	destroy_property_animation(&loading_prop_animation);
	loading_prop_animation = property_animation_create_layer_frame(bitmap_layer_get_layer(image_bitmap_layer), NULL, &GRect(144, 0, 144, 144));
	animation_set_duration((Animation*) loading_prop_animation, 500);
	animation_set_curve((Animation*) loading_prop_animation, AnimationCurveEaseOut);
	animation_set_handlers((Animation*) loading_prop_animation, (AnimationHandlers) {
		.started = (AnimationStartedHandler) animation_started,
		.stopped = (AnimationStoppedHandler) animation_stopped,
	}, NULL);
	animation_schedule((Animation*) loading_prop_animation);
}

static void down_single_click_handler(ClickRecognizerRef recognizer, Window *window) {
	if (!strlen(recs_get()->name)) return;
	tinder_pass();
 	destroy_property_animation(&loading_prop_animation);
	loading_prop_animation = property_animation_create_layer_frame(bitmap_layer_get_layer(image_bitmap_layer), NULL, &GRect(-144, 0, 144, 144));
	animation_set_duration((Animation*) loading_prop_animation, 500);
	animation_set_curve((Animation*) loading_prop_animation, AnimationCurveEaseOut);
	animation_set_handlers((Animation*) loading_prop_animation, (AnimationHandlers) {
		.started = (AnimationStartedHandler) animation_started,
		.stopped = (AnimationStoppedHandler) animation_stopped,
	}, NULL);
	animation_schedule((Animation*) loading_prop_animation);
}

static void click_config_provider(Window *window) {
	window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) select_single_click_handler);
	window_long_click_subscribe(BUTTON_ID_SELECT, 500, (ClickHandler) select_long_click_handler, NULL);
	window_single_click_subscribe(BUTTON_ID_UP, (ClickHandler) up_single_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, (ClickHandler) down_single_click_handler);
}

static void window_load(Window *window) {
	logo_bitmap_layer = bitmap_layer_create(GRect(40, 32, 64, 64));
	bitmap_layer_set_bitmap(logo_bitmap_layer, logo_bitmap);
	bitmap_layer_set_compositing_mode(logo_bitmap_layer, GCompOpAssignInverted);
	bitmap_layer_add_to_window(logo_bitmap_layer, window);

	action_bar_layer_create_in_window(action_bar_layer, window);
	action_bar_layer_set_click_config_provider(action_bar_layer, (ClickConfigProvider) click_config_provider);
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_UP, like_bitmap);
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_SELECT, info_bitmap);
	action_bar_layer_set_icon(action_bar_layer, BUTTON_ID_DOWN, pass_bitmap);

	actionbar_prop_animation = property_animation_create_layer_frame(action_bar_layer_get_layer(action_bar_layer), NULL, &GRect(144, 2, 20, 140));
	animation_set_duration((Animation*) actionbar_prop_animation, 500);
	animation_set_delay((Animation*) actionbar_prop_animation, 3000);
	animation_set_curve((Animation*) actionbar_prop_animation, AnimationCurveEaseOut);
	animation_schedule((Animation*) actionbar_prop_animation);

	loading_layer = layer_create(GRect(0, 108, 144, 24));
	layer_set_update_proc(loading_layer, loading_layer_update_callback);
	layer_add_to_window(loading_layer, window);

	name_text_layer = text_layer_create(GRect(5, 139, 134, 28));
	text_layer_set_system_font(name_text_layer, FONT_KEY_GOTHIC_24_BOLD);
	text_layer_set_text(name_text_layer, recs_get()->name);
	text_layer_set_colors(name_text_layer, GColorWhite, GColorClear);
	text_layer_add_to_window(name_text_layer, window);

	image_bitmap_layer = bitmap_layer_create(GRect(0, -144, 144, 144));
	bitmap_layer_set_bitmap(image_bitmap_layer, recs_get()->image);
	bitmap_layer_set_alignment(image_bitmap_layer, GAlignCenter);
	bitmap_layer_set_background_color(image_bitmap_layer, GColorBlack);
	bitmap_layer_add_to_window(image_bitmap_layer, window);
}

static void window_unload(Window *window) {
	layer_destroy_safe(loading_layer);
	action_bar_layer_destroy_safe(action_bar_layer);
	bitmap_layer_destroy_safe(logo_bitmap_layer);
	bitmap_layer_destroy_safe(image_bitmap_layer);
	text_layer_destroy_safe(name_text_layer);
}

static void loading_timer_callback(void *data) {
	layer_mark_dirty(loading_layer);
	loading_timer = app_timer_register(500, loading_timer_callback, NULL);
}
