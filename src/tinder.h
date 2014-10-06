#pragma once

#include <pebble.h>

typedef struct {
	char name[20];
	char distance[16];
	char bio[512];
	char common_likes[4];
	char common_friends[4];
	GBitmap* image;
} Recommendation;

void tinder_init(void);
void tinder_deinit(void);
void tinder_in_received_handler(DictionaryIterator *iter);
void tinder_out_failed_handler(DictionaryIterator *failed, AppMessageResult reason);
void tinder_reset();
void tinder_reset_image();
void tinder_request_rec();
void tinder_like();
void tinder_pass();
void tinder_reload_data_and_mark_dirty();
Recommendation* recs_get();
char* tinder_get_error();
