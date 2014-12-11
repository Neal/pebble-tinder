#include <pebble.h>
#include "tinder.h"
#include "libs/pebble-assist.h"
#include "generated/appinfo.h"
#include "generated/keys.h"
#include "appmessage.h"
#include "windows/win-recs.h"
#include "windows/win-error.h"

#define IMAGE_DATA_SIZE (sizeof(uint8_t) * (5 * 4) * 168 + 12)

static void send_method(uint8_t method);
static void buffersize_timer_callback(void *data);
static AppTimer* buffersize_timer = NULL;

static Recommendation* recs = NULL;

static uint8_t* image_data = NULL;

static char* error = NULL;

void tinder_init(void) {
	appmessage_init();

	buffersize_timer = app_timer_register(1000, buffersize_timer_callback, NULL);

	image_data = malloc(IMAGE_DATA_SIZE);

	recs = malloc(sizeof(Recommendation));
	tinder_reset();

	win_recs_init();
	win_error_init();
}

void tinder_deinit(void) {
	gbitmap_destroy_safe(recs_get()->image);
	free_safe(error);
	free_safe(recs);
	free_safe(image_data);
	win_error_deinit();
	win_recs_deinit();
}

void tinder_in_received_handler(DictionaryIterator *iter) {
	if (!dict_find(iter, APP_KEY_TYPE)) return;
	switch (dict_find(iter, APP_KEY_TYPE)->value->uint8) {
		case KEY_TYPE_ERROR: {
			free_safe(error);
			error = malloc(dict_find(iter, APP_KEY_NAME)->length);
			strncpy(error, dict_find(iter, APP_KEY_NAME)->value->cstring, dict_find(iter, APP_KEY_NAME)->length);
			win_error_push();
			win_recs_loading_stop();
			break;
		}
		case KEY_TYPE_REC: {
			if (!dict_find(iter, APP_KEY_METHOD)) return;
			switch (dict_find(iter, APP_KEY_METHOD)->value->uint8) {
				case KEY_METHOD_DATA: {
					if (dict_find(iter, APP_KEY_COMMLIKES)) {
						snprintf(recs_get()->common_likes, sizeof(recs_get()->common_likes) - 1, "%hu",  dict_find(iter, APP_KEY_COMMLIKES)->value->uint16);
					}
					if (dict_find(iter, APP_KEY_COMMFRIENDS)) {
						snprintf(recs_get()->common_friends, sizeof(recs_get()->common_friends) - 1, "%hu",  dict_find(iter, APP_KEY_COMMFRIENDS)->value->uint16);
					}
					if (dict_find(iter, APP_KEY_NAME)) {
						strncpy(recs_get()->name, dict_find(iter, APP_KEY_NAME)->value->cstring, sizeof(recs_get()->name) - 1);
					}
					if (dict_find(iter, APP_KEY_DISTANCE)) {
						strncpy(recs_get()->distance, dict_find(iter, APP_KEY_DISTANCE)->value->cstring, sizeof(recs_get()->distance) - 1);
					}
					if (dict_find(iter, APP_KEY_BIO)) {
						strncpy(recs_get()->bio, dict_find(iter, APP_KEY_BIO)->value->cstring, sizeof(recs_get()->bio) - 1);
					}
					break;
				}
				case KEY_METHOD_END:
					break;
			}
			break;
		}
		case KEY_TYPE_IMAGE: {
			if (!dict_find(iter, APP_KEY_METHOD)) return;
			switch (dict_find(iter, APP_KEY_METHOD)->value->uint8) {
				case KEY_METHOD_BEGIN:
					break;
				case KEY_METHOD_DATA: {
					int32_t index = dict_find(iter, APP_KEY_INDEX)->value->int32;
					memcpy(image_data + index, &dict_find(iter, APP_KEY_IMAGE)->value->uint8, dict_find(iter, APP_KEY_IMAGE)->length);
					gbitmap_destroy_safe(recs_get()->image);
					recs_get()->image = gbitmap_create_with_data(image_data);
					tinder_reload_data_and_mark_dirty();
					break;
				}
				case KEY_METHOD_END:
					win_recs_loading_stop();
					tinder_reload_data_and_mark_dirty();
					break;
			}
			break;
		}
	}
}

void tinder_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason) {
	free_safe(error);
	error = malloc(sizeof(char) * 56);
	strncpy(error, "Phone unreachable! Make sure the Pebble app is running.", 55);
	win_error_push();
	win_recs_loading_stop();
}

void tinder_reset() {
	strncpy(recs_get()->name, "", sizeof(recs_get()->name) - 1);
	strncpy(recs_get()->distance, "", sizeof(recs_get()->distance) - 1);
	strncpy(recs_get()->bio, "", sizeof(recs_get()->bio) - 1);
	strncpy(recs_get()->common_likes, "", sizeof(recs_get()->common_likes) - 1);
	strncpy(recs_get()->common_friends, "", sizeof(recs_get()->common_friends) - 1);
	tinder_reset_image();
}

void tinder_reset_image() {
	free_safe(recs_get()->image);
	memset(image_data, 0, IMAGE_DATA_SIZE);
	recs_get()->image = gbitmap_create_with_data(image_data);
}

void tinder_request_rec() {
	send_method(KEY_METHOD_REQUESTREC);
}

void tinder_like() {
	send_method(KEY_METHOD_LIKE);
}

void tinder_pass() {
	send_method(KEY_METHOD_PASS);
}

void tinder_reload_data_and_mark_dirty() {
	win_recs_reload_data_and_mark_dirty();
}

Recommendation* recs_get() {
	return recs;
}

char* tinder_get_error() {
	return error;
}

static void send_method(uint8_t method) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, APP_KEY_METHOD, method);
	dict_write_end(iter);
	app_message_outbox_send();
}

static void buffersize_timer_callback(void *data) {
	DictionaryIterator *iter;
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, APP_KEY_METHOD, KEY_METHOD_BUFFERSIZE);
	dict_write_uint32(iter, APP_KEY_INDEX, app_message_inbox_size_maximum());
	dict_write_end(iter);
	app_message_outbox_send();
}
